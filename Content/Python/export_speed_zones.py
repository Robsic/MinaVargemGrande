import unreal
import os
import math
import json

def get_actor_property_robust(actor, possible_names, default_value=None):
    """
    Busca uma propriedade no ator Blueprint testando múltiplos nomes e métodos de acesso
    da API Python da Unreal Engine.
    """
    for name in possible_names:
        try:
            val = actor.get_editor_property(name)
            if val is not None and str(val).strip() != "":
                return val
        except Exception:
            pass

    for name in possible_names:
        if hasattr(actor, name):
            try:
                val = getattr(actor, name)
                if val is not None and str(val).strip() != "":
                    return val
            except Exception:
                pass

    return default_value

def simplify_spline_indices(spline_comp, max_angle_deg=24.0, max_lateral_dev=700.0, max_segment_length=60000.0):
    """
    Agrupa pontos da Spline de forma inteligente usando a geometria dos segmentos (cordas):
    - max_angle_deg: desvio angular tolerado em graus (24° absorve ondulacoes de retas sem invadir taludes)
    - max_lateral_dev: desvio lateral maximo permitido (700 UU = 7.0 metros em uma pista de 30m)
    - max_segment_length: comprimento maximo de um unico retangulo (60000 UU = 600 metros)
    """
    num_points = spline_comp.get_number_of_spline_points()
    if num_points < 2:
        return list(range(num_points))

    key_indices = [0]
    start_idx = 0

    while start_idx < num_points - 1:
        end_idx = start_idx + 1
        loc_start = spline_comp.get_location_at_spline_point(start_idx, unreal.SplineCoordinateSpace.WORLD)
        
        # Vetor base inicial do primeiro segmento da reta (start_idx -> start_idx + 1)
        loc_first = spline_comp.get_location_at_spline_point(start_idx + 1, unreal.SplineCoordinateSpace.WORLD)
        base_dx = loc_first.x - loc_start.x
        base_dy = loc_first.y - loc_start.y
        base_mag = math.hypot(base_dx, base_dy)

        # Tentar estender a reta o maximo possivel
        for candidate in range(start_idx + 2, num_points):
            loc_cand = spline_comp.get_location_at_spline_point(candidate, unreal.SplineCoordinateSpace.WORLD)
            
            # Vetor da corda total (start_idx -> candidate)
            chord_dx = loc_cand.x - loc_start.x
            chord_dy = loc_cand.y - loc_start.y
            chord_dist = math.hypot(chord_dx, chord_dy)

            # 1. Limite de comprimento maximo
            if chord_dist > max_segment_length:
                break

            # 2. Desvio angular da corda em relacao a direcao inicial
            is_valid = True
            if base_mag > 1e-4 and chord_dist > 1e-4:
                dot = (base_dx * chord_dx + base_dy * chord_dy) / (base_mag * chord_dist)
                dot = max(-1.0, min(1.0, dot))
                angle_deg = math.degrees(math.acos(dot))
                if angle_deg > max_angle_deg:
                    is_valid = False

            # 3. Desvio lateral maximo de cada ponto intermediario ate a corda
            if is_valid and chord_dist > 1e-4:
                for mid in range(start_idx + 1, candidate):
                    loc_mid = spline_comp.get_location_at_spline_point(mid, unreal.SplineCoordinateSpace.WORLD)
                    num = abs(chord_dy * loc_mid.x - chord_dx * loc_mid.y + loc_cand.x * loc_start.y - loc_cand.y * loc_start.x)
                    lat_dist = num / chord_dist
                    if lat_dist > max_lateral_dev:
                        is_valid = False
                        break

            if is_valid:
                end_idx = candidate
            else:
                break

        key_indices.append(end_idx)
        start_idx = end_idx

    return key_indices

