# Zyris Engine - Gameplay Ability System (GAS) Module

Este módulo implementa um robusto sistema de Gameplay Ability baseado em dados para a Zyris Engine, inspirado em padrões da indústria (como o GAS da Unreal) mas sob medida para a arquitetura baseada em nós da Godot.

## 📂 Arquitetura & Estrutura de Arquivos

O módulo é organizado em pastas lógicas separando lógica central, recursos de dados, componentes de cena e ferramentas do editor.

### `scene/` - Componentes de Cena

* **`ability_system_component.h/cpp` (`AbilitySystemComponent`)**:
  * **Propósito**: O coração do sistema. DEVE ser adicionado a qualquer Ator (Personagem, Inimigo, NPC) que deseje usar habilidades, atributos ou efeitos.
  * **Responsabilidades**: Gerencia Tags ativas, mantém AttributeSets, executa Habilidades, processa Efeitos e lida com Cues. Também se integra com componentes irmãos (`AnimationPlayer`, `AudioStreamPlayer`) se configurado.

### `resources/` - Definições de Dados

* **`attribute_set.h/cpp` (`AttributeSet`)**:
  * **Propósito**: Define um conjunto de valores float (Vida, Mana, Stamina, etc.).
  * **Responsabilidades**: Armazena `BaseValue` (permanente) e `CurrentValue` (temporário/modificado). Emite sinais quando valores mudam.
* **`gameplay_ability.h/cpp` (`GameplayAbility`)**:
  * **Propósito**: Recurso scriptável definindo uma ação.
  * **Responsabilidades**: Contém lógica (`_activate_ability`), Custos, Cooldowns e requisitos de Tag (Tags de Ativação/Cancelamento/Bloqueio).
* **`gameplay_effect.h/cpp` (`GameplayEffect`)**:
  * **Propósito**: Dados descrevendo uma mudança no estado.
  * **Responsabilidades**: Define modificadores de atributo (Adicionar, Multiplicar, Substituir), Duração (Instantâneo, Finito, Infinito) e Tags Concedidas.
* **`gameplay_cue.h/cpp` (`GameplayCue`)**:
  * **Propósito**: Definição de feedback Áudio/Visual.
  * **Responsabilidades**: Referencia VFX (`PackedScene`), SFX (`AudioStream`) e nomes de Animação. Executado pelo ASC.
* **`gameplay_tag.h/cpp` (`GameplayTag`)**: Representa uma tag hierárquica (ex: `state.debuff.stun`).
* **`gameplay_tag_container.h/cpp` (`GameplayTagContainer`)**: Uma coleção de tags com lógica de correspondência rápida (`has_tag`, `has_any`, `has_all`).

### `core/` - Lógica Interna & Tipos

* **`gameplay_ability_spec.h/cpp`**: Instância em tempo de execução de uma Habilidade (rastreando estado ativo).
* **`gameplay_effect_spec.h/cpp`**: Instância em tempo de execução de um Efeito (rastreando duração).
* **`gameplay_cue_spec.h/cpp`**: Manipulador em tempo de execução para Cues (spawna VFX/SFX ou chama nós irmãos).

### `editor/` - Ferramentas

* **`gameplay_editor_plugin.h/cpp`**: Lida com integração no editor, customização do inspector e configurações do projeto.

---

## 🛠️ Tutoriais & Exemplos

### 1. Configurando um Ator

Adicione um nó `AbilitySystemComponent` à sua cena. Opcionalmente, linke componentes irmãos para manipulação automática.

```gdscript
extends CharacterBody3D

@onready var asc: AbilitySystemComponent = $AbilitySystemComponent

func _ready():
    # Configurar componentes irmãos (opcional, mas recomendado para Cues)
    asc.set_audio_player_path("AudioStreamPlayer3D")
    asc.set_animation_player_path("AnimationPlayer")

    # Inicializar Atributos
    var health_set = AttributeSet.new()
    health_set.register_attribute("health", 100.0)
    health_set.register_attribute("max_health", 100.0)

    # Conectar sinais
    health_set.attribute_changed.connect(_on_health_changed)

    # Adicionar ao ASC
    asc.add_attribute_set(health_set)

func _on_health_changed(attr_name, old_val, new_val):
    if attr_name == "health":
        print("Vida mudou: ", new_val)
```

### 2. Criando uma Habilidade (Gameplay Ability)

Crie um novo script estendendo `GameplayAbility` e salve-o como um recurso `.tres`.

```gdscript
# minha_bola_de_fogo.gd
extends GameplayAbility

func _activate_ability():
    # 'asc' está disponível via contexto, mas tipicamente o uso no GAS da Zyris
    # passa 'asc' para ativar.

    print("Bola de Fogo Conjurada!")

    # Lógica para spawnar projétil iria aqui

    # Finalizar habilidade
    end_ability()
```

### 3. Aplicando um Efeito (Dano, Buff)

Efeitos são geralmente Recursos apenas de dados, mas você pode criá-los via código também.

```gdscript
func take_damage(amount: float):
    # Criar um efeito de dano instantâneo
    var damage_effect = GameplayEffect.new()
    damage_effect.duration_type = GameplayEffect.GE_DURATION_INSTANT

    # Adicionar modificador: Vida = Vida + (-amount)
    damage_effect.add_modifier("health", GameplayEffect.GE_MOD_ADD, -amount)

    # Aplicar em si mesmo (ou no ASC do alvo)
    asc.apply_gameplay_effect_to_self(damage_effect)
```

### 4. Usando Cues (Gameplay Cues)

Cues desacoplam a lógica dos visuais.

1. Crie um recurso `GameplayCue`.
2. Designe uma `Cena VFX` (Partícula) e/ou `Som`.
3. Designe-o na propriedade `Cue Resource` da sua **Habilidade**.

O ASC executará automaticamente quando a habilidade ativar.

Ou dispare manualmente:

```gdscript
@export var hit_cue: GameplayCue

func on_hit(location: Vector3):
    asc.execute_gameplay_cue(hit_cue, location)
```

### 5. Usando Tags

Tags são primárias para controle de estado (ex: Atordoado, Silenciado).

```gdscript
# Checar se o dono tem uma tag específica
if asc.has_tag("state.debuff.stun"):
    print("Estou atordoado, não posso mover!")

# Adicionar uma tag manualmente (geralmente feito via GameplayEffect)
asc.add_tag("state.buff.speed_boost")
```

---

## ⚠️ Notas Importantes

* **Networking**: Este módulo é projetado com replicação em mente, mas a implementação do netcode é lidada na camada de `scene`.
* **Performance**: A lógica central é C++ para máximo throughput. Interações com GDScript são otimizadas via chamadas `GDVIRTUAL`.
* **Visuais**: Use `GameplayCue` para TODOS os feedbacks visuais. Não hardcode partículas na lógica da Habilidade se possível.
