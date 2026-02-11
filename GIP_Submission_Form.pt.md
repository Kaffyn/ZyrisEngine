# Add high-level persistence with SaveServer and @persistent annotation

## 1. Descreva o projeto em que você está trabalhando

Estou trabalhando no core da Godot Engine, especificamente com o objetivo de habilitar suporte nativo para RPGs de mundo aberto complexos e jogos de simulação. Esses projetos exigem o gerenciamento de milhares de objetos persistentes (entidades, itens de inventário, estado do mundo) que mudam dinamicamente. Para suportar verdadeiramente essa escala de desenvolvimento, a engine precisa de uma maneira confiável e nativa (built-in) de transmitir esses dados para o disco sem travar a thread principal, mantendo ao mesmo tempo um versionamento robusto e integridade dos dados.

## 2. Descreva o problema ou a limitação que você está enfrentando em seu projeto

Salvar o estado do jogo na Godot atualmente é um processo manual tedioso. Não existe um "jeito Godot" padrão de salvar dados. Os desenvolvedores são forçados a:

1. Construir manualmente dicionários para cada objeto salvável.
2. Implementar seu próprio manuseio de I/O de arquivos (frequentemente bloqueando a thread principal).
3. Reinventar sistemas para encriptação, compressão e backups.
4. Manusear manualmente a migração de versão quando os formatos de save mudam.

Usar `ResourceSaver` para salvar cenas inteiras empacotadas é frequentemente um exagero e inflexível para estado em tempo de execução (runtime), enquanto `FileAccess` é de nível muito baixo. Isso leva a um código frágil e cheio de boilerplate, propenso a bugs e corrupção de dados. Novos desenvolvedores frequentemente lutam com isso, implementando sistemas inseguros que quebram facilmente.

## 3. Descreva a funcionalidade / melhoria e como ela ajuda a superar o problema ou limitação

Eu proponho a adição de um **Sistema de Persistência de Alto Nível** composto pelo singleton `SaveServer`, a anotação `@persistent` e os métodos virtuais de objeto `_save_persistence` / `_load_persistence`.

**Distinção Importante:** O `SaveServer` não é uma funcionalidade rígida de "save game". Ele é uma **camada de infraestrutura**. Ele abstrai o "encanamento" complexo da persistência (I/O em threads, encriptação, escritas atômicas, compressão) para que os desenvolvedores possam construir seus próprios sistemas de save específicos sobre ele.

Enquanto a anotação `@persistent` lida com a serialização simples de propriedades, o sistema evolui a persistência tradicional para um modelo de **"Git para Gameplay"**. Em vez de salvar snapshots completos do mundo (que causam "Save Stutter"), utilizamos **Estados Incrementais**. Isso permite que a engine rastreie apenas objetos modificados ("Dirty Tracking") e salve atualizações fracionárias, reduzindo o I/O de disco em até 95%.

## 4. Descreva como sua proposta funcionará, incluindo código, pseudocódigo, protótipos e/ou diagramas

O núcleo da proposta é o sistema de anotação **`@persistent`**, que funciona em qualquer `Object` (Nodes, Resources, RefCounted).

### A Anotação `@persistent`

```gdscript
extends CharacterBody3D

# --- Uso Básico ---
# Salvo automaticamente no snapshot
@persistent var player_name: String = "Hero"

# --- Integração com Editor (@export) ---
# Designers definem valor inicial no editor. SaveServer salva mudanças em runtime.
@export @persistent var max_health: int = 100
@export @persistent var current_health: int = 100

# --- Tags (Saves Filtrados) ---
# Adiciona a tag "inventory". Permite salvar APENAS este subconjunto depois.
@persistent("inventory") var items: Array = []

# --- Grupos (Açúcar Sintático) ---
# Maneira limpa de aplicar a tag "progression" a múltiplas variáveis
@persistent_group("progression")
@persistent var level: int = 1
@persistent var experience: int = 0
```

### O Singleton `SaveServer`

Um novo singleton lida com o trabalho pesado:

```gdscript
# Salva o estado inteiro do jogo começando do nó raiz
# Async, encriptado, comprimido e atômico (seguro contra crashes)
await SaveServer.save_snapshot(get_tree().root, "save_slot_1")

# Carrega de volta
SaveServer.load_snapshot(get_tree().root, "save_slot_1")

# Salva apenas tags específicas (ex: apenas o inventário)
SaveServer.save_snapshot(get_tree().root, "slot_1", true, ["inventory"])

# --- SAVE INCREMENTAL (O Modelo Delta) ---
# Salva APENAS objetos sujos (modificados desde o load). "Diff" estilo Git.
# Elimina efetivamente o save stutter em jogos massivos.
SaveServer.save_incremental(get_tree().root, "save_slot_1")
```

