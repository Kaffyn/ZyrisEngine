# Implementar Persistência de Alto Nível: SaveServer e anotação @persistent

**Issue Relacionada:** <https://github.com/godotengine/godot-proposals/issues/14140>

## Resumo

Este PR introduz o **SaveServer**, um sistema padronizado de persistência para a Godot. Em vez de impor uma forma específica de salvar jogos, ele fornece a infraestrutura robusta (singleton, threading, encriptação, integridade) que os desenvolvedores atualmente precisam construir do zero. Ele expõe tanto uma API para prototipagem rápida (`@persistent`) quanto ganchos de baixo nível (`_save_persistence`/`_load_persistence`) para permitir que os usuários arquitetem seus próprios sistemas de save de forma eficiente.

## Visuais

<!-- Adicione screenshots ou vídeos demonstrando a funcionalidade em ação aqui. -->
<!-- Exemplo: Um GIF mostrando a restauração de estado após fechar/reabrir o jogo. -->

[Inserir Screenshot/Vídeo Aqui]

## Projeto de Teste

Para facilitar testes e revisão, preparei um projeto de demonstração (**NeonShooter**) que utiliza totalmente as funcionalidades do `SaveServer`.

- **Repositório:** [Link para o Repositório do NeonShooter]
- **O que testar:**
  - Jogue o jogo, colete itens e salve.
  - Feche e reabra para verificar a restauração do estado.
  - Verifique a pasta `user://saves/` para ver os arquivos gerados.

## O Problema

Salvar o estado do jogo na Godot agora é... tedioso. Mesmo para algo simples como vida e posição do jogador, você acaba escrevendo muito código repetitivo. Você constrói dicionários manualmente, serializa para JSON ou binário, lida com I/O de arquivos, e se quiser evitar quedas de frame durante autosave, precisa implementar seu próprio threading. Quer encriptação? Checksums? Arquivos de backup em caso de corrupção? Tudo isso é por sua conta.

Eu vi esse padrão repetido em quase todo projeto Godot que trabalhei ou revisei. Todo mundo escreve sua própria classe `SaveManager` com métodos `save_game()` e `load_game()`. Funciona, mas é repetitivo e propenso a erros. Desenvolvedores novos especialmente sofrem com isso - é uma daquelas áreas de "se vira" onde a engine não fornece muita orientação.

Usar `ResourceSaver` para salvar cenas inteiras é exagero na maioria dos casos - você não quer salvar a estrutura inteira da cena, apenas o estado dinâmico. Mas rastrear manualmente quais propriedades precisam ser salvas é chato e fácil de esquecer quando você adiciona novas features.

## Proposta

A ideia central é simples: deixar os desenvolvedores marcarem propriedades como persistentes, e a engine cuida do resto.

### 1. Sistema de Anotações `@persistent`

A ideia central é simples: marcar propriedades para salvamento automático. O sistema é flexível, suportando tags, grupos e integração nativa com `@export` em qualquer `Object` (Nodes ou Resources).

```gdscript
extends CharacterBody3D

# --- Básico ---
# Salvo automaticamente no snapshot
@persistent var player_name: String = "Hero"

# --- Integração com Editor (@export) ---
# Designers definem o valor inicial no editor. O SaveServer salva as mudanças em runtime.
@export @persistent var max_health: int = 100
@export @persistent var current_health: int = 100

# --- Tags (Saves Filtrados) ---
# Adiciona a tag "inventory". Permite salvar APENAS isso depois.
@persistent("inventory") var items: Array = []

# --- Grupos (Açúcar Sintático) ---
# Maneira limpa de aplicar a tag "progression" em várias variáveis
@persistent_group("progression")
@persistent var level: int = 1
@persistent var experience: int = 0
```

Isso elimina completamente a necessidade de dicionários manuais ou lógica de "ResourceSaver" customizada.

**Casos de Uso Avançados:**
Como funciona em nível de objeto, você pode preservar estados voláteis que geralmente são perdidos, como reprodução de áudio e animação:

```gdscript
extends AudioStreamPlayer

# Preserva a música exatamente onde parou
@persistent var playback_position: float = 0.0
@persistent var is_playing: bool = false
```

Não é necessário nenhuma lógica complexa de serialização; apenas marque o que importa.

### 2. Construindo Sistemas Customizados: `_save_persistence` e `_load_persistence`

Embora `@persistent` cubra casos de uso simples, as funções virtuais `_save_persistence` e `_load_persistence` são projetadas para desenvolvedores que precisam construir arquiteturas de save complexas e customizadas.

