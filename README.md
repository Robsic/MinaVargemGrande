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
