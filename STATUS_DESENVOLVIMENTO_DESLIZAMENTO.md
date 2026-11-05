# Status de Desenvolvimento: Física de Deslizamento, Dinâmica do CAT 789D e Zonas de Rejeito (Mina do Pico)

**Data de Atualização:** 05/11/2026  
**Ambiente:** `simulation-pc` (Laboratório RobSIC) / `panther` (Casa)  
**Branch UE5:** `development` (Azure DevOps + Git LFS)  
**Branch Estágio:** `main` (GitHub - `VRMiningExternalFiles`)

---

## 📌 1. Resumo do que foi Concluído e Validado

### A. Demarcação Territorial do Rejeito Filtrado (Cianita)
1. **Estrada / Rampa da Cianita:**
   * Utilização do ator `Pontalete_AutoSpline_02` com as tags nativas `SlipZone` e `SlipZone_Road`.
   * Preservação da linha central e largura transversal real da pista em `Scale.Y` (`Half-Width` de 900 a 1200 cm).
2. **Praças de Carga e Descarga:**
   * `SlipZone_Descarga_Cianita`: praça de manobra das 6 pilhas no fundo da Cianita (cota inferior da rampa).
   * `SlipZone_Carga_Rejeito`: pátio de operação da carregadeira LeTourneau L-1850 na Usina.
3. **Scripts de Automação:**
   * `sync_slip_zones.py`: gerador e sincronizador paramétrico das zonas usando diretamente `BP_SpeedZone`.
   * `export_speed_zones.py`: filtro condicional para ignorar zonas físicas no `.yaml` quando `SpeedLimit <= 0.0`.
   * `sync_landscape_splines.py`: persistência automática das tags `SlipZone`.

---

### B. Lógica de Detecção e Acoplamento de Carga Útil no CAT 789D
1. **Função `UpdateSlipZoneState` no `BP_CAT789D`:**
   * Detecção por distância à spline na estrada e produto vetorial (*Dot Product*) nas praças.
   * Feedback visual via `Print String` (`"REJEITO CIANITA"` vs `"PISTA PADRÃO"`).
2. **Modulação Dinâmica por Carga Útil (`RemainingWeight`):**
   * Como a física do Chaos mantém a massa rígida constante em 150t, a inércia e sensibilidade de carga foram acopladas via interpolação linear contínua (*Lerp*):
     $$\text{Mult}_{\text{atrito}} = \text{Lerp}(A = 0.50, \, B = 0.38, \, \text{Alpha} = 1.0 - \text{RemainingWeight})$$
   * **Vazio (`RemainingWeight = 1.0`):** atrito `0.50` (atrito efetivo pneu-solo $\mu \approx 0{,}35$).
   * **100% Carregado (`RemainingWeight = 0.0`):** atrito `0.38` (atrito efetivo pneu-solo $\mu \approx 0{,}266$).
   * Aplicado via `For Loop` nas 6 rodas através do nó `Set Wheel Friction Multiplier`.

---

### C. Calibração Mecânica e Eliminação de Trancos
1. **`Engine Brake Effect` ajustado de `100.0` para `1.0`:**
   * Eliminou o tranco violento, patinação reversa e desaceleração espúria de $-3\text{ m/s}^2$ ao soltar o acelerador em transmissão manual.
   * Preserva a rolagem livre por inércia (*coasting*) de um veículo de 150 a 330 toneladas.
2. **Descarte do nó `Add Force`:**
   * Comprovado experimentalmente que forças lineares externas no chassi causam ressonância de tração (*traction chattering* / efeito "anda-trava") no solver da Chaos. O controle puro por atrito no pneu garantiu resposta suave e realista.

---

## 📊 2. Dados Experimentais de Frenagem Validados a 40 km/h

| Condição Operacional | Estado de Carga | Multiplicador | Tempo de Parada | Desaceleração Média | Distância Estimada |
| :--- | :---: | :---: | :---: | :---: | :---: |
| **Pista Padrão** | Vazio ($150\text{ t}$) | `1.00` | $3{,}0\text{ a } 4{,}0\text{ s}$ | $\sim 3{,}1\text{ m/s}^2$ ($0{,}32 g$) | $\approx \mathbf{20\text{ m}}$ |
| **Rejeito Cianita** | Vazio ($150\text{ t}$) | `0.50` | $5{,}0\text{ a } 6{,}0\text{ s}$ | $\sim 2{,}0\text{ m/s}^2$ ($0{,}20 g$) | $\approx \mathbf{30\text{ m}}$ ($+50\%$) |
| **Rejeito Cianita** | Carregado ($331\text{ t}$) | `0.38` | $\mathbf{\sim 8{,}0\text{ s}}$ | $\mathbf{\sim 1{,}39\text{ m/s}^2}$ ($0{,}14 g$) | $\approx \mathbf{45\text{ m}}$ ($+125\%$) |

---

## 📄 3. Documentação Técnica no Relatório Vale (UNIFEI)

* **Etapa 4 oficializada no relatório:**
  * Arquivo criado: [`RelatorioVale/relatorio-projeto-mina-pico-2026/MI/MIA4.tex`](file:///home/simulation-pc/Desktop/Estagio/RelatorioVale/relatorio-projeto-mina-pico-2026/MI/MIA4.tex)
  * Resumo atualizado: [`2_Resumo.tex`](file:///home/simulation-pc/Desktop/Estagio/RelatorioVale/relatorio-projeto-mina-pico-2026/2_Resumo.tex)
  * Tabela de entregas atualizada: [`7_Produtos_entregas.tex`](file:///home/simulation-pc/Desktop/Estagio/RelatorioVale/relatorio-projeto-mina-pico-2026/7_Produtos_entregas.tex) (Etapa 4 marcada como **Concluída**)
  * Conclusão expandida: [`8_Conclusao.tex`](file:///home/simulation-pc/Desktop/Estagio/RelatorioVale/relatorio-projeto-mina-pico-2026/8_Conclusao.tex)
  * Figuras incorporadas: grafo completo do Blueprint, telemetria do caminhão vazio e carregado.
  * Commits enviados para o repositório remoto `VRMiningExternalFiles` no GitHub.
