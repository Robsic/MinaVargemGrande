# Transição de Desenvolvimento: Física de Deslizamento e Zonas de Rejeito (CAT 789D - Mina do Pico)

**Data:** 04/11/2026  
**Ambiente de Origem:** `simulation-pc` (Laboratório RobSIC)  
**Ambiente de Destino:** `panther` (Casa)  
**Branch UE5:** `development` (Azure DevOps + Git LFS)  
**Branch Estágio:** `main` (GitHub - `VRMiningExternalFiles`)

---

## 📌 1. Resumo do que foi Desenvolvido e Concluído

### A. Demarcação Territorial do Rejeito Filtrado (Cianita)
1. **Estrada / Rampa da Cianita:**
   * Utilização direta do ator `Pontalete_AutoSpline_02` com as tags nativas `SlipZone` e `SlipZone_Road`.
   * Preservação da linha central e largura transversal real da pista em `Scale.Y` (`Half-Width` de 900 a 1200 cm).
2. **Praças de Carga e Descarga:**
   * Implementadas utilizando instâncias da classe nativa `BP_SpeedZone` com tags `SlipZone` e `SlipZone_Area`:
     * `SlipZone_Descarga_Cianita`: praça de manobra das 6 pilhas no fundo da Cianita (cota inferior da rampa).
     * `SlipZone_Carga_Rejeito`: pátio de operação da carregadeira LeTourneau L-1850 na Usina.
3. **Scripts de Automação:**
   * `sync_slip_zones.py`: gerador e sincronizador paramétrico das zonas usando diretamente `BP_SpeedZone` (asset duplicado `BP_SlipZone` removido).
   * `export_speed_zones.py`: atualizado com filtro condicional para ignorar zonas puramente físicas no `.yaml` quando `SpeedLimit <= 0.0`.
   * `sync_landscape_splines.py`: atualizado para preservar as tags `SlipZone` automaticamente em reprocessamentos.

---

### B. Lógica de Detecção no Caminhão (`BP_CAT789D`)
1. **Variáveis Criadas:**
   * `SlipZoneActors` (`Array` de `Actor Object Reference`): populada no `Event BeginPlay` via `Get All Actors With Tag("SlipZone")`.
   * `Is in Slip Zone` (`Boolean`): atualizada em runtime pelo `Event Tick`.
2. **Função `UpdateSlipZoneState`:**
   * Reseta `Is in Slip Zone = False` no início do frame.
   * Itera sobre os atores de zona via `For Each Loop`.
   * **Se for Estrada (`SlipZone_Road`):** checa se `Distance2D(Caminhão, Ponto Mais Próximo na Spline) <= Scale.Y`.
   * **Se for Praça (`SlipZone_Area`):** cálculo geométrico vetorial exato via **`Dot Product`** com o `Right Vector` da spline (`>= 0.0`) e margem de segurança radial (`Distance <= 6000.0`).
   * Validação em tela via `Print String` alternando entre `PISTA PADRÃO` e `REJEITO CIANITA`.

---

### C. Física de Redução de Atrito (Validada na Pista)
1. **`Set Wheel Friction Multiplier`:**
   * Chamado no `ChaosWheeledVehicleMovementComponent` via `For Loop` para as 6 rodas (índices 0 a 5).
   * Multiplicador configurado:
     * `Pista Normal`: `1.0` (atrito padrão).
     * `Rejeito Cianita`: `0.60` a `0.65`.
2. **Resultados Observados em Teste Prático:**
   * O caminhão desliza ao frear bruscamente a ~30 mph, aumentando realisticamente o espaço de frenagem.
   * Em curvas em alta velocidade, o caminhão apresenta perda de aderência lateral e saída de frente (*understeer*).

---

## ⚙️ 2. Diagnóstico Atual: Tranco e Rotação Negativa ao Soltar o Acelerador

Durante os testes com **transmissão manual travada em marchas baixas**:
* **Comportamento observado:** Ao soltar a tecla 'W', ocorre uma desaceleração brusca de $\sim -2\text{ a } -3\text{ m/s}^2$, com as rodas traseiras patinando/girando momentaneamente para trás e o giro do motor subindo para $\sim 1800\text{ RPM}$.
* **Causa Raiz:** O motor do CAT 789D (2.100 HP) possui enorme torque resistivo de freio-motor mecânico. Em piso firme com atrito `1.0`, o pneu segura o tranco. Porém, com atrito reduzido para `0.60` nas 4 rodas de tração traseiras sem desacoplamento de embreagem, o freio-motor supera o limite de aderência do pneu com o solo fofo, quebrando a tração e induzindo patinação reversa temporária.

---

## 🚀 3. Próximos Passos (Para dar continuidade na `panther`)

Ao abrir o projeto na máquina de casa (`panther`):

1. **Ajuste de Freio-Motor e Amortecimento de Roda (Prioridade 1):**
   * No `BP_CAT789D` $\rightarrow$ `VehicleMovement`:
     * Localizar `Engine Brake Effect` e reduzir para `0.05` a `0.10` (suaviza a retenção súbita do motor ao soltar o acelerador em marchas manuais).
   * No asset da roda traseira `BP_CAT789D_RW`:
     * Reduzir `Damping Rate` para `0.25`.
2. **Diferenciação de Atrito Dianteiro vs. Traseiro (Opcional/Recomendado):**
   * Na função `UpdateSlipZoneState`:
     * Rodas Dianteiras (0 e 1): manter `0.55` a `0.60` (garante perda direcional e escorregamento da frente).
     * Rodas Traseiras de Tração (2, 3, 4 e 5): usar `0.75` a `0.80` (assegura tração estável sem patinação reversa indesejada).
3. **Resistência de Areia / Afundamento (*Sinkage Drag* Realista):**
   * Reconectar o `Add Force` com aceleração suave de apenas `-15.0` a `-25.0 cm/s²` (`Accel Change = True`), ativando apenas quando `Forward Speed > 100 cm/s` e com aceleração positiva.
4. **Acoplamento com a Carga (`RemainingWeight`):**
   * Modular a força de frenagem e escorregamento na descida da Cianita em função da variável `RemainingWeight` (0.0 a 1.0).
