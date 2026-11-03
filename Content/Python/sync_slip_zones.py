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

    valid_chains = []
    for ch in chains:
        if len(ch) >= 2:
            valid_chains.append(ch)

    return valid_chains


def make_oriented_box_points(anchor_loc, dir_from, dir_to, back_cm, front_cm, half_width_cm, z_offset_cm=40.0):
    """
    Gera 4 vértices em sentido horário (retângulo orientado pela direção da chegada/saída da estrada)
    para iniciar uma zona poligonal limpa, sem linhas cruzadas.
    """
    dx = dir_to.x - dir_from.x
    dy = dir_to.y - dir_from.y
    mag = math.hypot(dx, dy)
    if mag < 1e-3:
        fx, fy = 1.0, 0.0
    else:
        fx, fy = dx / mag, dy / mag

    # Vetor Direita perpendicular no plano XY (Left-Handed UE: Right = (-fy, fx))
    rx, ry = -fy, fx
    z = anchor_loc.z + z_offset_cm

    # 4 cantos em ordem horária: Traseira-Esq -> Dianteira-Esq -> Dianteira-Dir -> Traseira-Dir
    p1 = unreal.Vector(
        anchor_loc.x - fx * back_cm - rx * half_width_cm,
        anchor_loc.y - fy * back_cm - ry * half_width_cm,
        z,
    )
    p2 = unreal.Vector(
        anchor_loc.x + fx * front_cm - rx * half_width_cm,
        anchor_loc.y + fy * front_cm - ry * half_width_cm,
        z,
    )
    p3 = unreal.Vector(
        anchor_loc.x + fx * front_cm + rx * half_width_cm,
        anchor_loc.y + fy * front_cm + ry * half_width_cm,
        z,
    )
    p4 = unreal.Vector(
        anchor_loc.x - fx * back_cm + rx * half_width_cm,
        anchor_loc.y - fy * back_cm + ry * half_width_cm,
        z,
    )
    return [p1, p2, p3, p4]


def spawn_polygon_slip_zone(
    actor_subsystem,
    bp_class,
    label,
    center_loc,
    corner_points,
    tags,
    default_speed_limit=10.0,
    default_load_mode="loaded,unloaded",
):
    """
    Cria um ator BP_SpeedZone com Spline fechada (Closed Loop = True), vértices lineares
    e Tags de deslizamento ('SlipZone', 'SlipZone_Area'), servindo simultaneamente para:
    - Limite de velocidade no YAML (ou ignorado no YAML caso SpeedLimit seja alterado para 0.0)
    - Zona de deslizamento/afundamento de rejeito na física do caminhão.
    """
    zone_actor = actor_subsystem.spawn_actor_from_class(bp_class, center_loc, unreal.Rotator(0, 0, 0))
    zone_actor.set_actor_label(label)

    try:
        zone_actor.tags = [unreal.Name(t) for t in tags]
    except Exception:
        pass

    for prop_name in ["ZoneCode", "zone_code"]:
        try:
            zone_actor.set_editor_property(prop_name, label)
        except Exception:
            pass

    for prop_name in ["SpeedLimit", "speed_limit"]:
        try:
            zone_actor.set_editor_property(prop_name, float(default_speed_limit))
        except Exception:
            pass

    for prop_name in ["LoadMode", "load_mode"]:
        try:
            zone_actor.set_editor_property(prop_name, str(default_load_mode))
        except Exception:
            pass

    spline_comp = zone_actor.get_component_by_class(unreal.SplineComponent)
    if spline_comp:
        spline_comp.clear_spline_points(False)
        for idx, pt in enumerate(corner_points):
            spline_comp.add_spline_point(pt, unreal.SplineCoordinateSpace.WORLD, False)
            spline_comp.set_spline_point_type(idx, unreal.SplinePointType.LINEAR, False)

        spline_comp.set_closed_loop(True, False)
        spline_comp.update_spline()

        try:
            spline_comp.set_editor_properties({
                "spline_has_been_edited": True,
                "input_spline_points_to_construction_script": True,
            })
        except Exception:
            pass

    return zone_actor


