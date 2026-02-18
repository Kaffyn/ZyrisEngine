# Zyris Engine

[![Godot Version](https://img.shields.io/badge/Godot-4.6--stable-blue.svg)](https://github.com/godotengine/godot)
[![Zyris Version](https://img.shields.io/badge/4.6.0-zyris.3-orange.svg)](https://github.com/Kaffyn/ZyrisEngine/tree/Zyris)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE.txt)

> [!TIP]
> **Leia isto em outros idiomas / Read this in other languages:**
> [**English**](README.en.md) | [**Português**](README.md)

> [!NOTE]
> **Por que o README principal está em Português?**
> O Zyris é uma iniciativa do **MachiTwo** e brasileiros estão contribuindo diretamente com ele. Por isso, decidimos manter o README principal em português enquanto o projeto for pequeno. Assim que o Zyris crescer e atrair mais colaboradores internacionais, passaremos a utilizar o Inglês como idioma principal.

**Zyris** é uma poderosa distribuição de game engine baseada na [Godot Engine](https://godotengine.org), desenvolvida por **MachiTwo** e contribuidores. Nossa missão é expandir os limites do desenvolvimento de jogos de código aberto, implementando recursos avançados e otimizações que rivalizam com engines líderes da indústria como Unreal Engine e Unity.

## Visão

O Zyris não é um fork destinado a contribuições upstream para a Godot Engine. Em vez disso, somos uma **distribuição paralela** que:

- **Estende as capacidades da Godot** com recursos de ponta
- **Otimiza a performance** para o desenvolvimento de jogos de alto nível
- **Implementa inovações proprietárias** mantendo os princípios de código aberto
- **Foca em fluxos de trabalho e ferramentas de produção AAA**

Todo o crédito para a engine base vai para o [repositório official da Godot Engine](https://github.com/godotengine/godot) e sua incrível comunidade de contribuidores.

## Versionamento e Política de Sincronização

O Zyris realiza rebase manual apenas em versões maiores da Godot (ex: `4.5 → 4.6`).

Atualizações menores não são acompanhadas.

### Formato de Versão

**Formato de Versão Zyris** - `Godot-X.Y.Z-zyris.N`

- **X.Y.Z** - Versão official da Godot Engine (ex: `4.6.0`)

- **N** - Incremento de subversão do Zyris (ex: `zyris.1`, `zyris.2`)

**Versão Atual** - `4.6.0-zyris.3`

## Roadmap

O Zyris está implementando um conjunto abrangente de sistemas. Abaixo está nosso roadmap de desenvolvimento:

### Implementado

- [x] **Virtual Input Devices** - Camada de abstração de entrada multi-plataforma

    Um sistema completo de entrada virtual integrado diretamente ao core da engine, fornecendo controles de toque fluidos para dispositivos móveis e híbridos.

    **Nós de Interface:**

- `VirtualButton` - Botão de toque configurável com feedback visual e mapeamento de ações
- `VirtualJoystick` - Controle de analógico com zonas mortas e sensibilidade customizáveis
- `VirtualDPad` - Pad directional com suporte a 4 e 8 direções
- `VirtualTouchPad` - Área de gestos multi-toque para controle de câmera e viewport

    **Recursos Principais:**

- **Integração de Feedback Háptico** - Suporte a vibração para dispositivos móveis e gamepads
- **Rastreamento de Dispositivo de Entrada** - API `LastInputType` detecta e rastreia automaticamente o método de entrada ativo (Toque, Teclado/Mouse, Gamepad)
- **Adaptação de UI Dinâmica** - Controles virtuais aparecem/ocultam automaticamente com base no dispositivo detectado
- **Modos de Visibilidade** - `ALWAYS`, `TOUCHSCREEN_ONLY`, `WHEN_PAUSED` para comportamento flexível de UI
- **Mapeamento de Ações** - Integração direta com o sistema InputMap da Godot
- **Sistema de Entrada Polimórfico** - A classe base `InputEvent` agora usa métodos virtuais para correspondência de ações e comparação de eventos, permitindo que eventos virtuais customizados se integrem perfeitamente ao core da engine como cidadãos de primeira classe.
- **Integração com o Editor** - Suporte total no inspetor com configuração visual
- **Otimizado para Performance** - Overhead mínimo com manipulação eficiente de eventos

- [x] **Save Server** - Orquestrador de Persistência e Serialização

     Um sistema de persistência robusto e de alta performance integrado ao core da engine, permitindo salvar e carregar estados complexos de forma assíncrona e segura.

     **Recursos Principais:**

- **Protocolo Declarativo (@persistent)** - Automação total via anotações no GDScript e flags no C++. Suporta tipagem forte e organização hierárquica por `@persistent_group`.
- **Threaded Architecture** - Operações de I/O, compressão ZSTD e criptografia AES-256 rodam em threads dedicadas, garantindo autosaves fluidos sem travamentos.
- **Arquitetura Híbrida (Manifesto/Satélites)** - Suporte a persistência parcial através de tags, permitindo salvar subconjuntos de dados (ex: inventário, mundo) em arquivos satélites modulares.
- **Dynamic Spawning & Cleanup** - Recriação automática de nós instanciados em tempo de execução e remoção de órfãos (inimigos mortos) para garantir paridade total entre a cena e o snapshot.
- **Sistema Incremental (Amend Save)** - Inspirado no Git, rastreia objetos modificados e aplica "patches" cirúrgicos no snapshot, reduzindo a escrita em disco em até 95%.
- **Global ID Registry (persistence_id)** - Abstração de hierarquia que desvincula os dados de NodePaths, impedindo que saves quebrem se nós forem renomeados ou movidos.
- **Evolução de Dados (Migrações)** - Sistema de versionamento que permite registrar transformações de dados (`register_migration`) para manter compatibilidade com saves antigos.
- **Segurança e Integridade** - Sistema de backups rotativos, validação por checksum SHA-256 e mecanismo de *Flush* no encerramento para evitar corrupção de dados.

- [x] **Gameplay Ability System (GAS)** - Framework de Gameplay Orientado a Dados

    Uma implementação nativa de alto desempenho do padrão GAS, projetada para escalar de mecânicas simples a sistemas de combate RPG complexos.

    **Arquitetura:**
  - `AbilitySystemComponent`: O processador central anexado a entidades (Player, Inimigos).
  - `AttributeSet`: Define estatísticas vitais (HP, Mana, Stamina) com replicação e cálculos de modificadores.
  - `GameplayAbility`: Lógica modular de habilidades (Pular, Atirar, Magia) com **Costs & Cooldowns**.
  - `GameplayEffect`: Regras de alteração de atributos (Dano, Cura, Buffs/Debuffs).
  - `GameplayCue`: Sistema de feedback visual/sonoro (VFX, SFX, screen shake).
  - **Gameplay Tasks**: Ações assíncronas para habilidades complexas (ex: aguardar input, projéteis).
  - **Target Data**: Pipeline completo de seleção de alvos e validação do servidor.
  - **MMC/GEE (Magnitude Calculation)**: Cálculos matemáticos dinâmicos para atributos e efeitos.
  - **Attribute Events**: Signals para mudanças de atributos (ideal para UI e triggers).

    **Integração com Editor (GAS Studio):**
    O **GAS Studio** é um editor modal completo integrado ao Inspector:
  - **Janela Flutuante de Grafo** - Edição visual rápida sem perder o contexto da cena.
  - **Tag Management** - Seletor visual hierárquico de `GameplayTags`.
  - **Live Debug** - Visualização em tempo real do fluxo de efeitos e atributos.

    **Diferenciais:**
  - **Multiplayer Nativo** - Predição do cliente e reconciliação do servidor (Netcode robusto).
  - **Data-Driven** - Designers podem criar variações de habilidades inteiras apenas alterando configurações no Editor.

### Em Desenvolvimento

- [ ] **Behavior Tree** - IA Modular e Reativa

    Uma implementação robusta de IA baseada em nós, focada em criar comportamentos complexos através de lógica visual simples e reutilizável.

    **Arquitetura:**
  - `BTAgent`: O orquestrador anexado ao NPC, gerenciando a memória (`Blackboard`) e o tick da árvore.
  - **Composites** - Nós de fluxo (`Sequence`, `Selector`) que definem a tomada de decisão.
  - **Decorators** - Lógica condicional e modificadores (`Cooldown`, `Loop`, `Blackboard Check`) que envolvem outros nós.
  - **Leafs** - As unidades de trabalho reais (`MoveTo`, `Wait`, `ActivateAbility`).

    **Integração com Editor (EditorPlugin):**
    Behavior Trees utilizam um **Editor Visual Dedicado** que opera em modo exclusivo.
  - **Visual Board** - Canvas infinito para organizar hierarquias de IA complexas com clareza visual, suportando drag-and-drop de nós.
  - **Live Debugging** - Rastreamento visual do fluxo de execução em tempo real (os nós acendem enquanto ativos).
  - **Sub-Trees** - Capacidade de criar nós que encapsulam árvores inteiras, permitindo reutilização massiva (ex: uma árvore "Combat" usada dentro de várias IAs diferentes).

    **Diferenciais:**
  - **Nativo & GAS** - Projetado para disparar `GameplayAbilities` diretamente via nós de ação.
  - **Performance** - Travessia de árvore em C++ puro, eliminando gargalos de script em IAs densas.

- [ ] **Inventory System** - Gestão de Items e Transações
  - **InventoryServer** - Singleton autoritativo que valida todas as movimentações de items, prevenindo estados inconsistentes ou trapaças.
  - **Equipment Bridge (GAS)** - Ao equipar um item, o sistema injeta/remove automaticamente *Gameplay Abilities* e *Effects* no componente do portador, sem scripts manuais.
  - **Smart UI Nodes** - Conjunto de nós `Control` (`InventoryGrid`, `EquipmentSlot`) que gerenciam drag-and-drop e sincronização automática com o servidor.
  - **Loot Tables** - Geração procedural integrada ao sistema de sorte e tags do GAS.

- [ ] **Camera System (vCam)** - Arbitragem Cinematográfica
  - **Virtual Camera Resources** - Perfis de câmera definidos como Resources, contendo FOV, Follow Target, LookAt e restrições.
  - **Priority Arbiter** - O `CameraServer` avalia em tempo real qual vCam possui a maior prioridade para assumir o controle da viewport principal com blends configuráveis.
  - **Procedural Shake** - Sistema de trauma baseado em ruído de Perlin, onde os perfis de tremor são Resources editáveis e acumulativos.
  - **Integração com Viewport** - Utiliza **Gizmos 3D Customizados** para desenhar e ajustar visualmente zonas de transição, caminhos de tracking e volumes de influência diretamente na cena.

- [ ] **Melhorias no Audio System**

    O Zyris expande e reformula seu sistema de áudio nativo, atuando diretamente sobre os componentes existentes (`AudioServer`, `AudioStream`, `AudioStreamPlayer`, `AudioBus`, `AudioEffect`), sem introduzir servidores paralelos ou nomenclaturas externas.

    **Escopo das Melhorias:**
  - **Reformulação dos AudioStreamPlayers e Listeners**
    - Players passam a suportar lógica sonora de alto nível.
    - Integração direta com eventos, estados e parâmetros de gameplay.
    - Comportamento mais expressivo sem abandonar o modelo da Godot.

  - **Expansão dos Audio Resources existentes**
    - `AudioStream` com metadados e parâmetros dinâmicos.
    - Suporte a variações, camadas e resolução contextual.

  - **Introdução de novos Audio Resources**
    - Novos tipos seguem rigorosamente a herança padrão da Godot e nomenclatura existente.
    - Integração direta com o AudioServer.

  - **Mixagem Avançada e DSP**
    - DSP por stream, bus ou global (Equalização, filtros, compressão e efeitos temporais).
    - Controle dinâmico via código ou eventos.

  - **Eventos Sonoros**
    - Sons deixam de ser disparados apenas por arquivos; execução baseada em intenção e contexto.
    - Resolução dinâmica do áudio em runtime.

  - **Integração com Gameplay**
    - Integração direta com GAS, Behavior Trees, LSS e Save Server.
    - Estados do jogo influenciam mixagem, execução e transições sonoras.

    Essas melhorias elevam o áudio do Zyris a um nível moderno, reativo e escalável, mantendo compatibilidade conceitual com a engine base.

- [ ] **AOT Export System** - Arquitetura baseada em SDK

    O AOT Export System é um dos pilares centrais da visão do Zyris, projetado para entregar execução nativa de alto desempenho sem alterar o fluxo de desenvolvimento da Godot.

    O AOT não substitui a VM durante o desenvolvimento. Ele atua exclusivamente no momento da exportação, removendo a VM apenas do jogo final buildado e gerando binários nativos direcionados ao hardware de destino.

    **Benefícios Chave:**
  - **Performance Nativa** - Ao remover a Virtual Machine (VM) do executável final, o jogo roda diretamente no hardware, eliminando o overhead de interpretação de scripts.
  - **Segurança (Anti-Hack)** - A compilação para código de máquina nativo torna a engenharia reversa exponencialmente mais difícil se comparada à decompilação de scripts ou bytecode, protegendo a propriedade intelectual.
  - **Export-Only** - O fluxo de desenvolvimento permanece ágil com Hot-Reloading, enquanto o AOT atua apenas na pipeline de build final.

    Para garantir sustentabilidade, escalabilidade e facilidade de contribuição, o AOT é concebido como um SDK externo, integrado ao pipeline de exportação da engine.

    **Diretrizes do Modelo:**
  - Execução apenas na exportação (não participa do editor ou debug)
  - Editor permanece leve e desacoplado do custo do compilador
  - Contribuição facilitada via repositório dedicado
  - Arquitetura alinhada ao modelo de SDKs profissionais (Android SDK, NDK)

    O SDK utiliza Python como camada de orquestração, responsável por coordenar pipelines complexos de compilação, transformação de IR e integração com toolchains nativas.

## Licença

A Zyris Engine é licenciada sob a **Licença MIT**, mantendo compatibilidade com o licenciamento da Godot Engine.

Copyright (c) 2025-present Zyris contributors
Copyright (c) 2014-present Godot Engine contributors
Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur

Veja LICENSE.txt e COPYRIGHT.txt para detalhes completos.

## Agradecimentos

O Zyris é construído sobre o trabalho incrível da comunidade da **Godot Engine**. Somos profundamente gratos a:

- **Juan Linietsky** e **Ariel Manzur** - Fundadores da Godot Engine
- Todos os **contribuidores da Godot Engine** - Por criarem uma base fantástica
- A **comunidade open-source** - Por tornar projetos como este possíveis

Para a lista completa de contribuidores da Godot, veja [AUTHORS.md](AUTHORS.md) e [DONORS.md](DONORS.md).

## Links

- **Godot Engine Official**: <https://godotengine.org>
- **GitHub da Godot**: <https://github.com/godotengine/godot>
- **Repositório do Zyris**: <https://github.com/Kaffyn/ZyrisEngine>
- **Rastreador de Issues**: <https://github.com/Kaffyn/ZyrisEngine/issues>

---

**Zyris Engine** - Expandindo os limites do desenvolvimento de jogos de código aberto
