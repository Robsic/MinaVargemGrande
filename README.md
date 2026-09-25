# MinaVargemGrande — Simulação Digital de Mineração

Projeto de simulação 3D interativa da Mina Vargem Grande, desenvolvido em **Unreal Engine 5** com integração **ROS2** via plugin SIRV.

## 📋 Sobre o Projeto

Simulação de campo de lavra com veículos pesados de mineração, rotas de transporte, zonas de velocidade e marcadores de ponto de interesse (POI). Desenvolvido como ferramenta de análise operacional e treinamento.

## 🚜 Veículos Implementados

| Veículo | Tipo | Descrição |
|---|---|---|
| **CAT 789D** | Caminhão fora-de-estrada | Modelo custom riggado com blueprint de simulação |
| **PC 8000** | Escavadeira hidráulica | Modelo custom com sistema de braços e carregamento bilateral |
| CAT 793F | Caminhão fora-de-estrada | Veículo do plugin SIRV |
| CAT D10T | Trator de esteiras | Veículo do plugin SIRV |

## 🗺️ Mapas

- **MVG_SAP** — Setor SAP da mina (estado atual)
- **MVG_GAL** — Setor GAL da mina (estado atual)
- **MVG_CIA** — Setor CIA da mina (estado atual)
- **MVG_SIMULATION** — Mapa de simulação integrada
- **SAP_Futuro / GAL_Futuro** — Cenários de planejamento futuro

## 🛠️ Tecnologias

- **Unreal Engine 5** — Motor de simulação 3D
- **ROS2 / SIRV** — Integração com sistemas robóticos e ROS
- **C++** — Plugin SIRV customizado
- **Python** — Scripts de importação de dados e exportação de zonas

## 📁 Estrutura do Repositório

```
MinaVargemGrande/
├── Config/                         # Configurações do projeto UE5
├── Content/
│   ├── Blueprints/                 # BPs customizados (POI, SpeedZone, Rotas)
│   ├── Landscapes/                 # Terrenos e materiais da mina
│   │   ├── Levels/                 # Mapas principais
│   │   ├── Levels_Futuro/          # Mapas de planejamento
│   │   └── Material/               # Materiais de terreno
│   ├── Python/                     # Scripts Python para importação/exportação
│   └── Sirv/                       # Assets do plugin SIRV + veículos customizados
│       ├── Blueprints/Vehicles/CAT789D/   # ← Desenvolvimento próprio
│       └── Blueprints/Vehicles/PC8000/    # ← Desenvolvimento próprio
├── Plugins/Sirv/Source/            # Código-fonte C++ do plugin SIRV
└── Script/                         # Scripts auxiliares
```

## 🔧 Como usar

1. Instale o **Unreal Engine 5.3+**
2. Clone este repositório com Git LFS habilitado:
   ```bash
   git lfs install
   git clone https://github.com/IagoVilela05/MinaVargemGrande.git
   ```
3. Abra `MinaVargemGrande.uproject` no Unreal Engine
4. O plugin SIRV requer dependências ROS2 — consulte `Plugins/Sirv/SirvDependencies/`

> **Nota:** Este repositório contém os assets de desenvolvimento. Assets pesados do plugin SIRV (veículos genéricos, materiais genéricos) estão disponíveis no repositório principal no Azure Repos.

## 📄 Documentação

- [`DOCUMENTACAO_CARREGAMENTO_BILATERAL_ESCAVADEIRA.md`](DOCUMENTACAO_CARREGAMENTO_BILATERAL_ESCAVADEIRA.md) — Documentação do sistema de carregamento bilateral da PC8000
- [`VALIDACAO_E_PROXIMOS_PASSOS_CAT789D.md`](VALIDACAO_E_PROXIMOS_PASSOS_CAT789D.md) — Validação e próximos passos do CAT789D

---

*Desenvolvido durante estágio em simulação de mineração.*
