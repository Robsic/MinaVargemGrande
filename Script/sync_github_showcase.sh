#!/usr/bin/env bash
# ==============================================================================
# sync_github_showcase.sh
# Sincroniza incrementalmente o branch 'development' (Azure) com o branch
# 'github-showcase' (GitHub main), filtrando ~5.5 GB de assets não utilizados
# para respeitar o limite de 10 GB do GitHub LFS e manter o histórico de commits
# (gráfico de contribuições do GitHub) intacto sem precisar de --force.
# ==============================================================================

set -e

PROJECT_DIR="/home/iago/Documentos/Unreal Projects/MinaVargemGrande"
cd "$PROJECT_DIR"

COMMIT_MSG="${1:-$(git log -1 --pretty=%B development)}"

echo "========================================================"
echo "  🚀 Sincronizando Showcase para o GitHub (Incremental)"
echo "========================================================"

# 1. Garantir que estamos no branch development e com árvore limpa
CURRENT_BRANCH=$(git branch --show-current)
if [ "$CURRENT_BRANCH" != "development" ]; then
    echo "⚠️  Mudando de '$CURRENT_BRANCH' para 'development'..."
    git checkout -f development
fi

if [ -n "$(git status --porcelain)" ]; then
    echo "❌ Erro: Existem alterações não commitadas no branch 'development'."
    echo "   Faça commit no 'development' antes de sincronizar o showcase."
    exit 1
fi

# 2. Buscar estado atual do GitHub e preparar o branch github-showcase
echo "🔄 Atualizando referência do remote 'github'..."
git fetch github main

if git show-ref --verify --quiet refs/heads/github-showcase; then
    git checkout -f github-showcase
    git reset --hard github/main
else
    git checkout -b github-showcase github/main
fi

# 3. Espelhar a árvore exata do 'development' (incluindo deleções) mantendo o HEAD no github-showcase
echo "📂 Importando árvore atualizada do branch 'development'..."
git read-tree --reset -u development

# 4. Remover do índice (git rm --cached) os assets pesados não utilizados (~5.5 GB)
echo "🧹 Filtrando pacotes de assets não referenciados (~5.5 GB)..."
UNUSED_DIRS=(
    "Content/Sirv/Static/GenericMaterials/Tiles"
    "Content/Sirv/Static/GenericMaterials/Ground"
    "Content/Sirv/Static/GenericMaterials/Masters"
    "Content/Sirv/Static/GenericMaterials/00_MastersOpt"
    "Content/Sirv/Static/GenericMaterials/Concrete"
    "Content/Sirv/Static/GenericMaterials/Grass"
    "Content/Sirv/Static/GenericMaterials/RoofBitumen"
    "Content/Sirv/Static/GenericMaterials/Brick"
    "Content/Sirv/Static/GenericMaterials/Gutters_Curbs"
    "Content/Sirv/Static/GenericMaterials/RoadStencil"
    "Content/Sirv/Static/GenericMaterials/LaneMarking"
    "Content/Sirv/Static/GenericMaterials/Water"
    "Content/Sirv/Static/GenericMaterials/WetPavement"
    "Content/Sirv/Static/GenericMaterials/CheapMaterials"
    "Content/Sirv/Static/GenericMaterials/Generic_Rock_Material"
    "Content/Sirv/Static/GenericMaterials/Licenseplates/Textures"
    "Content/Sirv/Static/Rain_material"
    "Content/Sirv/Static/ConstructionSite/Demo"
    "Content/Sirv/Static/ConstructionSite/Environment/Bulldozer_01"
    "Content/Sirv/Static/ConstructionSite/Environment/MetalContainer_01"
    "Content/Sirv/Static/ConstructionSite/Environment/RoadBarrier_01"
    "Content/Sirv/Static/ConstructionSite/Environment/TileGround_01"
    "Content/Sirv/Static/ConstructionSite/Environment/Sky_01"
    "Content/Sirv/Static/ConstructionSite/Environment/Ruble_03"
    "Content/Sirv/Static/ConstructionSite/Environment/Cobble_01"
    "Content/Sirv/Static/ConstructionSite/Environment/Ground_01"
    "Content/Sirv/Static/ConstructionSite/Environment/Barrel_01"
    "Content/Sirv/Static/ConstructionSite/Environment/BlockBrick_01"
    "Content/Sirv/Static/ConstructionSite/Environment/SafetyHelmet_01"
    "Content/Sirv/Static/props/Meshes/Textures"
    "Content/Sirv/Blueprints/Vehicles/CAT7295"
    "Content/Sirv/Static/GenericMaterials/Asphalt/M_GTARoad"
    "Content/Sirv/Static/GenericMaterials/Asphalt/M_demoRoad"
)

