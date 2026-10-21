# Diretrizes do Workspace e Mapa de Conhecimento (Estágio UNIFEI / Vale)

Este arquivo serve como contexto global permanente para o agente de IA neste workspace, eliminando a necessidade de buscas exploratórias excessivas, prevenindo comandos lentos desnecessários e mapeando a arquitetura completa das frentes de trabalho.

---

## 🚫 1. Regras Críticas de Execução (O que NÃO fazer & Diretrizes Obrigatórias)

Para evitar lentidão extrema, loops de comandos e consumo excessivo de contexto:

1. **NUNCA tente inspecionar ou fazer parse de arquivos binários `.uasset` ou `.umap` via scripts Python ou shell (`open()`, `grep`, `strings`, `re.findall`):**
   * Os assets da Unreal Engine 5 são binários serializados pelo editor. Abrir com scripts Python ad-hoc não funciona e gera dezenas de comandos lentos e inúteis.
   * Consulte a documentação interna existente em Markdown ou pergunte/confirme com o usuário a assinatura exata de nós ou variáveis do Blueprint.
2. **NÃO faça varreduras cegas de vértices/polígonos no Blender:**
   * O modelo e rig já estão consolidados em malha única (`CAT789D_Mesh`) e esqueleto (`CAT789D_Armature`).
   * Caminho fixo do Blender local: `/home/iago/.local/programs/blender/blender`.
3. **NÃO faça buscas recursivas em pastas gigantes:**
   * Nunca execute `grep` ou `find` sem filtro em diretórios de cache ou assets pesados: `Content/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `build/`, `.dart_tool/`, `node_modules/`.
4. **DOCUMENTAÇÃO OBRIGATÓRIA DE PROGRESSO E ENTREGAS:**
   * **Qualquer progresso ou alteração significativa para o desenvolvimento do projeto DEVE ser documentada em:**
     `/home/iago/Documentos/Estagio/RelatorioVale/relatorio-projeto-mina-pico-2026`
   * A documentação deve seguir estritamente o modelo, estilo textual e estrutura presentes em:
     `/home/iago/Documentos/Estagio/RelatorioVale/relatorio-projeto-app-atualizaco-mina-conceicao`

---

## 🗺️ 2. Mapa Arquitetural do Workspace

O workspace `/home/iago/Documentos/Estagio` divide-se em 3 frentes de trabalho principais:

```
/home/iago/Documentos/Estagio/                     # [Repo Git: VRMiningExternalFiles]
├── .gitignore                                 # Filtra ProjetoMina/ (15GB), backups e temps
├── GEMINI.md                                  # Este arquivo de contexto
├── VR - Mining 789D/                          # Assets 3D, Rigging e Processamento Geoespacial
├── RelatorioVale/                             # Relatórios Técnicos em LaTeX (UNIFEI & Vale)
│   ├── relatorio-projeto-mina-pico-2026/      # [ATIVO] Relatório de desenvolvimento atual
│   └── relatorio-projeto-app-atualizaco-mina-conceicao/ # [REFERÊNCIA] Repo independente
└── SiteRobsic/                                # Portal do Laboratório RobSIC (Repos independentes)

Projetos Externos Vinculados:
└── /home/iago/Documentos/Unreal Projects/MinaVargemGrande/  # [Repo Git: MinaVargemGrande (Azure/GitHub)]
```

### 🔗 Repositórios Remotos Vinculados
* **Arquivos Externos (Estágio):** `git@github.com:IagoVilela05/VRMiningExternalFiles.git` (GitHub)
* **Simulador Unreal Engine 5:**
  * Azure DevOps (Principal): `https://d2023012249@dev.azure.com/d2023012249/MinaVargemGrande/_git/MinaVargemGrande` (branch `development`)
  * GitHub (Showcase): `git@github.com:Iago-Vilela05/MinaVargemGrande.git` (ou `IagoVilela05/MinaVargemGrande.git`)


---

## 🚛 Frente 1: Simulador VR de Mineração / SIRV (UNIFEI & Vale)

