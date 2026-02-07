# Documentação: Sistema de Persistência Core (v2)

Este guia explica como implementar um sistema de salvamento robusto e profissional usando o **SaveServer** e a anotação `@persistent`.

---

## 1. Filosofia Central: O Modelo Reconstrutivo

O nosso sistema de persistência é construído sobre um **Modelo de ID Reconstrutivo**. Isso garante que tanto nodes estáticos quanto dinâmicos possam ser perfeitamente restaurados entre sessões.

- 🏛️ **Nodes Estáticos:** Objetos que sempre existem na sua cena (como o Player ou a UI) são identificados via `persistence_id` e restaurados automaticamente.
- ⚡ **Nodes Dinâmicos:** Objetos spawnados durante o gameplay (Inimigos, Loot, Projéteis) devem ser re-instanciados no carregamento. O sistema então reconcilia o estado interno deles usando seu `persistence_id` único.

## 2. Configurando uma Variável Persistente

Salvar dados é tão simples quanto adicionar a anotação `@persistent`.

> [!IMPORTANT]
> **A tipagem explícita é obrigatória.** Para performance e segurança, variáveis marcadas como `@persistent` devem ter um tipo definido (ex: `: int`, `: String`).

```gdscript
extends CharacterBody2D

@persistent var health: int = 100
@persistent var inventory_items: Array[String] = []
@persistent var current_checkpoint_id: StringName = &"checkpoint_alpha"
```

### 2.1 Usando Tags para Salvamento Seletivo

Você pode agrupar variáveis persistentes usando **Tags**. Isso permite salvar apenas dados de "Combate" ou de "Inventário" separadamente.

```gdscript
@persistent("stats") var strength: int = 10
@persistent("inventory") var items: Array = []
```

## 3. Lidando com Entidades Dinâmicas (Padrão Manager)

Ao spawnar objetos em runtime, você deve gerenciar a fase de **Reconstrução**.

### Passo A: Capturando o Estado (`_save_persistence`)

O node pai deve implementar `_save_persistence` para armazenar metadados sobre seus filhos dinâmicos.

```gdscript
# LevelManager.gd
func _save_persistence(state: Dictionary):
    var enemy_data = []
    for enemy in $Enemies.get_children():
        enemy_data.append({
            "id": enemy.persistence_id, # O ID único gerenciado pela engine
            "type": enemy.type_id,
            "pos": enemy.global_position
        })
    state["active_enemies"] = enemy_data
```

### Passo B: Reconstrutindo Nodes (`_load_persistence`)

Durante a fase de carregamento, recrie os nodes e **re-aplique seus IDs** antes de adicioná-los à Árvore.

```gdscript
# LevelManager.gd
func _load_persistence(data: Dictionary):
    if not data.has("active_enemies"): return

    for info in data["active_enemies"]:
        var enemy = ENEMY_SCENE.instantiate()

        # CRÍTICO: Re-aplique o persistence_id ANTES de adicionar à árvore
        enemy.persistence_id = info["id"]
        enemy.global_position = info["pos"]

        $Enemies.add_child(enemy)

        # Uma vez na árvore, o SaveServer injeta automaticamente as
        # propriedades @persistent (como vida/stats) a partir do snapshot.
```

## 4. Hooks Manuais e Dados Customizados

Use estes hooks para lógicas complexas que não são propriedades simples.

| Hook | Propósito |
| :--- | :--- |
| `_save_persistence(state: Dictionary)` | Preenche o dicionário `state` com valores customizados. |
| `_load_persistence(data: Dictionary)` | Processa valores customizados após o carregamento automático das propriedades. |

```gdscript
func _save_persistence(state: Dictionary):
    state["custom_logic_data"] = { "power_level": calculate_power() }

func _load_persistence(data: Dictionary):
    var power = data.get("custom_logic_data", {}).get("power_level", 1.0)
    apply_power_modifier(power)
```

## 5. Evolução Avançada: Migrações

Ao lançar uma atualização que muda a estrutura dos seus dados, use **Migrações** para evitar corrupção de saves.

```gdscript
func _init():
    # Upgrade de save da versão 1.0 para 1.1
    SaveServer.register_migration("1.0", "1.1", func(data: Dictionary):
        if data.has("old_xp_system"):
            data["new_rank_system"] = convert_xp_to_rank(data["old_xp_system"])
    )
```

## 6. Otimização e Melhores Práticas

1. 🎯 **Minimalismo:** Apenas marque variáveis que *realmente* mudam.
2. 🔒 **Segurança:** Ative **ZSTD Compression** e **Criptografia AES** em `Project Settings > Application > Persistence` para builds de produção.
3. 🔄 **Backups:** Mantenha `backup_enabled` ativo para proteger os jogadores de quedas de energia durante o salvamento.
4. 🤖 **IDs Automatizados:** Nunca defina `persistence_id` manualmente no gameplay normal. Deixe a engine gerar; você apenas o re-aplica na reconstrução (load).

---
*Framework de Persistência de Nível Profissional.*
