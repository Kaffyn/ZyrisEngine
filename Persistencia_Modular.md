# Sistema de Persistência Modular - Zyris Engine (v2.1)

## 1. O Conceito: "Git para Gameplay"

Atualmente, a maioria das engines salva o estado do jogo como um "Full Dump" (um snapshot completo da memória/cena). Em jogos massivos, isso causa "Save Stutter" (lag ao salvar) e arquivos gigantescos.

A Zyris v2 evolui para o modelo de **Estados Incrementais**, inspirado no sistema de commits do Git e na infraestrutura do GitHub.

### Como funciona

* **Base (Loaded State):** Quando você entra em uma área, a Zyris mantém o estado original carregado (`base_snapshot`) como um objeto de leitura imutável.
* **Incrementos (Modified State):** Apenas os objetos cujas propriedades mudarem em relação à base são capturados.
* **Modularidade:** Se você mudar apenas o inventário, o save incremental gravará apenas o nó correspondente ao Inventário, sem percorrer o resto do mundo.

## 2. Abstração de Hierarquia via Persistence ID

O problema clássico do Godot/Unity: se você renomear um nó ou movê-lo de `Cena/Player` para `Cena/Combat/Player`, o sistema de save quebra porque o caminho (NodePath) mudou.

**Na Zyris:**

* Implementamos o **Global ID Registry**.
* Se um `persistence_id` estiver presente, a engine ignora o NodePath e vincula os dados diretamente ao objeto registrado globalmente.
* Isso permite que o Player seja movido entre sub-cenas durante o runtime sem perder o progresso.

## 3. Fluxo de Trabalho do Save Incremental (Non-Sweeping)

Para evitar a "Varredura Completa":

1. **Dirty Tracking:** Toda vez que um `@persistent var` é alterado via `set()`, o objeto se registra na lista de `dirty_objects` do `SaveServer`.
2. **Explicit Save:** Ao chamar `save_incremental()`, o server percorre **apenas** a lista de objetos sujos.
3. **Dictionary Merge:** No carregamento, a Zyris aplica: `Base Layout` + `Incremental Commit`.

## 4. Benefícios AAA

* **Performance:** Escrita em disco reduzida em até 95% em saves frequentes.
* **Estabilidade:** Menor chance de corrupção total do save, já que salvamos incrementos modulares.
* **Flexibilidade:** Permite "Undo/Redo" de estados de mundo carregando commits anteriores.

---
*Assinado: Assistente (Dev Core)*
