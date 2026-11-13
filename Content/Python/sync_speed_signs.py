import unreal
import math


def dist_3d(a, b):
    pa, pb = a["world_loc"], b["world_loc"]
    return math.dist([pa.x, pa.y, pa.z], [pb.x, pb.y, pb.z])


def get_all_landscape_control_points(splines_comp, max_scan_id=5000):
    """
    Recupera todos os objetos ULandscapeSplineControlPoint válidos pertencentes
    ao ULandscapeSplinesComponent informado.
    """
    control_points = []
    comp_transform = splines_comp.get_world_transform()

    for idx in range(max_scan_id):
        cp_name = f"LandscapeSplineControlPoint_{idx}"
        cp = unreal.find_object(splines_comp, cp_name)
        if cp is not None and unreal.SystemLibrary.is_valid(cp):
            try:
                local_loc = cp.get_editor_property("location")
                local_rot = cp.get_editor_property("rotation")
                half_width = float(cp.get_editor_property("width"))
                world_loc = comp_transform.transform_location(local_loc)
                control_points.append({
                    "id": idx,
                    "name": cp_name,
                    "object": cp,
                    "world_loc": world_loc,
                    "local_rot": local_rot,
                    "half_width": half_width,
                })
            except Exception as e:
                unreal.log_warning(f"Erro ao ler {cp_name}: {e}")

    return control_points


def split_and_order_chains(control_points, max_gap_cm=25000.0):
    """
    Agrupa e ordena os Control Points em cadeias contínuas (rotas/trechos),
    conectando a qualquer cadeia existente cujas extremidades estejam a menos de
    max_gap_cm (250m) e descartando pontos órfãos isolados.
    """
    if not control_points:
        return []

    chains = [[control_points[0]]]

    for i in range(1, len(control_points)):
        curr_pt = control_points[i]
        placed = False

        for ch in reversed(chains):
            if dist_3d(ch[-1], curr_pt) <= max_gap_cm:
                ch.append(curr_pt)
                placed = True
                break
            elif dist_3d(ch[0], curr_pt) <= max_gap_cm:
                ch.insert(0, curr_pt)
                placed = True
                break

        if not placed:
            chains.append([curr_pt])

    valid_chains = [ch for ch in chains if len(ch) >= 2]
    return valid_chains


def project_to_ground(world, x, y, ref_z, trace_up=2500.0, trace_down=2500.0):
    """
    Faz um Line Trace vertical para encontrar a cota exata Z do terreno (Landscape)
    na posição (x, y) onde a base do poste da placa será apoiada.
    """
    start = unreal.Vector(x, y, ref_z + trace_up)
    end = unreal.Vector(x, y, ref_z - trace_down)
    try:
        hit = unreal.SystemLibrary.line_trace_single(
            world,
            start,
            end,
            unreal.TraceTypeQuery.TRACE_TYPE_QUERY1,
            True,
            [],
            unreal.DrawDebugTrace.NONE,
            True,
        )
        if hit is not None:
            if isinstance(hit, tuple):
                b_hit, out_hit = hit[0], hit[1]
                if b_hit:
                    return out_hit.impact_point.z
            elif hasattr(hit, "impact_point"):
                if hit.impact_point.z != 0.0:
                    return hit.impact_point.z
    except Exception:
        pass
    return ref_z