git rm --cached -r --ignore-unmatch "${UNUSED_DIRS[@]}" > /dev/null

# Filtrar arquivos individuais não usados na pasta Asphalt (manter apenas 1, 2, 3 e T_Asphalt02_d)
git ls-files "Content/Sirv/Static/GenericMaterials/Asphalt/" | while read -r f; do
    base=$(basename "$f")
    case "$base" in
        "1.uasset"|"2.uasset"|"3.uasset"|"T_Asphalt02_d.uasset")
            ;;
        *)
            git rm --cached --ignore-unmatch "$f" > /dev/null
            ;;
    esac
done

# 5. Atualizar .gitignore do showcase
cat << 'EOF' >> .gitignore

# =====================================================
# GitHub Showcase — assets grandes não utilizados
# =====================================================
Content/Sirv/Static/GenericMaterials/Tiles/
Content/Sirv/Static/GenericMaterials/Ground/
Content/Sirv/Static/GenericMaterials/Masters/
Content/Sirv/Static/GenericMaterials/00_MastersOpt/
Content/Sirv/Static/GenericMaterials/Concrete/
Content/Sirv/Static/GenericMaterials/Grass/
Content/Sirv/Static/GenericMaterials/RoofBitumen/
Content/Sirv/Static/GenericMaterials/Brick/
Content/Sirv/Static/GenericMaterials/Gutters_Curbs/
Content/Sirv/Static/GenericMaterials/RoadStencil/
Content/Sirv/Static/GenericMaterials/LaneMarking/
Content/Sirv/Static/GenericMaterials/Water/
Content/Sirv/Static/GenericMaterials/WetPavement/
Content/Sirv/Static/GenericMaterials/CheapMaterials/
Content/Sirv/Static/GenericMaterials/Generic_Rock_Material/
Content/Sirv/Static/GenericMaterials/Licenseplates/Textures/
Content/Sirv/Static/Rain_material/
Content/Sirv/Static/ConstructionSite/Demo/
Content/Sirv/Static/ConstructionSite/Environment/Bulldozer_01/
Content/Sirv/Static/ConstructionSite/Environment/MetalContainer_01/
Content/Sirv/Static/ConstructionSite/Environment/RoadBarrier_01/
Content/Sirv/Static/ConstructionSite/Environment/TileGround_01/
Content/Sirv/Static/ConstructionSite/Environment/Sky_01/
Content/Sirv/Static/ConstructionSite/Environment/Ruble_03/
Content/Sirv/Static/ConstructionSite/Environment/Cobble_01/
Content/Sirv/Static/ConstructionSite/Environment/Ground_01/
Content/Sirv/Static/ConstructionSite/Environment/Barrel_01/
Content/Sirv/Static/ConstructionSite/Environment/BlockBrick_01/
Content/Sirv/Static/ConstructionSite/Environment/SafetyHelmet_01/
Content/Sirv/Static/props/Meshes/Textures/
Content/Sirv/Blueprints/Vehicles/CAT7295/
Content/Sirv/Static/GenericMaterials/Asphalt/M_GTARoad/
Content/Sirv/Static/GenericMaterials/Asphalt/M_demoRoad/
Content/Sirv/Static/GenericMaterials/Asphalt/Asphalt*
Content/Sirv/Static/GenericMaterials/Asphalt/M_Asphalt*
Content/Sirv/Static/GenericMaterials/Asphalt/MI_Asphalt*
Content/Sirv/Static/GenericMaterials/Asphalt/T_Asphalt01*
Content/Sirv/Static/GenericMaterials/Asphalt/T_Asphalt02Cracks*
Content/Sirv/Static/GenericMaterials/Asphalt/T_Asphalt02_n*
Content/Sirv/Static/GenericMaterials/Asphalt/T_Asphalt02_oc*
Content/Sirv/Static/GenericMaterials/Asphalt/T_Asphalt02_r*
Content/Sirv/Static/GenericMaterials/Asphalt/T_NewAsphalt*
Content/Sirv/Static/GenericMaterials/Asphalt/T_Asphalt.uasset
Content/Sirv/Static/GenericMaterials/Asphalt/asphalt_02*
Content/Sirv/Static/GenericMaterials/Asphalt/4.uasset
EOF