def export_all_speed_zones_to_yaml(
    output_filepath="/home/iago/Desktop/speed_limits_mina.yaml",
    road_half_width=1500.0,       # 15.0m para cada lado (Pista de 30m padrao Vale / CAT 789D)
    road_default_speed=50.0,      # 50 km/h velocidade padrao da via
    road_default_load_mode="loaded,unloaded",
    max_angle_deg=24.0,           # Limiar angular em graus (24 deg permite cobrir retas com leves curvas)
    max_lateral_dev=700.0,        # Desvio lateral maximo tolerado (700 UU = 7.0 metros)
    max_segment_length=60000.0    # Comprimento maximo de um unico segmento em reta (600 metros)
):
    """
    Exporta todas as zonas de velocidade para YAML e salva JSON estruturado para o visualizador do mapa:
    1. Atores BP_SpeedZone (pontos de interesse)
    2. Atores Pontalete_BP (trechos da via agrupados por retas e curvas)
    """
    actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    all_actors = actor_subsystem.get_all_level_actors()
    
    speed_zone_actors = []
    pontalete_actors = []
    landscape_info = {}

    for actor in all_actors:
        actor_class_name = actor.get_class().get_name()
        actor_label = actor.get_actor_label()
        
        if "Landscape" in actor_class_name and not landscape_info:
            loc = actor.get_actor_location()
            scale = actor.get_actor_scale3d()
            landscape_info = {
                "name": actor_label,
                "location": [loc.x, loc.y, loc.z],
                "scale": [scale.x, scale.y, scale.z]
            }
            print(f"[Landscape] Detectado: {actor_label} em Location={loc}, Scale={scale}")

        if "BP_SpeedZone" in actor_class_name or "SpeedZone" in actor_class_name or actor_label.startswith("SpeedZone") or actor_label.startswith("BP_SpeedZone"):
            speed_zone_actors.append(actor)
        elif "Pontalete" in actor_class_name or "Pontalete" in actor_label:
            pontalete_actors.append(actor)
            
    if not speed_zone_actors and not pontalete_actors:
        unreal.log_warning("Nenhum ator de SpeedZone ou Pontalete foi encontrado no nível aberto!")
        print("AVISO: Nenhum ator de SpeedZone ou Pontalete foi encontrado no nível aberto!")
        return

    yaml_lines = []
    json_data = {
        "landscape": landscape_info,
        "zones": []
    }
    total_exported_zones = 0
    next_code_id = 1

    # ==========================================================
    # 1. PROCESSAR ATORES BP_SpeedZone (Pontos de Interesse)
    # ==========================================================
    if speed_zone_actors:
        yaml_lines.append("# ==========================================================")
        yaml_lines.append("# 1. PONTOS DE INTERESSE E ÁREAS ESPECÍFICAS (BP_SpeedZone)")
        yaml_lines.append("# ==========================================================")
        yaml_lines.append("")
        
        print("\n--- PROCESSANDO ATORES BP_SpeedZone ---")
        for actor in speed_zone_actors:
            label = actor.get_actor_label()
            
            zone_code = get_actor_property_robust(actor, ["ZoneCode", "zone_code", "Zone_Code", "ZoneName", "zone_name", "Tag", "tag", "AreaName", "Area_Name"], label)
            code_id = get_actor_property_robust(actor, ["CodeID", "code_id", "Code_ID", "CodeId", "Code", "code", "ID", "id"], next_code_id)
            speed_limit = get_actor_property_robust(actor, ["SpeedLimit", "speed_limit", "Speed_Limit", "Speedlimit", "Speed", "speed", "Velocidade", "velocidade", "VelocidadeLimite"], 10.0)
            load_mode = get_actor_property_robust(actor, ["LoadMode", "load_mode", "Load_Mode", "Loadmode", "Mode", "mode"], "loaded,unloaded")
            
            try:
                code_id = int(code_id)
            except:
                code_id = next_code_id
                
            try:
                speed_limit = float(speed_limit)
            except:
                speed_limit = 10.0
                
            zone_code = str(zone_code).replace(" ", "_")
            if zone_code == "None" or not zone_code:
                zone_code = label.replace(" ", "_")
                
            load_mode = str(load_mode)
            
            print(f"[BP_SpeedZone] Ator: '{label}' -> ZoneCode='{zone_code}', CodeID={code_id}, SpeedLimit={speed_limit} km/h, LoadMode='{load_mode}'")
            
            spline_comp = None
            if hasattr(actor, "get_component_by_class"):
                spline_comp = actor.get_component_by_class(unreal.SplineComponent)
            if not spline_comp and hasattr(actor, "get_components_by_class"):
                comps = actor.get_components_by_class(unreal.SplineComponent)
                if comps:
                    spline_comp = comps[0]

            if not spline_comp:
                unreal.log_warning(f"SpeedZone {label} não possui SplineComponent!")
                continue
                
            num_points = spline_comp.get_number_of_spline_points()
            
            yaml_lines.append(f"SpeedLimit_{zone_code}.code: {code_id}")
            yaml_lines.append(f"SpeedLimit_{zone_code}.speedLimit: {speed_limit:.1f}")
            yaml_lines.append(f"SpeedLimit_{zone_code}.loadMode: {load_mode}")
            yaml_lines.append(f"SpeedLimit_{zone_code}.numberOfPoints: {num_points}")
            
            zone_points = []
            for i in range(num_points):
                world_loc = spline_comp.get_location_at_spline_point(i, unreal.SplineCoordinateSpace.WORLD)
                yaml_lines.append(f"SpeedLimit_{zone_code}.point_{i+1}: [{world_loc.x:.8f}, {world_loc.y:.8f}, {world_loc.z:.8f}]")
                zone_points.append([world_loc.x, world_loc.y, world_loc.z])
                
            yaml_lines.append("")
            
            json_data["zones"].append({
                "name": zone_code,
                "type": "area",
                "code": code_id,
                "speed_limit": speed_limit,
                "points": zone_points
            })
            
            total_exported_zones += 1
            next_code_id = max(next_code_id + 1, code_id + 1)

    # ==========================================================
    # 2. PROCESSAR ATORES Pontalete_BP (Trechos Otimizados)
    # ==========================================================
    if pontalete_actors:
        yaml_lines.append("# ==========================================================")
        yaml_lines.append("# 2. TRECHOS DA PISTA PRINCIPAL (Otimizados por Curvatura)")
        yaml_lines.append(f"# Largura util: {road_half_width*2/100:.1f}m (Meia-largura: {road_half_width/100:.1f}m)")
        yaml_lines.append(f"# Limiar angular: {max_angle_deg} graus | Tolerancia lateral: {max_lateral_dev/100:.1f}m")
        yaml_lines.append("# ==========================================================")
        yaml_lines.append("")
        
        print("\n--- PROCESSANDO SPLINES DE PISTAS (Pontalete_BP) ---")
        total_raw_segments = 0
        total_optimized_segments = 0
        
        for actor in pontalete_actors:
            actor_label = actor.get_actor_label()
            route_name = actor_label.replace("Pontalete_", "").replace("Pontaletes_", "").replace(" ", "_")
            
            half_w = float(road_half_width)
            custom_offset = get_actor_property_robust(actor, ["RoadWidthOffset", "road_width_offset", "Offset", "offset"], None)
            if custom_offset is not None:
                try:
                    val = float(custom_offset)
                    if val > 0 and val < 5000:
                        half_w = val
                except:
                    pass
                
            spline_comp = None
            if hasattr(actor, "get_component_by_class"):
                spline_comp = actor.get_component_by_class(unreal.SplineComponent)
            if not spline_comp and hasattr(actor, "get_components_by_class"):
                comps = actor.get_components_by_class(unreal.SplineComponent)
                if comps:
                    spline_comp = comps[0]

            if not spline_comp:
                unreal.log_warning(f"Pontalete {actor_label} não possui SplineComponent!")
                continue

            num_spline_points = spline_comp.get_number_of_spline_points()
            if num_spline_points < 2:
                continue

            total_raw_segments += (num_spline_points - 1)

            # Algoritmo de agrupamento robusto por cordas
            key_indices = simplify_spline_indices(
                spline_comp,
                max_angle_deg=max_angle_deg,
                max_lateral_dev=max_lateral_dev,
                max_segment_length=max_segment_length
            )
            
            num_segments = len(key_indices) - 1
            total_optimized_segments += num_segments
            print(f"[Pontalete_BP] Rota '{route_name}': {num_spline_points} pontos brutos -> {num_segments} trechos!")

            # Guardar pontos completos da spline central para desenhar no mapa
            spline_centerline = []
            for p_idx in range(num_spline_points):
                p_loc = spline_comp.get_location_at_spline_point(p_idx, unreal.SplineCoordinateSpace.WORLD)
                spline_centerline.append([p_loc.x, p_loc.y, p_loc.z])

            for seg_idx in range(num_segments):
                idx_a = key_indices[seg_idx]
                idx_b = key_indices[seg_idx + 1]

                loc_a = spline_comp.get_location_at_spline_point(idx_a, unreal.SplineCoordinateSpace.WORLD)
                right_a = spline_comp.get_right_vector_at_spline_point(idx_a, unreal.SplineCoordinateSpace.WORLD)
                
                loc_b = spline_comp.get_location_at_spline_point(idx_b, unreal.SplineCoordinateSpace.WORLD)
                right_b = spline_comp.get_right_vector_at_spline_point(idx_b, unreal.SplineCoordinateSpace.WORLD)
                
                # 4 Vertices do trecho da pista (Ordem horaria):
                v1_x = loc_a.x - (right_a.x * half_w)
                v1_y = loc_a.y - (right_a.y * half_w)
                v1_z = loc_a.z
                
                v2_x = loc_a.x + (right_a.x * half_w)
                v2_y = loc_a.y + (right_a.y * half_w)
                v2_z = loc_a.z
                
                v3_x = loc_b.x + (right_b.x * half_w)
                v3_y = loc_b.y + (right_b.y * half_w)
                v3_z = loc_b.z
                
                v4_x = loc_b.x - (right_b.x * half_w)
                v4_y = loc_b.y - (right_b.y * half_w)
                v4_z = loc_b.z

                segment_code = f"{route_name}_T{seg_idx+1:02d}"
                
                yaml_lines.append(f"SpeedLimit_{segment_code}.code: {next_code_id}")
                yaml_lines.append(f"SpeedLimit_{segment_code}.speedLimit: {road_default_speed:.1f}")
                yaml_lines.append(f"SpeedLimit_{segment_code}.loadMode: {road_default_load_mode}")
                yaml_lines.append(f"SpeedLimit_{segment_code}.numberOfPoints: 4")
                yaml_lines.append(f"SpeedLimit_{segment_code}.point_1: [{v1_x:.8f}, {v1_y:.8f}, {v1_z:.8f}]")
                yaml_lines.append(f"SpeedLimit_{segment_code}.point_2: [{v2_x:.8f}, {v2_y:.8f}, {v2_z:.8f}]")
                yaml_lines.append(f"SpeedLimit_{segment_code}.point_3: [{v3_x:.8f}, {v3_y:.8f}, {v3_z:.8f}]")
                yaml_lines.append(f"SpeedLimit_{segment_code}.point_4: [{v4_x:.8f}, {v4_y:.8f}, {v4_z:.8f}]")
                yaml_lines.append("")
                
                json_data["zones"].append({
                    "name": segment_code,
                    "type": "road",
                    "code": next_code_id,
                    "speed_limit": road_default_speed,
                    "points": [
                        [v1_x, v1_y, v1_z],
                        [v2_x, v2_y, v2_z],
                        [v3_x, v3_y, v3_z],
                        [v4_x, v4_y, v4_z]
                    ]
                })

                total_exported_zones += 1
                next_code_id += 1

            if "routes" not in json_data:
                json_data["routes"] = []
            json_data["routes"].append({
                "name": route_name,
                "centerline": spline_centerline
            })

    # Gravar arquivos de saida
    output_dir = os.path.dirname(output_filepath)
    if output_dir and not os.path.exists(output_dir):
        os.makedirs(output_dir)

    with open(output_filepath, "w", encoding="utf-8") as f:
        f.write("\n".join(yaml_lines))
        
    json_filepath = os.path.splitext(output_filepath)[0] + ".json"
    with open(json_filepath, "w", encoding="utf-8") as f:
        json.dump(json_data, f, indent=2)

    print(f"\n============================================================")
    print(f" RESUMO DA OTIMIZACAO:")
    print(f" Trechos brutos anteriores : {total_raw_segments}")
    print(f" Trechos otimizados finais : {total_optimized_segments}")
    print(f" Reducao total de poligonos: {((1.0 - total_optimized_segments/max(1, total_raw_segments))*100):.1f}%")
    print(f" Total geral de zonas (com areas): {total_exported_zones}")
    print(f" Arquivo YAML gerado em : {output_filepath}")
    print(f" Arquivo JSON gerado em : {json_filepath}")
    print(f"============================================================")
    unreal.log(f"SUCESSO: {total_exported_zones} zonas exportadas para '{output_filepath}'")

if __name__ == "__main__":
    export_all_speed_zones_to_yaml("/home/iago/Desktop/speed_limits_mina.yaml")