def sample_chain_at_distance(chain, target_dist_cm):
    """
    Interpola ao longo da polilinha da cadeia (em 0 <= target_dist_cm <= total_len)
    retornando:
      - pos (unreal.Vector)
      - forward_dir (vetor unitário 2D no sentido 0 -> fim)
      - half_width (interpolado entre os pontos)
      - seg_idx (índice do ponto inicial do segmento)
    """
    cum_d = 0.0
    for i in range(len(chain) - 1):
        p0 = chain[i]["world_loc"]
        p1 = chain[i + 1]["world_loc"]
        seg_len = math.dist([p0.x, p0.y, p0.z], [p1.x, p1.y, p1.z])
        if seg_len < 1e-3:
            continue

        if cum_d + seg_len >= target_dist_cm or i == len(chain) - 2:
            alpha = max(0.0, min(1.0, (target_dist_cm - cum_d) / seg_len))
            x = p0.x + (p1.x - p0.x) * alpha
            y = p0.y + (p1.y - p0.y) * alpha
            z = p0.z + (p1.z - p0.z) * alpha

            dx = p1.x - p0.x
            dy = p1.y - p0.y
            mag2d = math.hypot(dx, dy)
            fx, fy = (dx / mag2d, dy / mag2d) if mag2d > 1e-4 else (1.0, 0.0)

            hw0 = chain[i]["half_width"]
            hw1 = chain[i + 1]["half_width"]
            hw = hw0 + (hw1 - hw0) * alpha

            return unreal.Vector(x, y, z), (fx, fy), hw, i

        cum_d += seg_len

    p_last = chain[-1]["world_loc"]
    return p_last, (1.0, 0.0), chain[-1]["half_width"], len(chain) - 2


def compute_chain_metrics(chain):
    """
    Calcula distâncias acumuladas e inclinação (grade %) de cada segmento da cadeia.
    """
    cum_dists = [0.0]
    seg_grades = []  # grade (%) indo de i para i+1

    for i in range(len(chain) - 1):
        p0 = chain[i]["world_loc"]
        p1 = chain[i + 1]["world_loc"]
        d3 = math.dist([p0.x, p0.y, p0.z], [p1.x, p1.y, p1.z])
        d2 = max(1.0, math.hypot(p1.x - p0.x, p1.y - p0.y))
        dz = p1.z - p0.z
        grade_pct = (dz / d2) * 100.0

        cum_dists.append(cum_dists[-1] + d3)
        seg_grades.append(grade_pct)

    return cum_dists, seg_grades


def spawn_sign_at_chain_distance(
    actor_subsystem,
    world,
    chain,
    dist_along_chain_cm,
    travel_direction,  # +1: caminhão indo de chain[0] -> chain[-1] | -1: indo de chain[-1] -> chain[0]
    mesh_asset,
    sign_label,
    folder_path="TrafficSigns_Auto",
    outside_pontalete_offset_cm=120.0,  # 1.2m para fora da linha dos pontaletes
):
    """
    Posiciona uma placa (StaticMeshActor) na margem DIREITA do sentido de tráfego informado,
    fora da linha dos pontaletes (HalfWidth + outside_pontalete_offset_cm), com a face (+X)
    voltada diretamente para o motorista que se aproxima e poste 100% vertical (Roll=0, Pitch=0).
    """
    center_pos, (fx, fy), half_width, seg_idx = sample_chain_at_distance(chain, dist_along_chain_cm)

    # Direção de deslocamento do veículo (vx, vy)
    vx = fx * travel_direction
    vy = fy * travel_direction

    # No sistema Left-Handed da Unreal Engine (+X Forward, +Y Right):
    # Se Forward = (vx, vy), o vetor Right (mão direita do motorista) é (-vy, vx)
    rx = -vy
    ry = vx

    lateral_dist = half_width + outside_pontalete_offset_cm
    sign_x = center_pos.x + rx * lateral_dist
    sign_y = center_pos.y + ry * lateral_dist
    sign_z = project_to_ground(world, sign_x, sign_y, center_pos.z)

    # A face pintada da placa aponta para +X local.
    # Para olhar de frente para o motorista que vem na direção (vx, vy), +X deve apontar para (-vx, -vy).
    yaw_deg = math.degrees(math.atan2(-vy, -vx))

    # IMPORTANTE: Em UE5 Python, unreal.Rotator(a, b, c) usa a ordem (Roll, Pitch, Yaw)!
    # Por isso atribuímos explicitamente roll=0, pitch=0, yaw=yaw_deg para a placa nunca deitar!
    sign_rot = unreal.Rotator()
    sign_rot.roll = 0.0
    sign_rot.pitch = 0.0
    sign_rot.yaw = float(yaw_deg)

    sign_loc = unreal.Vector(sign_x, sign_y, sign_z)

    sign_actor = actor_subsystem.spawn_actor_from_class(unreal.StaticMeshActor, sign_loc, sign_rot)
    # Garantir a rotação após o spawn
    sign_actor.set_actor_rotation(sign_rot, False)
    sign_actor.set_actor_label(sign_label)

    try:
        sign_actor.set_folder_path(folder_path)
    except Exception:
        pass

    try:
        sign_actor.tags = [unreal.Name("AutoSpeedSign")]
    except Exception:
        pass

    sm_comp = sign_actor.get_component_by_class(unreal.StaticMeshComponent)
    if sm_comp and mesh_asset:
        sm_comp.set_editor_property("mobility", unreal.ComponentMobility.MOVABLE)
        sm_comp.set_static_mesh(mesh_asset)
        sm_comp.set_editor_property("mobility", unreal.ComponentMobility.STATIC)

    print(
        f"  + [{sign_label}] dist={dist_along_chain_cm/100.0:6.1f}m | "
        f"Loc=({sign_x:.0f}, {sign_y:.0f}, {sign_z:.0f}) | Yaw={yaw_deg:6.1f}° (Roll=0, Pitch=0)"
    )
    return sign_actor


