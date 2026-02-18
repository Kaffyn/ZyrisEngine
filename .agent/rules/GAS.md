# Gameplay Ability System - GAS (Zyris v3)

## 1. Contexto e Filosofia

O **Gameplay Ability System (GAS)** na Zyris Engine é um framework nativo de alta performance projetado para lidar com lógica de combate, habilidades e atributos de forma modular e baseada em dados.

Diferente de implementações em script puro, o GAS do Zyris foca em:

- **Performance Bruta:** Cálculos de modificadores e processamento de tags em C++.
- **Data-Driven:** Habilidades e efeitos são `Resource`, permitindo que designers criem mecânicas sem tocar em código.
- **Multiplayer Nativo:** Suporte a predição e reconciliação de atributos e estados (Determinismo).
- **Desacoplamento:** A lógica de "como um dano é calculado" é separada de "quem causa o dano".

## 2. Arquitetura Core (C++)

O sistema está organizado em subpastas em `modules/gas/` para melhor manutenibilidade:

- `core/`: Lógica base, Tags e Specs.
- `resources/`: Definições de Atributos, Habilidades e Efeitos.
- `scene/`: Componentes de Cena (ASC).

### 2.1 AbilitySystemComponent (ASC)

O "coração" do sistema. Deve ser anexado a qualquer `Node` que interaja com o sistema.

- **API Principal:**
  - `give_ability(GameplayAbility ability)`: Registra uma nova habilidade.
  - `try_activate_ability(GameplayAbility ability)`: Tenta disparar uma lógica.
  - `apply_gameplay_effect_to_self(GameplayEffect effect)`: Aplica modificadores.
  - `has_tag(StringName tag)`: Verifica se possui uma tag (hierárquico).
  - `get_attribute_value(StringName name)`: Retorna o valor atual de um atributo.

### 2.2 AttributeSet

Define os dados brutos (HP, Mana, Stamina, Força).

- **Modularidade:** Suporta múltiplos sets por ASC.
- **Cálculo de Modificadores:** Suporta `BaseValue` e `CurrentValue` (com buffs/debuffs).

### 2.3 GameplayAbility (Resource)

Define a lógica de "o que acontece".

- **Fluxo de Vida:** `can_activate` -> `activate_ability` -> `end_ability`.
- **Tags:** Gerencia tags de habilidade, cancelamento e bloqueio.

### 2.4 GameplayEffect (Resource)

Regras para alterar atributos ou tags.

- **Duração:** Instantâneo, Duração ou Infinito.
- **Modificadores:** Operações de `ADD`, `MULTIPLY`, `DIVIDE` e `OVERRIDE`.
- **Costs:** Custos de ativação (Mana, Stamina, etc.).
- **Cooldowns:** Tempo de recarga entre usos.

### 2.5 GameplayCue (Resource)

Sistema de feedback visual e sonoro para eventos de gameplay.

- **Trigger:** Executado quando efeitos são aplicados/removidos ou habilidades ativadas.
- **Tipos:** `OnActive` (enquanto ativo), `OnExecute` (instantâneo), `OnRemove` (ao remover).
- **Uso:** Spawn de VFX, SFX, screen shake, números flutuantes.

### 2.6 Attribute Events

Signals emitidos quando atributos mudam.

- **`attribute_changed(attribute_name, old_value, new_value)`**: Disparado em qualquer mudança.
- **`attribute_pre_change(attribute_name, old_value, new_value)`**: Antes da mudança (pode ser cancelado).
- **Uso:** Atualizar UI (health bars), triggers de gameplay (morte quando HP = 0).

### 2.7 Gameplay Tasks

Sistema assíncrono para operações de longa duração dentro de habilidades.

- **Tasks:** `WaitDelay`, `WaitInputPress`, `MoveToLocation`, `SpawnProjectile`.
- **Async:** Permite escrever lógica de habilidade linear usando `await`.

### 2.8 Target Data

Pipeline de validação e replicação de alvos.

- **Fluxo:** Cliente seleciona alvos -> Envia Data Handle para Servidor -> Servidor valida e aplica.
- **Support:** Line Trace, Sphere Overlap, Box, Custom Shapes.

### 2.9 Magnitude Calculation (MMC)

Cálculos complexos de magnitude para efeitos.

- **Custom logic:** Permite definir curvas de dano, escala por atributos (Força, Inteligência) e modificadores de ambiente.
- **GEE:** Gameplay Effect Execution para lógica arbitrária ao aplicar efeitos.

### 2.10 Multiplayer Core

Sistema nativo de rollback e reconciliação.

- **Prediction:** Cliente simula ações instantaneamente.
- **Reconciliation:** Servidor corrige estado se houver divergência.
- **Netcode:** Otimizado para baixa latência.

## 3. Gameplay Tags

As **GameplayTags** são `StringName` hierárquicos otimizados.

- **Exemplo:** `state.buff.speed`, `state.debuff.stun`.

## 4. Interface de Script (GDScript)

```gdscript
extends GameplayAbility

func _activate_ability():
    print("Ativando habilidade!")
    end_ability()
```

## 5. Ferramentas do Editor (GAS Studio)

O módulo inclui ferramentas integradas para facilitar o desenvolvimento:

- **GAS Studio:** Interface visual para criação e debug de habilidades.
- **Tag Manager:** Editor hierárquico para gerenciamento centralizado de tags.
- **Visual Debugger:** Ferramenta para inspeção em tempo real de ASCs, atributos e efeitos ativos.

## 6. Sincronização e Persistência

Integrado nativamente com o `SaveServer` e com suporte completo a multiplayer (predição e reconciliação).

---

_Este documento define a especificação técnica oficial do módulo GAS na Zyris Engine._