# 6. Gerar README.md atualizado no showcase
cat << 'EOF' > README.md
# MinaVargemGrande — Simulador VR de Mineração (UNIFEI & Vale)

Projeto de simulação 3D interativa da **Mina do Pico (Complexo Vargem Grande)**, desenvolvido na **Unreal Engine 5** com integração **ROS 2** via plugin SIRV.

## 📋 Sobre o Projeto

Simulação em escala real (1:1) de operações de lavra com caminhões fora-de-estrada e escavadeiras hidráulicas, reconstrução topográfica 2026 via nuvens de pontos LiDAR (`.las`) e ortofotos georreferenciadas (`.tif`), demarcação de zonas de velocidade (`.yaml`) e treinamento operacional em Realidade Virtual.

## 🚜 Veículos Implementados

| Veículo | Tipo | Destaques Técnicos |
|---|---|---|
| **CAT 789D** | Caminhão fora-de-estrada (181 t) | Malha e rig customizados (20 ossos), iluminação funcional em 18 slots de materiais e Display Externo de Carga (TPMS/VIMS 3D UMG) |
| **PC 8000** | Escavadeira hidráulica | Cinemática de braços/caçamba e procedimento de carregamento bilateral |
| **CAT 793F** | Caminhão fora-de-estrada | Veículo base do plugin SIRV |
| **CAT D10T** | Trator de esteiras | Equipamento de apoio operacional |

## 🗺️ Mapas Principais

- **`MVG_SIMULATION_2026`** — Novo cenário 2026 da Mina do Pico ($5611 \times 9691\text{ px}$, $18,30\text{ km}^2$, SIRGAS 2000 / UTM 23S)
- **`MVG_SIMULATION`** — Cenário integrado de simulação operacional
- **`Teste789D`** — Mapa leve de bancada para validação dinâmica e visual do CAT 789D
- **`SAP_Futuro / GAL_Futuro`** — Cenários de planejamento futuro de lavra

## 🛠️ Tecnologias

- **Unreal Engine 5** (Chaos Vehicles Physics, Landscape, Niagara, UMG 3D)
- **ROS 2 / SIRV** — Comunicação e telemetria robótica em C++
- **Blender 3D** — Modelagem, UV Mapping e Rigging veicular
- **Python** — Processamento geoespacial (LiDAR/GeoTIFF) e automação no Editor UE5

## 📄 Documentação Técnica

- [`GEMINI.md`](GEMINI.md) — Arquitetura geral do projeto e especificações técnicas
- [`VALIDACAO_E_PROXIMOS_PASSOS_CAT789D.md`](VALIDACAO_E_PROXIMOS_PASSOS_CAT789D.md) — Especificação do Rig, física Chaos e iluminação do CAT 789D
- [`DOCUMENTACAO_CARREGAMENTO_BILATERAL_ESCAVADEIRA.md`](DOCUMENTACAO_CARREGAMENTO_BILATERAL_ESCAVADEIRA.md) — Procedimento operacional da escavadeira PC8000
EOF

git add .gitignore README.md

# 7. Criar commit incremental e enviar para o GitHub
if git diff --cached --quiet; then
    echo "ℹ️  Nenhuma alteração nova para enviar ao GitHub Showcase."
else
    echo "📦 Criando commit incremental no showcase: '$COMMIT_MSG'"
    git commit -m "$COMMIT_MSG"
    echo "☁️  Enviando para o GitHub (apenas deltas novos)..."
    git push github github-showcase:main
    echo "✅ Showcase atualizado com sucesso no GitHub!"
fi

# 8. Voltar de forma limpa para o branch development
echo "🔙 Retornando para o branch 'development'..."
git checkout -f development
echo "========================================================"
echo "  ✅ Concluído! Branch atual: $(git branch --show-current)"
echo "========================================================"
