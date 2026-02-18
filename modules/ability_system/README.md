# Zyris Engine - Ability System Module

Este módulo implementa um robusto sistema de Ability System baseado em dados para a Zyris Engine, inspirado em padrões da indústria mas sob medida para a arquitetura baseada em nós da Godot.

## 📂 Arquitetura & Estrutura de Arquivos

O módulo é organizado em pastas lógicas separando lógica central, recursos de dados, componentes de cena e ferramentas do editor.

### Raiz do Módulo - Registro e Configuração

- **`register_types.h/cpp`**: Define os pontos de entrada do módulo, registrando todas as classes no `ClassDB` e inicializando o singleton `AbilitySystem`.
- **`config.py`**: Configuração do SCons e definição das classes que possuem documentação XML.
- **`SCsub`**: Script de compilação que organiza a inclusão de todas as subpastas.

### `scene/` - Componentes de Cena

- **`ability_system_component.h/cpp` (`AbilitySystemComponent`)**:
  - **Propósito**: O cérebro do Ator. Gerencia habilidades, atributos e estados.
  - **Características**:
    - **Sibling Cache**: Detecta e cacheia automaticamente nós irmãos (`CharacterBody2D/3D`, `AnimationPlayer`, `AudioStreamPlayer2D/3D`, `RayCast2D/3D`, `GPUParticles2D/3D`, `Marker2D/3D`).
    - **Reactive Signals**: Emite sinais nativos como `attribute_changed`, `tag_changed` e `ability_activated`.
    - **Performance**: Processamento de tags hierárquicas e modificadores em C++ puro.
    - **Netcode**: Métodos `get_net_state` e `apply_net_state` para sincronização multiplayer.

### `resources/` - Definições de Dados

- **`ability_system_attribute_set.h/cpp` (`AbilitySystemAttributeSet`)**: Define conjuntos de atributos (Vida, Mana, etc.) com suporte a valores Base e Atuais.
- **`ability_system_ability.h/cpp` (`AbilitySystemAbility`)**: Recurso que define a lógica de uma habilidade, incluindo tags de ativação e bloqueio.
- **`ability_system_effect.h/cpp` (`AbilitySystemEffect`)**: Define modificadores de atributos, durações e tags concedidas.
- **`ability_system_cue.h/cpp` (`AbilitySystemCue`)**: Define feedbacks visuais e sonoros disparados por eventos.
- **`ability_system_tag.h/cpp` (`AbilitySystemTag`)**: Recurso que representa uma única Gameplay Tag hierárquica.
- **`ability_system_tag_container.h/cpp` (`AbilitySystemTagContainer`)**: Container para gerenciar coleções de tags com buscas otimizadas.

### `core/` - Lógica Interna & Controle

- **`ability_system.h/cpp` (`AbilitySystem`)**: Singleton central que atua como o registro mestre de Gameplay Tags do projeto.
- **`ability_system_ability_spec.h/cpp`**: Representa uma instância ativa de uma habilidade em um ASC.
- **`ability_system_effect_spec.h/cpp`**: Representa um efeito aplicado e ativo, rastreando sua duração restante.
- **`ability_system_cue_spec.h/cpp`**: Gerenciador em tempo de execução para a execução de Cues.
- **`ability_system_magnitude_calculation.h/cpp` (MMC)**: Classe base para criar fórmulas complexas de cálculo de magnitude (ex: dano baseado em stats).
- **`ability_system_task.h/cpp`**: Base para tarefas assíncronas dentro de habilidades (ex: esperar por um delay ou input).
- **`ability_system_target_data.h/cpp`**: Estrutura serializável para transportar informações de alvos entre cliente e servidor.

### `editor/` - Ferramentas

- **`ability_system_editor_plugin.h/cpp`**: Plugin principal do editor. Integra seletores especializados de Tags e Atributos no Inspector da Godot.

---

## 🛠️ Tutoriais & Exemplos

### 1. Sistema de Combate (Net State e MMC)

```gdscript
extends CharacterBody3D

@onready var asc = $AbilitySystemComponent

func perform_attack(target_asc: AbilitySystemComponent):
    var spec = asc.make_outgoing_spec(load("res://effects/damage_fire.tres"))
    spec.set_magnitude("base_damage", 50.0 * asc.get_attribute_value("stats.intellect"))
    asc.apply_gameplay_effect_spec_to_target(spec, target_asc)
```

### 2. Habilidades Assíncronas (Tasks)

```gdscript
extends AbilitySystemAbility

func _activate_ability():
    asc.play_montage("attack_sequence_a")
    # Exemplo conceitual de Task assíncrona
    var event = await asc.wait_for_gameplay_event("event.combat.impact").completed
    _spawn_hitbox(event.position)
    end_ability()
```

### 3. UI Reativa (Signals)

```gdscript
extends ProgressBar

func _ready():
    var asc = get_owner().get_node("AbilitySystemComponent")
    asc.attribute_changed.connect(_on_health_changed)

func _on_health_changed(attr_name, old, new):
    if attr_name == &"health":
        create_tween().tween_property(self, "value", new, 0.2)
```

---

## ⚠️ Notas Importantes

- **Networking**: O netcode utiliza sincronização de **Net State** (não confunda com save snapshots).
- **Performance**: Todo o processamento pesado de tags e modificadores é feito em C++.
- **Visuais**: Utilize `AbilitySystemCue` para manter a lógica desacoplada dos efeitos visuais.

_Este documento define a especificação técnica oficial do módulo GAS na Zyris Engine._
