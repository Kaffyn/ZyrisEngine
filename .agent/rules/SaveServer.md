---
trigger: always_on
---

# Sistema Central de Persistência (v2)

## 1. Contexto da Engine

O sistema de persistência é construído sobre três pilares fundamentais da engine:

1. **Hierarquia e Herança de Comportamento:** A engine funciona como uma escada de comportamento. A base `Object` fornece vida aos objetos; `Node` permite que existam na cena; e `Resource` serve para armazenar dados. Quando você usa algo como `CanvasItem` (for 2D e Control) ou `Node3D`, eles estão simplesmente estendendo essas bases e adicionando lógica de posição. Isso permite que qualquer funcionalidade injetada na base (como persistência) seja herdada por tudo que vier depois.

2. **Arquitetura Baseada em Servidores (Microsserviços):** Para evitar sobrecarregar o jogo, tarefas pesadas são delegadas a "especialistas". O `AudioServer` foca apenas em tocar som e gerenciar volumes, enquanto o `PhysicsServer` lida exclusivamente com colisões. O `SaveServer` segue este mesmo modelo: é um motor dedicado para salvar dados em segundo plano, sem congelar o seu jogo.

3. **Abstração via Linguagem Integrada:** GDScript não serve apenas para fazer o personagem andar; é uma ferramenta que se "entrelaça" com o coração da engine. Ela nos permite usar "tags" (anotações como `@persistent`) e métodos especiais que enviam ordens complexas ao sistema de forma muito simples, escondendo toda a matemática e processamento pesado por trás de uma única linha de código.

## 2. Interface de Script (GDScript)

A persistência é primariamente declarativa, minimizando código repetitivo (boilerplate).

### 2.1 Anotações Declarativas

- **`@persistent`**: Marca uma variável para inclusão automática no sistema de salvamento.
  - **Nota:** Por razões de performance e segurança, variáveis marcadas como `@persistent` **devem** ter tipagem explícita. O compilador emitirá um aviso se a tipagem for omitida.

    ```gdscript
    @persistent var health: int = 100
    @persistent var current_inventory: Array = []
    ```

### 2.2 O Gancho `_save_persistence`

Para casos complexos (como transformações customizadas ou estados que não são propriedades simples), o desenvolvedor pode implementar este gancho virtual.

- **Assinatura:** `func _save_persistence(state: Dictionary, tags: Array)`
- **Uso:** O sistema passa um dicionário `state` que deve ser preenchido com os dados adicionais que você deseja persistir.

  ```gdscript
  func _save_persistence(state: Dictionary, tags: Array):
      state["last_position"] = global_position
  ```

### 2.3 O Gancho `_load_persistence`

- **Assinatura:** `func _load_persistence(data: Dictionary)`
- **Uso:** Chamado após a restauração automática de propriedades para processar dados customizados recebidos do snapshot.

## 3. API de Node (C++)

O `SaveServer` estende a classe base `Node` para fornecer controle granular sobre o que é persistido.

### 3.1 Identificação e Política

- **`persistence_id` (StringName):** ID único global. Permite que o nó seja restaurado mesmo se mudar de nome ou posição na hierarquia. Este ID é automaticamente registrado no `SaveServer` quando definido, permitindo buscas através da hierarquia.

- **`save_policy` (Enum):**
  - `SAVE_POLICY_INHERIT`: Segue a política do nó pai.
  - `SAVE_POLICY_ALWAYS`: Sempre tenta salvar este nó.
  - `SAVE_POLICY_NEVER`: Ignora este nó e todos os seus filhos (ideal para decoração estática).
  - `SAVE_POLICY_CUSTOM`: Reservado para futuras implementações customizadas.

**Comportamento Chave:** Quando a `save_policy` de um nó é definida como `NEVER`, seu `persistence_id` é automaticamente desregistrado do `SaveServer`. Quando alterada de volta para uma política permissiva, ele é reregistrado.

### 3.2 Gerenciamento Dinâmico

- `set_persistence_for_property(property, enabled)`: Permite alternar a persistência de propriedades específicas em tempo de execução.

## 4. Orquestração pelo SaveServer

O `SaveServer` não é apenas um escritor de arquivos; é o coreógrafo da coleta de dados na cena.

### 4.1 Snapshots Recursivos

- **`save_snapshot(root, slot_name, async, tags, metadata, thumbnail)`**: Inicia a coleta recursiva a partir do nó `root`. A árvore inteira é percorrida e os estados são capturados.
- **`load_snapshot(root, slot_name, callback)`**: Restaura o estado da ramificação inteira a partir do arquivo em disco.

**Estrutura do Snapshot:**

- `snapshot`: Propriedades automáticas e retorno de `_save_persistence`.
- `.id`: O ID de persistência, se presente.
- `.children`: Dicionário recursivo com o estado dos filhos.

