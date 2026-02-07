---
trigger: always_on
---

# Sistema de Persistência Core (Zyris v2)

## 1. Contexto da Engine

Para entender o `SaveServer`, é preciso primeiro compreender como a engine funciona de forma "invisível" através de três pilares:

1. **Hierarquia e Herança de Comportamento:** A engine funciona como uma escada de comportamentos. A base fundamental, `Object`, dá vida aos objetos; o `Node` permite que eles existam na cena; e o `Resource` serve para guardar dados. Quando você usa algo como `CanvasItem` (para 2D e Control) ou `Node3D`, eles estão apenas estendendo essas bases e adicionando lógica de posição. Isso permite que qualquer funcionalidade injetada na base (como a persistência) seja herdada por tudo que vem depois.

2. **Arquitetura Baseada em Servers (Micro-serviços):** Para não sobrecarregar o jogo, tarefas pesadas são delegadas a "especialistas". O `AudioServer` foca apenas em tocar som e gerenciar volumes, enquanto o `PhysicsServer` cuida exclusivamente de colisões. O `SaveServer` segue este mesmo modelo: ele é um motor dedicado a salvar dados em segundo plano, sem travar o seu jogo.

3. **Abstração via Linguagem integrada:** O GDScript não é apenas para fazer o personagem andar; ele é uma ferramenta que se "emaranha" com o coração da engine. Ele nos permite usar "tags" (anotações como `@persistent`) e métodos especiais que mandam ordens complexas para o sistema de forma muito simples, escondendo toda a matemática e processamento pesado por trás de uma linha de código.

## 2. Interface de Script (GDScript)

A persistência é primariamente declarativa, minimizando o código repetitivo (_boilerplate_).

### 2.1 Anotações Declarativas

- **`@persistent`**: Marca uma variável para inclusão automática no sistema de save.
  - **Nota:** Por questões de performance e segurança, variáveis marcadas como `@persistent` **devem** ter tipagem explícita. O compilador emitirá um alerta caso a tipagem seja omitida.

    ```gdscript
    @persistent var health: int = 100
    @persistent var current_inventory: Array = []
    ```

### 2.2 O Hook `_save_persistence`

Para casos complexos (como transformações customizadas ou estados que não são simples propriedades), o desenvolvedor pode implementar este hook virtual.

- **Assinatura:** `func _save_persistence(state: Dictionary)`
- **Uso:** O sistema passa um dicionário `state` que deve ser preenchido com os dados adicionais que se deseja persistir.

  ```gdscript
  func _save_persistence(state: Dictionary):
      state["last_position"] = global_position
  ```

### 2.3 O Hook `_load_persistence`

- **Assinatura:** `func _load_persistence(data: Dictionary)`
- **Uso:** Chamado após a restauração automática das propriedades para processar os dados customizados recebidos do snapshot.

## 3. API do Node (C++)

O `SaveServer` estende a classe base `Node` para fornecer controle granular sobre o que é persistido.

### 3.1 Identificação e Política

- **`persistence_id` (StringName):** ID único global. Permite que o nó seja restaurado mesmo se mudar de nome ou posição na hierarquia.
- **`save_policy` (Enum):**
  - `SAVE_POLICY_INHERIT`: Segue a política do nó pai.
  - `SAVE_POLICY_ALWAYS`: Sempre tenta salvar este nó.
  - `SAVE_POLICY_NEVER`: Ignora este nó e todos os seus filhos (ideal para decoração estática).

### 3.2 Gerenciamento Dinâmico

- `set_persistence_for_property(property, enabled)`: Permite ligar/desligar a persistência de propriedades específicas em tempo de execução.

## 4. Orquestração pelo SaveServer

O `SaveServer` não é apenas um gravador de arquivos; ele é o coreógrafo da coleta de dados na cena.

### 4.1 Snapshots Recursivos

- **`save_snapshot(root, slot_name, async)`**: Inicia a coleta recursiva a partir do nó `root`. Toda a árvore é percorrida e os estados são capturados.
- **`load_snapshot(root, slot_name)`**: Restaura o estado de toda a ramificação a partir do arquivo no disco.