### 1.1 Modelagem 3D & Rigging do CAT 789D (`VR - Mining 789D/`)
* **Arquivo 3D Principal:** `VR - Mining 789D/CAT789D_Rigged.blend` (Contém 18 slots de materiais e recortes manuais de Knife Tool nas setas da passarela e faróis superiores/milha).
* **Exportação FBX:** `VR - Mining 789D/CAT789D_Rigged.fbx`
* **Script de Automação do Rig:** `VR - Mining 789D/build_cat789d_rig.py` (Script histórico base. NÃO rodar direto sobre o blend bruto sem preservar os recortes dos 18 slots).
* **Slots de Materiais de Iluminação (18 slots no total):**
  * Slot 11: `CAT789D_Farois_Principais` (15 faces)
  * Slot 12: `CAT789D_Setas_Esq` (5 faces: traseira + passarela frontal + passarela lateral)
  * Slot 13: `CAT789D_LuzRe` (2 faces)
  * Slot 14: `CAT789D_LuzFreio` (2 faces)
  * Slot 15: `CAT789D_Setas_Dir` (5 faces: traseira + passarela frontal + passarela lateral)
  * Slot 16: `CAT789D_Farois_Position` (12 faces)
  * Slot 17: `CAT789D_Farois_Milha` (2 faces)
* **Texturas:** `VR - Mining 789D/Textures/` (todas unificadas no canal primário `UVMap` no Canal 0).
* **Malha Única:** `CAT789D_Mesh` (Single Skeletal Mesh com pesos 1.0 rígidos, evitando crashes de Vulkan).
* **Esqueleto (`CAT789D_Armature`):** 20 ossos alinhados estritamente com os eixos da Unreal Engine:
  * **+X Forward** (Frente), **+Z Up** (Cima), **+Y Right** (Direita - Left-Handed).
  * `VehicleMesh` (Root no solo em $0,0,0$).
  * Rodas: `FL`, `FR`, `IL`, `IR`, `TL`, `TR`.
  * Caçamba: `BasculaRig` $\rightarrow$ `cacamba` $\rightarrow$ `pistonUpperPiece` $\rightarrow$ `TopPiston`.
  * Pistão Inferior: `pistonBottomPiece` $\rightarrow$ `BasePiston`.
* **Dimensões Reais (Escala Métrica 1:1 / 100x nativo UE5):**
  * Comprimento: $12,88\text{ m}$ ($1.288\text{ cm}$) | Largura: $7,87\text{ m}$ ($787\text{ cm}$) | Altura: $6,49\text{ m}$ ($649\text{ cm}$) | Diâmetro dos Pneus: $3,42\text{ m}$ ($342\text{ cm}$).
  * Piso alinhado em $Z = 0.0$.

### 1.2 Projeto Unreal Engine 5 (`/home/iago/Documentos/Unreal Projects/MinaVargemGrande/`)
* **Veículo de Referência (Lógica Pronta e Funcional):**
  * `Content/Sirv/Blueprints/Vehicles/CAT793F/`
  * O CAT 789D reaproveita >90% da lógica do CAT 793F (`BP_CAT793F` $\rightarrow$ `BP_CAT789D`, `BP_CAT793F_Animation` $\rightarrow$ `BP_CAT789D_Animation`).
* **Display de Carga (TPMS / VIMS External Payload Display):**
  * Integrado via **Widget Component 3D (UMG)** no `BP_CAT789D`.
  * Capacidade nominal: **181 toneladas métricas**.
  * Variável condutora: `RemainingWeight` (float $0.0$ a $1.0$, já ligada ao Morph Target da carga).
  * Fórmula: $\text{Peso (t)} = \text{RemainingWeight} \times 181.0$.
* **Demarcação de Zonas de Velocidade e Gerador de YAML:**
  * Blueprint Ator: `BP_SpeedZone` (com `SplineComponent` fechado).
  * Script de exportação: `Content/Python/export_speed_zones.py`.
  * Gera o arquivo `.yaml` de limites de velocidade por coordenadas no formato `SpeedLimit_<ZoneCode>`.
* **Outros Veículos no Projeto:**
  * `Content/Sirv/Blueprints/Vehicles/PC8000/` (Escavadeira hidráulica)
  * `Content/Sirv/Blueprints/Vehicles/CAT7295/` (Pá elétrica)
  * `Content/Sirv/Blueprints/Vehicles/LeTourneau/` (Carregadeira de rodas L-1850)
  * `Content/Sirv/Blueprints/Vehicles/CATD10T/` (Trator de esteiras)
  * `Content/Sirv/Blueprints/Vehicles/VolkswagenT2/` (Kombi / Veículo de apoio)