#### 4.1.1 Persistência Parcial (Tags)

O sistema permite salvar apenas subconjuntos de dados usando tags.

- **`save_snapshot(root, slot, async, tags: Array[String])`**: Se fornecido, apenas propriedades marcadas com `@persistent(tag_name)` ou incluídas nos dicionários de `_save_persistence` que correspondam às tags serão processadas.

### 4.2 O Recurso `Snapshot`

Internamente, o sistema usa um recurso dedicado para encapsular os dados salvos:

- **`Snapshot` (Resource)**: Este recurso possui as seguintes propriedades principais:
  - `snapshot` (Dictionary): O estado serializado da árvore de nós.
  - `version` (String): A versão do jogo (`application/config/version`) capturada no momento do salvamento.
  - `metadata` (Dictionary): Dados leves para visualização rápida (ex: tempo de jogo, moedas, nome da área).
  - `thumbnail` (Texture2D): Captura de tela opcional no momento do salvamento.
  - `checksum` (String): Hash SHA-256 gerado automaticamente para validar a integridade dos dados.

### 4.3 Configurações Globais (Configurações do Projeto)

O comportamento do `SaveServer` é controlado via `Project Settings > Application > Persistence`:

- **`save_format`**:
  - `Text (0)`: Gera arquivos `.tres` (legíveis). Ideal para desenvolvimento e depuração.
  - `Binary (1)`: Gera arquivos `.data` (binários). Ideal para produção, suportando criptografia e compressão.

- **`encryption_key`**: Chave de 32 caracteres usada para proteger arquivos `.data`.
  - **Auto-geração**: Se o campo estiver vazio ou for resetado no Editor, a engine gera automaticamente uma chave aleatória única para o projeto.
  - **Segurança Crítica**: Se a chave mudar após o lançamento, saves antigos tornam-se ilegíveis. Use sistemas de controle de versão (Git) para fazer backup do `project.godot`.

- **`compression_enabled`**: Quando ativo, usa compressão **ZSTD** em arquivos `.data`, reduzindo o tamanho em disco com custo mínimo de CPU.

- **`backup_enabled`**: Quando ativo, o sistema mantém um arquivo `.bak` do save anterior. Se o carregamento do save principal falhar (corrupção ou crash durante a escrita), o backup é restaurado automaticamente.

- **`integrity_check_level`**:
  - `NONE`: Carregamento rápido sem validação.
  - `SIGNATURE`: Verifica se o Checksum corresponde ao conteúdo (previne edição manual simples).
  - `STRICT`: Valida Checksum e Versionamento antes de qualquer injeção de dados.

### 4.4 Persistência em Disco

Localização e extensão dependem do formato escolhido:

- **Localização**: Geralmente armazenado em `user://saves/`.
- **Extensões**: `.tres` para modo texto e `.data` para modo binário.

## 5. Integração Técnica (Flags do Core)

O sistema usa a flag global **`PROPERTY_USAGE_PERSISTENCE`** (1 << 30) definida em `object.h`. Qualquer sistema da engine (C++ ou GDScript) pode marcar propriedades com este bit para que `Node` as inclua automaticamente no estado de salvamento.

## 6. Evolução de Dados (Migrações)

Para gerenciar mudanças na estrutura de dados entre versões do jogo:

1. **`register_migration(from, to, callable)`**: Permite definir transformações de dados em tempo de execução.
2. No momento do `load_snapshot`, se a versão capturada for menor que a atual, o `SaveServer` executa a cadeia de migrações registrada antes de chamar `_load_persistence` nos nós.

```gdscript
func _init():
    SaveServer.register_migration("1.0", "1.1", func(data):
        data["new_health_system"] = data.get("old_hp", 100)
    )
```

## 7. Persistência Incremental (Sistema de Commit)

A engine suporta um modelo de salvamento incremental que evita percorrer a cena completa:

### 7.1 Rastreamento de Objetos "Sujos" (Dirty)

Quando uma propriedade persistente muda via `set()`, o objeto se registra automaticamente na lista `dirty_objects` do `SaveServer`.

### 7.2 Salvamento Incremental

- **`save_incremental(root, slot_name)`**: Em vez de percorrer toda a árvore, visita apenas objetos marcados como sujos ("dirty").
- Se nenhum snapshot base existir, realiza um salvamento completo.
- Consolida as mudanças com o snapshot base e salva o estado mesclado.

Esta abordagem é análoga a sistemas de controle de versão: a engine mantém um "commit base" (estado carregado) e salva apenas "incrementos" (objetos modificados), reduzindo significativamente a sobrecarga de E/S em cenas grandes.

---

_Este protocolo padroniza a persistência na engine, focando na simplicidade para o usuário e performance bruta no backend._