def sync_landscape_splines_to_slip_zones(
    speed_zone_bp_path="/Game/Blueprints/BP_SpeedZone",
    obsolete_slip_bp_path="/Game/Blueprints/BP_SlipZone",
    cianita_chain_index=1,          # Indice da Rota 02 (Cianita -> Rejeito)
    overwrite_plaza_zones=True,     # Se False, preserva ajustes manuais feitos nas zonas de Carga/Descarga
    plaza_speed_limit=10.0,         # Velocidade limite padrão nas praças (use 0.0 se não quiser exportar p/ YAML)
):
    """
    Configura a demarcação completa do circuito de Rejeito Filtrado (Cianita) usando BP_SpeedZone:
    1. Remove atores antigos que usavam a classe duplicada BP_SlipZone e deleta o asset BP_SlipZone redundante.
    2. Aplica a Tag 'SlipZone' e 'SlipZone_Road' diretamente no 'Pontalete_AutoSpline_02' existente.
    3. Cria 2 polígonos fechados usando a própria 'BP_SpeedZone' (com Tag 'SlipZone' e 'SlipZone_Area'):
       - 'SlipZone_Descarga_Cianita' (em chain[0] - Praça das 6 pilhas da Cianita)
       - 'SlipZone_Carga_Rejeito' (em chain[-1] - Pátio de carregamento da L-1850)
    """
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()

    # 1. Limpar atores antigos ou que ainda usavam a classe duplicada BP_SlipZone_C
    existing_plaza_zones = {}

    for actor in all_actors:
        label = actor.get_actor_label()
        class_name = actor.get_class().get_name()

        if label.startswith("SlipZone_AutoSpline_") or label.startswith("SlipZone_VisualBorder_") or "BP_SlipZone" in class_name:
            actor_subsystem.destroy_actor(actor)
        elif label in ("SlipZone_Carga_Rejeito", "SlipZone_Descarga_Cianita"):
            if overwrite_plaza_zones:
                actor_subsystem.destroy_actor(actor)
            else:
                existing_plaza_zones[label] = actor

    # Remover o asset duplicado BP_SlipZone do Content/Blueprints caso exista
    if unreal.EditorAssetLibrary.does_asset_exist(obsolete_slip_bp_path):
        try:
            if unreal.EditorAssetLibrary.delete_asset(obsolete_slip_bp_path):
                print(f"[SlipZone] Asset redundante '{obsolete_slip_bp_path}' removido com sucesso!")
        except Exception as e:
            unreal.log_warning(f"Não foi possível remover {obsolete_slip_bp_path}: {e}")

    # 2. Aplicar a Tag 'SlipZone' diretamente no Pontalete_AutoSpline_02 existente
    for actor in actor_subsystem.get_all_level_actors():
        label = actor.get_actor_label()
        if label == f"Pontalete_AutoSpline_{cianita_chain_index + 1:02d}":
            try:
                actor.tags = [unreal.Name("SlipZone"), unreal.Name("SlipZone_Road")]
                print(f"[SlipZone] Tag 'SlipZone' e 'SlipZone_Road' aplicadas com sucesso em '{label}'!")
            except Exception as e:
                unreal.log_warning(f"Erro ao aplicar Tag em {label}: {e}")

    # 3. Ler a Landscape Spline da Rota 02 para posicionar as praças de Descarga e Carga
    splines_comp = None
    for actor in actor_subsystem.get_all_level_actors():
        comps = actor.get_components_by_class(unreal.LandscapeSplinesComponent)
        if comps:
            splines_comp = comps[0]
            break

    if not splines_comp:
        unreal.log_error("Nenhuma Landscape Spline encontrada no nível!")
        return

    cps = get_all_landscape_control_points(splines_comp)
    chains = split_and_order_chains(cps)
    if not chains:
        unreal.log_warning("Nenhuma cadeia de pontos encontrada.")
        return

    chain_idx = cianita_chain_index if 0 <= cianita_chain_index < len(chains) else (len(chains) - 1)
    chain = chains[chain_idx]

    bp_asset = unreal.EditorAssetLibrary.load_asset(speed_zone_bp_path)
    if not bp_asset:
        unreal.log_error(f"Não foi possível carregar '{speed_zone_bp_path}'!")
        return
    bp_class = bp_asset.generated_class()

    # =========================================================================
    # A. ZONA DE DESCARGA (Início da Rota 02: chain[0] - Praça das 6 Pilhas da Cianita)
    # =========================================================================
    if "SlipZone_Descarga_Cianita" not in existing_plaza_zones:
        p0 = chain[0]["world_loc"]
        p1 = chain[1]["world_loc"]
        descarga_corners = make_oriented_box_points(
            anchor_loc=p0,
            dir_from=p0,
            dir_to=p1,
            back_cm=6000.0,       # 60m para dentro da praça das 6 pilhas (antes do ponto 0)
            front_cm=2000.0,      # 20m para frente (cobrindo a transição com o 1º pontalete)
            half_width_cm=4000.0, # 40m para cada lado (80m de largura total)
        )
        spawn_polygon_slip_zone(
            actor_subsystem=actor_subsystem,
            bp_class=bp_class,
            label="SlipZone_Descarga_Cianita",
            center_loc=p0,
            corner_points=descarga_corners,
            tags=["SlipZone", "SlipZone_Area", "SlipZone_Descarga"],
            default_speed_limit=plaza_speed_limit,
        )
        print(f" -> Criado 'SlipZone_Descarga_Cianita' (Class=BP_SpeedZone) em chain[0]: ({p0.x:.0f}, {p0.y:.0f}, {p0.z:.0f})")

    # =========================================================================
    # B. ZONA DE CARGA (Fim da Rota 02: chain[-1] - Pilha de Rejeito / L-1850)
    # =========================================================================
    if "SlipZone_Carga_Rejeito" not in existing_plaza_zones:
        p_prev = chain[-2]["world_loc"]
        p_end = chain[-1]["world_loc"]
        carga_corners = make_oriented_box_points(
            anchor_loc=p_end,
            dir_from=p_prev,
            dir_to=p_end,
            back_cm=2000.0,       # 20m antes do último pontalete
            front_cm=4500.0,      # 45m à frente (adentrando o pátio de carga da L-1850)
            half_width_cm=3500.0, # 35m para cada lado (70m de largura total)
        )
        spawn_polygon_slip_zone(
            actor_subsystem=actor_subsystem,
            bp_class=bp_class,
            label="SlipZone_Carga_Rejeito",
            center_loc=p_end,
            corner_points=carga_corners,
            tags=["SlipZone", "SlipZone_Area", "SlipZone_Carga"],
            default_speed_limit=plaza_speed_limit,
        )
        print(f" -> Criado 'SlipZone_Carga_Rejeito' (Class=BP_SpeedZone) em chain[-1]: ({p_end.x:.0f}, {p_end.y:.0f}, {p0.z:.0f})")

    unreal.log("SUCESSO: Zonas de rejeito criadas usando diretamente BP_SpeedZone e asset redundante BP_SlipZone removido!")


if __name__ == "__main__":
    sync_landscape_splines_to_slip_zones()
