import unreal
import os

def import_plate_textures():
    project_dir = unreal.SystemLibrary.get_project_directory()
    source_dir = os.path.join(project_dir, 'Content', 'Sirv', 'Static', 'props', 'Placas', 'material')
    destination_path = '/Game/Sirv/Static/props/Placas/material'
    
    files_to_import = [
        'Mat_SIRV_8_ALB.png',
        'Mat_SIRV_08_ALB.png',
        'Mat_SIRV_50_ALB.png'
    ]
    
    tasks = []
    for fname in files_to_import:
        fpath = os.path.join(source_dir, fname)
        if os.path.exists(fpath):
            task = unreal.AssetImportTask()
            task.filename = fpath
            task.destination_path = destination_path
            task.destination_name = os.path.splitext(fname)[0]
            task.replace_existing = True
            task.automated = True
            task.save = True
            tasks.append(task)
            print(f'[ImportPlacas] Preparando importacao: {fname}')
        else:
            print(f'[ImportPlacas] AVISO: Arquivo nao encontrado: {fpath}')
            
    if tasks:
        unreal.AssetToolsHelpers.get_asset_tools().import_asset_tasks(tasks)
        print(f'[ImportPlacas] SUCESSO: {len(tasks)} texturas importadas para {destination_path}!')
        unreal.log(f'[ImportPlacas] {len(tasks)} texturas de placas importadas com sucesso!')
    else:
        print('[ImportPlacas] Nenhuma textura foi importada.')

if __name__ == '__main__':
    import_plate_textures()