É aqui que o `SaveServer` brilha como um **sistema padronizado**: ele cuida do I/O de arquivos, compressão e threading, mas deixa a *lógica* do que constitui o "estado" para o usuário.

```gdscript
func _save_persistence(state: Dictionary) -> void:
    # Adiciona dados customizados ao dicionário de estado
    pass

func _load_persistence(state: Dictionary) -> void:
    # Restaura do dicionário de estado
    pass
```

O `SaveServer` chama esses métodos automaticamente durante save/load. O dicionário `state` já contém todas as suas propriedades `@persistent`.

Essa separação permite que os desenvolvedores criem sistemas de save sofisticados (usando as "gêmeas" para definir a lógica) sem nunca tocar no `FileAccess` ou se preocupar em bloquear a thread principal.

#### Padrão 1: Coleta Manual de Estado (Controle Total)

Esta separação permite que desenvolvedores criem sistemas de save sofisticados (usando as "gêmeas" para definir a lógica) sem nunca tocar no `FileAccess` ou se preocupar em bloquear a thread principal.

Aqui está um exemplo prático - um spawner que precisa coletar dados manualmente de seus filhos:

```gdscript
extends Node2D

var enemy_scene = preload("res://enemy.tscn")

func _save_persistence(state: Dictionary, _tags: Array[StringName]):
    var enemy_data = []
    for enemy in get_children():
        enemy_data.append({
            "pos": enemy.position,
            "hp": enemy.hp,
            "type": enemy.type
        })
    state["spawned_enemies"] = enemy_data

func _load_persistence(state: Dictionary):
    # Limpa os existentes e recria baseado em dados de dicionário manuais
    for child in get_children(): child.queue_free()
    for data in state.get("spawned_enemies", []):
        var e = enemy_scene.instantiate()
        e.position = data.pos
        e.hp = data.hp
        e.type = data.type
        add_child(e)
```

#### Padrão 2: Modelo de ID Reconstrutivo (Assistido pela Engine)

Para cenários que exigem alta estabilidade com objetos complexos, o `SaveServer` fornece um **Modelo de ID Reconstrutivo**. O gerente lida apenas com o spawn e o re-vínculo do `persistence_id`.

```gdscript
# Re-instancia usando os IDs gerenciados pela engine
func _load_persistence(state: Dictionary):
    for child in get_children(): child.queue_free()

    for data in state.get("spawned_enemies", []):
        var e = enemy_scene.instantiate()
        # RE-APLICA o ID para que a engine possa mapear todas as propriedades @persistent
        e.persistence_id = data.id
        add_child(e)
        # O SaveServer agora prossegue para injetar 'e.hp', 'e.ammo', etc., automaticamente
```

### 3. O Resource Snapshot

A classe `Snapshot` é um novo tipo de `Resource` que encapsula o estado salvo do jogo. Serve como formato de container para todos os arquivos de save, seja texto ou binário.

**Propriedades:**

- **`snapshot`** (Dictionary): Os dados reais do estado do jogo coletados de propriedades persistentes e chamadas `_save_persistence()`
- **`version`** (String): A versão do jogo quando este save foi criado (de `application/config/version`)
- **`metadata`** (Dictionary): Dados leves para exibição na UI (tempo de jogo, nome do nível, nome do personagem, etc.)
- **`thumbnail`** (Texture2D): Screenshot opcional tirada no momento do save
- **`checksum`** (String): Hash SHA-256 dos dados do snapshot para validação de integridade

A separação de `metadata` de `snapshot` é crucial para performance. Ao exibir uma lista de arquivos de save na UI, você pode carregar apenas os metadados e thumbnail sem deserializar o estado completo do jogo:

```gdscript
# Menu de load pode mostrar info do save sem carregar o estado completo
var save_info = SaveServer.get_save_metadata("slot_1")
$Label.text = "%s - Level %d - %s" % [
    save_info["character_name"],
    save_info["level"],
    save_info["playtime"]
]
$TextureRect.texture = save_info["thumbnail"]
```

Este design espelha como jogos modernos exibem informações de arquivos de save instantaneamente, sem os atrasos de carregamento que viriam de deserializar gigabytes de estado do jogo só para mostrar uma prévia.

### 4. O Singleton `SaveServer`

Um novo singleton core responsável por orquestrar o processo de save. Age de forma similar ao `AudioServer` ou `PhysicsServer`, delegando o trabalho pesado para um sistema dedicado.

O uso básico seria assim:

```gdscript
# Salva o estado completo do jogo começando do node raiz
await SaveServer.save_snapshot(get_tree().root, "save_slot_1")

# Carrega de volta
SaveServer.load_snapshot(get_tree().root, "save_slot_1")
```

O servidor cuida de:

- **Threading em Background**: Escrita em disco acontece assincronamente.
- **Backups Atômicos**: Mantém automaticamente um arquivo `.bak`. Se o arquivo de save principal for corrompido, ele restaura o backup.
- **Dupla Camada de Segurança**:
  - **Validação de Integridade**: Computa checksums SHA-256 para validar a integridade dos dados antes da injeção.
  - **Formato Binário Seguro**: Suporta criptografia AES para arquivos `.data` usando uma chave de projeto.
- **Versioning**: Rastreia versões do jogo para permitir migração de dados.

## Detalhes Técnicos

### Flag de Uso de Propriedade

Esta implementação introduz uma nova flag `PROPERTY_USAGE_PERSISTENCE` (bit 30) no sistema de propriedades da classe `Object`. A anotação `@persistent` define esta flag em variáveis de script durante a compilação.

Quando o compilador GDScript encontra `@persistent`, ele:

1. Define a flag `PROPERTY_USAGE_PERSISTENCE` no `PropertyInfo` da propriedade
2. Opcionalmente armazena tags no `hint_string` da propriedade (para salvamento filtrado)
3. Valida que a propriedade tem anotação de tipo explícita (necessário para serialização confiável)

O sistema de iteração de propriedades da engine pode então identificar eficientemente quais propriedades precisam ser salvas verificando esta flag, sem requerer qualquer construção de dicionários em runtime ou listas manuais de propriedades.

### Configurações de Projeto

O sistema adicionaria uma nova seção `persistence/` nas Configurações de Projeto com as seguintes opções:

**`application/persistence/save_format`** (enum: Text, Binary)

- **Text**: Salva como arquivos `.tres` (legível por humanos, amigável para controle de versão, bom para desenvolvimento)
- **Binary**: Salva como arquivos `.data` (suporta encriptação e compressão, bom para produção)
- Padrão: Text

**`application/persistence/save_path`** (String)

- Diretório base para arquivos de save
- Padrão: `"user://saves/"`

**`application/persistence/encryption_key`** (String)

- Chave de encriptação AES para saves binários (32 caracteres)
- Auto-gerada se vazia (apenas no editor)
- Aplica-se apenas ao formato binário

**`application/persistence/compression_enabled`** (bool)

- Habilita compressão ZSTD para saves binários
- Reduz significativamente o tamanho do arquivo com custo mínimo de CPU
- Padrão: true

**`application/persistence/backup_enabled`** (bool)

- Mantém automaticamente cópias shadow `.bak`
- Restaura do backup se o save principal estiver corrompido
- Padrão: true

**`application/persistence/integrity_check_level`** (enum: None, Signature, Strict)

- **None**: Sem validação (mais rápido)
- **Signature**: Validação de checksum SHA-256
- **Strict**: Checksum + verificação de compatibilidade de versão
- Padrão: Signature

Essas configurações fornecem segurança e confiabilidade prontas para produção, enquanto permanecem configuráveis para diferentes necessidades de projeto.

### Suporte a Migração

Jogos evoluem, e arquivos de save quebram. O `SaveServer` permite registrar callbacks de migração para atualizar dados de save antigos:

```gdscript
SaveServer.register_migration("1.0", "1.1", func(data):
    # Migra estrutura de dados
    data["new_stat"] = 10
)
```

## Por Que Isso Importa

Eu acho que isso seria uma melhoria enorme de qualidade de vida para a Godot. Sistemas de save são uma daquelas coisas que todo jogo não-trivial precisa, mas a engine não ajuda muito. Você ou gasta tempo construindo sua própria infraestrutura ou lança com um sistema frágil baseado em JSON e torce para dar certo.

Ter isso built-in colocaria a Godot à frente da Unity (que não tem nada assim) e no mesmo nível do sistema de save game da Unreal, mas mais simples de usar. Mais importante, permitiria que desenvolvedores - especialmente os mais novos - foquem em fazer seu jogo ao invés de debugar por que o arquivo de save corrompeu.

A implementação no nível de Object torna flexível o suficiente para qualquer caso de uso, de plataformers simples a RPGs complexos com milhares de entidades persistentes. E porque é declarativo com `@persistent`, é difícil errar - você só marca o que quer salvar e a engine faz o resto.