def sync_speed_signs(
    placa_20_path="/Game/Sirv/Static/props/Placas/placa_20",
    placa_50_path="/Game/Sirv/Static/props/Placas/placa_50",
    outside_offset_cm=120.0,           # 1.2m para fora da linha dos pontaletes
    plaza_entry_advance_cm=3000.0,     # 30m antes da extremidade da spline (chegada na praça/britador)
    plaza_exit_offset_cm=3000.0,       # 30m após sair da extremidade da spline (saída p/ via de 50 km/h)
    downhill_grade_threshold=-3.5,     # Declive <= -3.5% no sentido carregado dispara zona de Descida Carregado (20 km/h)
    min_downhill_length_cm=8000.0,     # Mínimo de 80m de rampa para considerar uma descida real
    repeat_50_every_cm=50000.0,        # Repetir placa de 50 km/h a cada 500m em trechos longos
):
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    unreal_editor_subsystem = unreal.get_editor_subsystem(unreal.UnrealEditorSubsystem)
    world = unreal_editor_subsystem.get_editor_world()
    all_actors = actor_subsystem.get_all_level_actors()

    # 1. Carregar StaticMeshes das placas 20 e 50 km/h
    mesh_20 = unreal.EditorAssetLibrary.load_asset(placa_20_path)
    mesh_50 = unreal.EditorAssetLibrary.load_asset(placa_50_path)

    if not mesh_20 or not mesh_50:
        unreal.log_error(f"Não foi possível carregar as malhas de placa em {placa_20_path} ou {placa_50_path}!")
        return

    # 2. Remover placas automáticas geradas em execuções anteriores
    removed_count = 0
    for actor in all_actors:
        label = actor.get_actor_label()
        has_tag = False
        try:
            has_tag = unreal.Name("AutoSpeedSign") in actor.tags
        except Exception:
            pass
        if has_tag or label.startswith("PlacaVel_R"):
            actor_subsystem.destroy_actor(actor)
            removed_count += 1

    if removed_count > 0:
        print(f"[SpeedSigns] {removed_count} placas automáticas anteriores removidas.")

    # 3. Localizar a Landscape Spline no nível
    splines_comp = None
    for actor in actor_subsystem.get_all_level_actors():
        comps = actor.get_components_by_class(unreal.LandscapeSplinesComponent)
        if comps:
            splines_comp = comps[0]
            break

    if not splines_comp:
        unreal.log_error("Nenhuma Landscape Spline encontrada no nível atual!")
        return

    cps = get_all_landscape_control_points(splines_comp)
    chains = split_and_order_chains(cps)

    print(f"\n=====================================================================")
    print(f" SINCRONIZAÇÃO DE PLACAS DE VELOCIDADE ({len(cps)} Control Points -> {len(chains)} Rotas)")
    print(f"=====================================================================")

    total_spawned = 0

    for chain_idx, chain in enumerate(chains):
        route_num = chain_idx + 1
        cum_dists, seg_grades = compute_chain_metrics(chain)
        total_len = cum_dists[-1]

        p_start = chain[0]["world_loc"]
        p_end = chain[-1]["world_loc"]

        # Definir a semântica exata de cada rota com base nas coordenadas reais do nível:
        # - Rota 01 (43 pontos, 2373.9m):
        #   * Ponto [00] (X=6629, Y=-56984, Z=-56.2m, HW=900cm) = Britador Primário SBR (Descarga)
        #   * Ponto [17/18] (830.8m..873.5m, Z=-9.3m) = Crista / Topo da Rampa (fim da subida do Sapecado e início da descida p/ Britador)
        #   * Ponto [42] (X=-45067, Y=91560, Z=-136.9m, HW=1200cm) = Cava do Sapecado (Praça de Carga)
        #   -> Sentido Carregado: [42] Sapecado -> [00] Britador (loaded_dir = -1)
        #
        # - Rota 02 (25 pontos, 1884.4m):
        #   * Ponto [00] (X=-60655, Y=-388485, Z=-113.2m) = Praça das 6 Pilhas da Cianita (Descarga)
        #   * Ponto [24] (X=3777, Y=-284312, Z=-40.7m) = Pátio da Usina / Carregadeira L-1850 (Carga)
        #   -> Sentido Carregado: [24] Usina -> [00] Cianita (loaded_dir = -1)
        if route_num == 1 and len(chain) > 30:
            route_tag = "R01_Sapecado"
            loaded_dir = -1  # Carregado vai de [42] Sapecado -> [00] Britador
            name_pt0 = "Britador_Descarga"
            name_ptEnd = "Sapecado_Carga"
        else:
            route_tag = "R02_Cianita"
            loaded_dir = -1  # Carregado vai de [24] Usina -> [00] Cianita
            name_pt0 = "Cianita_Descarga"
            name_ptEnd = "Usina_Carga"

        print(f"\n--- {route_tag} ({len(chain)} pontos | {total_len/100.0:.1f} m) ---")
        print(f"  Ponto [00] ({name_pt0}): ({p_start.x:.0f}, {p_start.y:.0f}, {p_start.z:.0f})")
        print(f"  Ponto [{len(chain)-1:02d}] ({name_ptEnd}): ({p_end.x:.0f}, {p_end.y:.0f}, {p_end.z:.0f})")

        # =====================================================================
        # 1. ENTRADA DAS PRAÇAS / BRITADOR NAS DUAS PONTAS (20 km/h)
        # =====================================================================
        # Chegando no Extremo Final (sentido +1: 0 -> Fim): 20 km/h a 30m antes do fim
        d_arrive_end = max(1000.0, total_len - plaza_entry_advance_cm)
        spawn_sign_at_chain_distance(
            actor_subsystem, world, chain,
            dist_along_chain_cm=d_arrive_end,
            travel_direction=+1,
            mesh_asset=mesh_20,
            sign_label=f"PlacaVel_{route_tag}_20kmh_Entrada_{name_ptEnd}",
            outside_pontalete_offset_cm=outside_offset_cm,
        )
        total_spawned += 1

        # Chegando no Extremo Inicial [00] (sentido -1: Fim -> 0): 20 km/h a 30m antes do ponto 0
        d_arrive_start = min(total_len - 1000.0, plaza_entry_advance_cm)
        spawn_sign_at_chain_distance(
            actor_subsystem, world, chain,
            dist_along_chain_cm=d_arrive_start,
            travel_direction=-1,
            mesh_asset=mesh_20,
            sign_label=f"PlacaVel_{route_tag}_20kmh_Entrada_{name_pt0}",
            outside_pontalete_offset_cm=outside_offset_cm,
        )
        total_spawned += 1

        # =====================================================================
        # 2. SENTIDO VAZIO (Descarga -> Carga): Toda a estrada é 50 km/h até a entrada da Carga
        # =====================================================================
        empty_dir = -loaded_dir
        d_exit_unloaded = plaza_exit_offset_cm if empty_dir == +1 else (total_len - plaza_exit_offset_cm)
        name_unloaded_origin = name_pt0 if empty_dir == +1 else name_ptEnd

        # Placa de 50 km/h logo na saída da área de Descarga (caminhão vazio iniciando retorno)
        spawn_sign_at_chain_distance(
            actor_subsystem, world, chain,
            dist_along_chain_cm=d_exit_unloaded,
            travel_direction=empty_dir,
            mesh_asset=mesh_50,
            sign_label=f"PlacaVel_{route_tag}_50kmh_SaidaVazio_{name_unloaded_origin}",
            outside_pontalete_offset_cm=outside_offset_cm,
        )
        total_spawned += 1

        # Placas de confirmação de 50 km/h a cada 500m no sentido Vazio
        curr_d = (d_exit_unloaded + repeat_50_every_cm) if empty_dir == +1 else (d_exit_unloaded - repeat_50_every_cm)
        while 12000.0 < curr_d < (total_len - 12000.0):
            spawn_sign_at_chain_distance(
                actor_subsystem, world, chain,
                dist_along_chain_cm=curr_d,
                travel_direction=empty_dir,
                mesh_asset=mesh_50,
                sign_label=f"PlacaVel_{route_tag}_50kmh_Vazio_{int(curr_d/100)}m",
                outside_pontalete_offset_cm=outside_offset_cm,
            )
            total_spawned += 1
            curr_d += (repeat_50_every_cm * empty_dir)

        # =====================================================================
        # 3. SENTIDO CARREGADO (Carga -> Descarga): 50 km/h em plano/subida e 20 km/h em Descida Carregado
        # =====================================================================
        d_exit_loaded = plaza_exit_offset_cm if loaded_dir == +1 else (total_len - plaza_exit_offset_cm)
        name_loaded_origin = name_pt0 if loaded_dir == +1 else name_ptEnd
        seg_indices = list(range(len(chain) - 1)) if loaded_dir == +1 else list(range(len(chain) - 2, -1, -1))

        in_downhill = False
        last_sign_dist_loaded = d_exit_loaded

        # Verificar se logo na saída da praça de carga o caminhão já entra em descida carregado:
        first_seg = seg_indices[0]
        first_grade_loaded = seg_grades[first_seg] * loaded_dir

        if first_grade_loaded <= downhill_grade_threshold or route_tag == "R02_Cianita":
            # Na Rota 02 (Cianita), saindo do Pátio da Usina (Pt 24 -> Pt 22) já inicia a descida carregado no rejeito!
            # Colocamos 20 km/h logo na saída do pátio da Usina (dist = total_len - 30m)
            spawn_sign_at_chain_distance(
                actor_subsystem, world, chain,
                dist_along_chain_cm=d_exit_loaded,
                travel_direction=loaded_dir,
                mesh_asset=mesh_20,
                sign_label=f"PlacaVel_{route_tag}_20kmh_SaidaCarregadoEmDescida_{name_loaded_origin}",
                outside_pontalete_offset_cm=outside_offset_cm,
            )
            in_downhill = True
            total_spawned += 1
        else:
            # Na Rota 01 (Sapecado), saindo da Cava (Pt 00 -> Pt 17) o caminhão SOBE a rampa (+5% a +11.9%)
            # Portanto, ao sair da praça da escavadeira, entra em via de 50 km/h!
            spawn_sign_at_chain_distance(
                actor_subsystem, world, chain,
                dist_along_chain_cm=d_exit_loaded,
                travel_direction=loaded_dir,
                mesh_asset=mesh_50,
                sign_label=f"PlacaVel_{route_tag}_50kmh_SaidaCarregado_{name_loaded_origin}",
                outside_pontalete_offset_cm=outside_offset_cm,
            )
            in_downhill = False
            total_spawned += 1

        margin_cm = 8000.0
        for idx_pos, seg_i in enumerate(seg_indices):
            grade_loaded = seg_grades[seg_i] * loaded_dir
            seg_enter_dist = cum_dists[seg_i] if loaded_dir == +1 else cum_dists[seg_i + 1]

            if seg_enter_dist < margin_cm or seg_enter_dist > (total_len - margin_cm):
                continue

            if not in_downhill and grade_loaded <= downhill_grade_threshold:
                downhill_len = 0.0
                for look_ahead in seg_indices[idx_pos:]:
                    if seg_grades[look_ahead] * loaded_dir <= (downhill_grade_threshold + 1.5):
                        downhill_len += (cum_dists[look_ahead + 1] - cum_dists[look_ahead])
                    else:
                        break

                if downhill_len >= min_downhill_length_cm:
                    in_downhill = True
                    spawn_sign_at_chain_distance(
                        actor_subsystem, world, chain,
                        dist_along_chain_cm=seg_enter_dist,
                        travel_direction=loaded_dir,
                        mesh_asset=mesh_20,
                        sign_label=f"PlacaVel_{route_tag}_20kmh_TopoDescidaCarregado_Pt{seg_i:02d}",
                        outside_pontalete_offset_cm=outside_offset_cm,
                    )
                    last_sign_dist_loaded = seg_enter_dist
                    total_spawned += 1

            elif in_downhill and grade_loaded > -1.0:
                # Verificar se realmente encerrou a descida por um trecho longo (> 250m planos/subida)
                flat_len = 0.0
                for look_ahead in seg_indices[idx_pos:]:
                    if seg_grades[look_ahead] * loaded_dir > -2.0:
                        flat_len += (cum_dists[look_ahead + 1] - cum_dists[look_ahead])
                    else:
                        break

                rem_to_dest = (total_len - seg_enter_dist) if loaded_dir == +1 else seg_enter_dist
                if flat_len >= 25000.0 and rem_to_dest > 15000.0:
                    in_downhill = False
                    spawn_sign_at_chain_distance(
                        actor_subsystem, world, chain,
                        dist_along_chain_cm=seg_enter_dist,
                        travel_direction=loaded_dir,
                        mesh_asset=mesh_50,
                        sign_label=f"PlacaVel_{route_tag}_50kmh_FimDescida_Pt{seg_i:02d}",
                        outside_pontalete_offset_cm=outside_offset_cm,
                    )
                    last_sign_dist_loaded = seg_enter_dist
                    total_spawned += 1

            # Reforço a cada 500m no sentido carregado (20 km/h se estiver em descida longa, ou 50 km/h se em plano/subida)
            if abs(seg_enter_dist - last_sign_dist_loaded) >= repeat_50_every_cm:
                rem_to_dest = (total_len - seg_enter_dist) if loaded_dir == +1 else seg_enter_dist
                if rem_to_dest > 15000.0:
                    ref_mesh = mesh_20 if in_downhill else mesh_50
                    vel_str = "20kmh_ReforcoDescida" if in_downhill else "50kmh_ReforcoCarregado"
                    spawn_sign_at_chain_distance(
                        actor_subsystem, world, chain,
                        dist_along_chain_cm=seg_enter_dist,
                        travel_direction=loaded_dir,
                        mesh_asset=ref_mesh,
                        sign_label=f"PlacaVel_{route_tag}_{vel_str}_{int(seg_enter_dist/100)}m",
                        outside_pontalete_offset_cm=outside_offset_cm,
                    )
                    last_sign_dist_loaded = seg_enter_dist
                    total_spawned += 1

    print(f"\n=====================================================================")
    print(f" SUCESSO: {total_spawned} placas 100% verticais posicionadas no nível!")
    print(f"=====================================================================")
    unreal.log(f"[SpeedSigns] {total_spawned} placas posicionadas com sucesso (Roll=0, Pitch=0)!")


if __name__ == "__main__":
    sync_speed_signs()