- `snapshot`: Propriedades automáticas e retorno de `_save_persistence`.
- `.id`: O ID de persistência, se presente.
- `.children`: Dicionário recursivo com o estado dos filhos.

#### 4.1.1 Persistência Parcial (Tags)

O sistema permite salvar apenas sub-conjuntos de dados utilizando tags.

- **`save_snapshot(root, slot, async, tags: Array[String])`**: Se fornecido, apenas propriedades marcadas com `@persistent(tag_name)` ou incluídas nos dicionários de `_save_persistence` que correspondam às tags serão processadas.

### 4.2 O Recurso `Snapshot`

Internamente, o sistema utiliza um recurso dedicado para encapsular os dados salvos:

- **`Snapshot` (Resource)**: Este recurso possui as seguintes propriedades principais:
  - `snapshot` (Dictionary): O estado serializado da árvore de nós.
  - `version` (String): A versão do jogo (`application/config/version`) capturada no momento do salvamento.
  - `metadata` (Dictionary): Dados leves para visualização rápida (ex: tempo de jogo, moedas, nome da área).
  - `thumbnail` (Texture2D): Captura opcional da tela no momento do save.
  - `checksum` (String): Hash SHA-256 gerado automaticamente para validar a integridade dos dados.

### 4.3 Configurações Globais (Project Settings)

O comportamento do `SaveServer` é controlado via `Project Settings > Zyris > Persistence`:

- **`save_format`**:
  - `Text (0)`: Gera arquivos `.tres` (legíveis). Ideal para desenvolvimento e debugging.
  - `Binary (1)`: Gera arquivos `.data` (binários). Ideal para produção, suportando encriptação e compressão.
- **`encryption_key`**: Chave de 32 caracteres usada para proteger arquivos `.data`.
  - **Auto-geração**: Se o campo estiver vazio ou for resetado no Editor, a Zyris gera automaticamente uma chave única e aleatória para o projeto.
  - **Segurança Crítica**: Se a chave mudar após o lançamento, saves antigos tornam-se ilegíveis. Use sistemas de controle de versão (Git) para fazer backup do `project.godot`.
- **`compression_enabled`**: Quando ativo, utiliza compressão **ZSTD** nos arquivos `.data`, reduzindo o tamanho em disco com custo mínimo de CPU.
- **`backup_enabled`**: Quando ativo, o sistema mantém um arquivo `.bak` do save anterior. Se o carregamento do save principal falhar (corrupção ou crash durante escrita), o backup é restaurado automaticamente.
- **`integrity_check_level`**:
  - `NONE`: Carregamento rápido sem validação.
  - `SIGNATURE`: Verifica se o Checksum bate com o conteúdo (previne edição manual simples).
  - `STRICT`: Valida Checksum e Versioning antes de qualquer injeção de dados.

### 4.4 Persistência em Disco

A localização e extensão dependem do formato escolhido:

- **Localização**: Geralmente armazenados em `user://saves/`.
- **Extensões**: `.tres` para modo texto e `.data` para modo binário.

## 5. Integração Técnica (Flags do Core)

O sistema utiliza a flag global **`PROPERTY_USAGE_PERSISTENCE`** (1 << 30) definida em `object.h`. Qualquer sistema da engine (C++ ou GDScript) pode marcar propriedades com este bit para que o `Node` as inclua automaticamente no estado de salvamento.

## 6. Evolução de Dados (Migrations)

Para gerenciar mudanças na estrutura de dados entre versões do jogo:

1. **`register_migration(from, to, callable)`**: Permite definir transformações de dados em runtime.
2. No momento do `load_snapshot`, se a versão capturada for inferior à atual, o `SaveServer` executa a cadeia de migrações cadastrada antes de chamar o `_load_persistence` dos nós.

```gdscript
func _init():
    SaveServer.register_migration("1.0", "1.1", func(data):
        data["new_health_system"] = data.get("old_hp", 100)
    )
```

---

_Este protocolo padroniza a persistência na Zyris Engine, focando em simplicidade para o usuário e performance bruta no backend._