### 1.3 Processamento Geoespacial e Nuvem de Pontos (`VR - Mining 789D/code/`)
* Scripts para conversão de LAS/TIF para Heightmaps e Texturas (`Las&Tif2Hm&Tx.py`, `mosaic_orthophotos_2026.py`, `generate_speed_zones_map.py`).

---

## 🌐 Frente 2: Portal do Laboratório RobSIC (`SiteRobsic/`)

### 2.1 Frontend (`SiteRobsic/SiteLab/`)
* **Tecnologia:** Flutter Web / Multiplataforma.
* **Comando de Build de Produção:**
  ```bash
  flutter build web --base-href "/" --dart-define=BASE_URL=https://robsic.unifei.edu.br
  ```
* **Geração do Pacote:**
  ```bash
  cd build/web && zip -r ../../web_prod.zip . && cd ../..
  ```

### 2.2 Backend (`SiteRobsic/strapi-robsic/`)
* **Tecnologia:** Strapi v4 Headless CMS (Node.js).
* **Desenvolvimento Local:** `npm run develop`
* **Build de Admin:** `npm run build`

### 2.3 Deploy em Produção
* Servidor: `robotica@robsic.unifei.edu.br`
* Procedimento detalhado em: `SiteRobsic/SobeProdução.md`

---

## 📄 Frente 3: Relatórios Técnicos de Estágio Vale (`RelatorioVale/`)

### 3.1 Relatório Ativo de Entregas (Mina do Pico 2026)
* **Diretório Ativo:** `/home/iago/Documentos/Estagio/RelatorioVale/relatorio-projeto-mina-pico-2026`
* **Regra:** Todo progresso ou alteração técnica relevante no simulador, rig, mapa ou integração deve ser registrado aqui.
* **Arquivo Mestre:** `0_relatorio_sirv.tex` (compilação via `latexmk 0_relatorio_sirv.tex`).
* **Seções Principais:** `1_Capa.tex`, `2_Resumo.tex`, `3_Introducao.tex`, `7_Produtos_entregas.tex`, `8_Conclusao.tex`.

### 3.2 Modelo de Referência Padrão (Mina Conceição)
* **Diretório Modelo:** `/home/iago/Documentos/Estagio/RelatorioVale/relatorio-projeto-app-atualizaco-mina-conceicao`
* Serve como padrão textual, de formatação LaTeX, ABNT e estruturação para novas seções do relatório da Mina do Pico.

### Comandos de Compilação (XeLaTeX):
* **Compilar PDF completo:**
  ```bash
  latexmk 0_relatorio_sirv.tex
  ```
* **Modo Contínuo (Watch Mode):**
  ```bash
  latexmk -pvc 0_relatorio_sirv.tex
  ```
* **Limpeza de temporários:**
  ```bash
  latexmk -c
  ```

---

## 📚 4. Arquivos de Documentação Detalhada Existentes

Quando precisar de detalhes profundos sobre especificações, consulte diretamente estes arquivos em vez de pesquisar na árvore:
1. `VR - Mining 789D/VALIDACAO_E_PROXIMOS_PASSOS_CAT789D.md` — Histórico do Rig, dimensões, ossos, pneus, física Chaos e mapeamento do CAT 793F vs 789D.
2. `VR - Mining 789D/CONTEXTO_E_PLANO_IMPLEMENTACAO_UE5_YAML.md` — Especificação do formato `.yaml` de limites de velocidade e script `export_speed_zones.py`.
3. `/home/iago/Documentos/Unreal Projects/MinaVargemGrande/DOCUMENTACAO_CARREGAMENTO_BILATERAL_ESCAVADEIRA.md` — Procedimento operacional de carregamento bilateral na mina.
4. `RelatorioVale/relatorio-projeto-mina-pico-2026/README.md` — Estrutura do relatório ativo da Mina do Pico.
5. `RelatorioVale/relatorio-projeto-app-atualizaco-mina-conceicao/README.md` — Detalhes da estrutura do relatório modelo de referência.
6. `SiteRobsic/SobeProdução.md` — Fluxo de deploy SCP/SSH para produção do site RobSIC.
