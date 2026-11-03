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
    max_gap_cm (250m) e descartando pontos órfãos isolados (cadeias de 1 único ponto).
    """
    if not control_points:
        return []

    chains = [[control_points[0]]]

    for i in range(1, len(control_points)):
        curr_pt = control_points[i]
        placed = False

        # Verifica da cadeia mais recente para a mais antiga se curr_pt conecta no fim ou no início
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

    # Filtrar pontos órfãos/fantasmas isolados (len < 2) e avisar no log
    valid_chains = []
    for ch in chains:
        if len(ch) < 2:
            orphan = ch[0]
            print(
                f"[LandscapeSpline] Ignorando Control Point órfão/isolado: "
                f"{orphan['name']} em ({orphan['world_loc'].x:.1f}, {orphan['world_loc'].y:.1f}, {orphan['world_loc'].z:.1f})"
            )
        else:
            valid_chains.append(ch)

    return valid_chains


def sync_landscape_splines_to_pontaletes(
    pontalete_bp_path="/Game/Sirv/Static/props/Pontaletes/Pontalete_BP",
    road_margin_offset=-50.0,
    pontalete_spacing_offset=1500.0,
):
    """
    Lê todas as Landscape Splines do terreno ativo e cria/atualiza atores Pontalete_BP
    copiando a linha central (World Location) e o Half-Width de cada ponto para o Scale.Y.
    """
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()

    # 1. Localizar o LandscapeSplinesComponent no nível
    splines_comp = None
    landscape_actor = None

    for actor in all_actors:
        comps = actor.get_components_by_class(unreal.LandscapeSplinesComponent)
        if comps:
            splines_comp = comps[0]
            landscape_actor = actor
            break

    if not splines_comp:
        unreal.log_error("Nenhuma Landscape Spline foi encontrada no nível atual!")
        print("ERRO: Nenhuma Landscape Spline encontrada no nível atual!")
        return

    print(f"[LandscapeSpline] Encontrada no ator '{landscape_actor.get_actor_label()}'")

    # 2. Extrair todos os Control Points
    cps = get_all_landscape_control_points(splines_comp)
    if len(cps) < 2:
        unreal.log_warning("Menos de 2 Control Points encontrados na Landscape Spline.")
        print("AVISO: Menos de 2 Control Points encontrados.")
        return

    chains = split_and_order_chains(cps)
    print(f"[LandscapeSpline] {len(cps)} pontos lidos -> divididos em {len(chains)} rota(s) contínua(s).")

    # 3. Localizar a classe do Pontalete_BP (se já existir instância no mapa ou pelo Asset Path)
    bp_class = None
    template_actor = None
    existing_pontaletes = []
    for actor in all_actors:
        if "Pontalete" in actor.get_class().get_name() or "Pontalete" in actor.get_actor_label():
            if bp_class is None:
                bp_class = actor.get_class()
            if actor.get_actor_label().startswith("Pontalete_AutoSpline_"):
                existing_pontaletes.append(actor)
            elif template_actor is None:
                template_actor = actor

    if bp_class is None:
        bp_asset = unreal.EditorAssetLibrary.load_asset(pontalete_bp_path)
        if bp_asset:
            bp_class = bp_asset.generated_class()

    if bp_class is None:
        unreal.log_error(
            f"Não foi possível localizar a classe Pontalete_BP em '{pontalete_bp_path}'. "
            "Coloque pelo menos 1 Pontalete_BP no mapa para referência ou ajuste o caminho!"
        )
        print("ERRO: Coloque pelo menos 1 Pontalete_BP no nível ou verifique o caminho do asset.")
        return

    # Ler propriedades customizadas do template_actor, se houver no nível
    template_props = {}
    if template_actor is not None:
        for p_name in ["PlacedMesh", "placed_mesh", "Color1", "color1", "Color2", "color2", "Color3", "color3"]:
            try:
                val = template_actor.get_editor_property(p_name)
                if val is not None:
                    template_props[p_name] = val
            except Exception:
                pass

    # Remover atores automáticos antigos antes de recriar
    for old_actor in existing_pontaletes:
        actor_subsystem.destroy_actor(old_actor)

    # 4. Criar e popular um Pontalete_BP para cada cadeia contínua da Landscape Spline
    for chain_idx, chain in enumerate(chains):
        if len(chain) < 2:
            continue

        start_loc = chain[0]["world_loc"]
        new_actor = actor_subsystem.spawn_actor_from_class(bp_class, start_loc, unreal.Rotator(0, 0, 0))
        new_actor.set_actor_label(f"Pontalete_AutoSpline_{chain_idx + 1:02d}")

        # Se for a Rota 02 (Cianita -> Rejeito), adicionar automaticamente a Tag de Zona de Deslizamento
        if chain_idx == 1:
            try:
                new_actor.tags = [unreal.Name("SlipZone"), unreal.Name("SlipZone_Road")]
            except Exception:
                pass

        # PASSO A: Configurar TODAS as variáveis do Ator PRIMEIRO
        # (Pois cada set_editor_property no Ator destrói e recria os componentes internos!)
        for p_name, p_val in template_props.items():
            try:
                new_actor.set_editor_property(p_name, p_val)
            except Exception:
                pass

        for prop_name in ["RoadWidthOffset", "road_width_offset"]:
            try:
                new_actor.set_editor_property(prop_name, float(road_margin_offset))
            except Exception:
                pass

        for prop_name in ["Offset", "offset"]:
            try:
                new_actor.set_editor_property(prop_name, float(pontalete_spacing_offset))
            except Exception:
                pass

        # PASSO B: Agora pegamos a instância VIVA e atual do SplineComponent
        spline_comp = new_actor.get_component_by_class(unreal.SplineComponent)
        if not spline_comp:
            continue

        spline_comp.clear_spline_points(False)

        for pt_idx, pt_data in enumerate(chain):
            w_loc = pt_data["world_loc"]
            hw = pt_data["half_width"]

            spline_comp.add_spline_point(w_loc, unreal.SplineCoordinateSpace.WORLD, False)
            spline_comp.set_spline_point_type(pt_idx, unreal.SplinePointType.CURVE, False)
            # Gravar o Half-Width exato do Control Point no eixo Y da Escala da Spline
            spline_comp.set_scale_at_spline_point(pt_idx, unreal.Vector(1.0, hw, 1.0), False)

        spline_comp.update_spline()

        # PASSO C: Ativar as duas flags em uma única transação atômica (set_editor_properties)
        # Isso preserva os pontos recém-adicionados e dispara o ConstructionScript com eles!
        try:
            spline_comp.set_editor_properties({
                "spline_has_been_edited": True,
                "input_spline_points_to_construction_script": True,
            })
        except Exception as e:
            unreal.log_warning(f"Aviso ao configurar flags da Spline: {e}")

        # Verificar quantos pontos e quantas instâncias foram geradas
        live_spline = new_actor.get_component_by_class(unreal.SplineComponent)
        live_ism = new_actor.get_component_by_class(unreal.InstancedStaticMeshComponent)
        n_pts = live_spline.get_number_of_spline_points() if live_spline else 0
        n_inst = live_ism.get_instance_count() if live_ism else 0

        print(
            f" -> Criado '{new_actor.get_actor_label()}': {n_pts} pontos na Spline | "
            f"{n_inst} pontaletes instanciados! "
            f"(Half-Width de {min(p['half_width'] for p in chain):.0f} a {max(p['half_width'] for p in chain):.0f} cm)"
        )

    unreal.log("SUCESSO: Pontaletes sincronizados com as Landscape Splines!")


if __name__ == "__main__":
    sync_landscape_splines_to_pontaletes()