### O Recurso `Snapshot`

Todo o estado salvo é encapsulado em um novo tipo de Resource chamado `Snapshot`. Ele separa os dados pesados do jogo de metadados leves:

- **`snapshot`** (Dictionary): Dados completos de persistência.
- **`metadata`** (Dictionary): Dados para UI (tempo de jogo, nível, etc.).
- **`thumbnail`** (Texture2D): Screenshot opcional.
- **`checksum`** (String): Hash SHA-256 para integridade.

Isso permite carregar **apenas** metadados e thumbnails para menus de "Load Game" instantâneos, sem precisar deserializar gigabytes de estado do jogo.

### As Funções Virtuais `_save_persistence` e `_load_persistence`

Estas funções são integradas na classe `Object` e servem como ganchos de ciclo de vida nativos, assim como `_ready()`, `_process()` ou `_physics_process()`. Enquanto `_ready()` roda na instanciação e `_process()` roda a cada quadro, as "Gêmeas da Persistência" são executadas pelo sistema exatamente quando um evento de save ou load acontece.

Embora a anotação `@persistent` seja poderosa para propriedades individuais, ela não foi projetada para rastrear grandes coleções de dados dinâmicos, como 50 inimigos spawnados em tempo de execução. Nesses casos, o objeto "Gerenciador" (ex: o Spawner) assume a responsabilidade.

#### Padrão 1: Coleta Manual de Estado (Controle Total)

Neste padrão, o gerente é responsável tanto pela hierarquia quanto pelo loop de dados. Ele NÃO depende da reconciliação automática de IDs, servindo como um fallback manual completo.

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

Para cenários que exigem alta estabilidade com objetos complexos, o `SaveServer` fornece um **Modelo de ID Reconstrutivo**. O gerente lida apenas com o spawn e o re-vínculo do `persistence_id`, deixando que a engine cuide da restauração das propriedades internas do node automaticamente.

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

Além disso, o **Global ID Registry** garante que os nós sejam identificados pelo seu `persistence_id` em vez do `NodePath` hierárquico. Isso resolve o problema clássico de arquivos de save quebrarem quando nós são renomeados ou movidos dentro da árvore durante o desenvolvimento ou execução.

Isso garante que o `SaveServer` continue sendo um **sistema de persistência** flexível: o estado simples é automatizado, mas a lógica complexa do jogo permanece totalmente sob o controle do desenvolvedor.

### Integração com Sistemas Existentes

- **Saves em Texto (`.tres`)**: Usa `ResourceSaver` internamente, então arquivos de save são depuráveis no editor.
- **Saves Binários (`.data`)**: Usa `FileAccess` para encriptação AES e compressão ZSTD, mas mantém a estrutura interna compatível com o sistema de variantes da Godot.

## 5. Se essa melhoria não for usada com frequência, é possível contorná-la com algumas linhas de script?

É possível contornar, mas não com "algumas linhas". Requer escrever um framework de serialização completo, gerenciamento de threads para I/O assíncrono para evitar picos de lag, e tratamento de erros robusto. Quase todo projeto sério acaba escrevendo sua própria versão disso, levando a uma duplicação massiva de esforço da comunidade e frequentemente implementações abaixo do padrão e bugadas.

## 6. Existe algum motivo para que isso seja um recurso principal e não um complemento na biblioteca de ativos?

Sim.

1. **Performance**: Iterar sobre toda a árvore de cena para coletar propriedades persistentes precisa ser rápido. Fazer isso em C++ com acesso direto aos internos do objeto (flag `PROPERTY_USAGE_PERSISTENCE`) é significativamente mais rápido que um addon GDScript iterando via reflection.
2. **Confiabilidade**: Sistemas de save são infraestrutura crítica. Eles requerem integração profunda com `ResourceLoader`/`Saver` para lidar corretamente com sub-recursos, dependências e escritas atômicas.
3. **Padronização**: Isso diminui significativamente a barreira de entrada para iniciantes. Assim como a Godot fornece `HighLevelMultiplayer`, fornecer `HighLevelPersistence` torna a engine muito mais completa para produção real de jogos.
