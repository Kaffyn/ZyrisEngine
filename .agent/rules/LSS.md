# Implementação do Level Streaming System - LSS (Zyris v3)

Este documento descreve a arquitetura do **Level Streaming System (LSS)**, o ponto central de orquestração da Zyris Engine, definido como a **v3** do projeto.

## 1. Visão Geral

O LSS não é apenas um sistema de carregamento; ele é o **Nexus** do jogo. Ele foi projetado para ser a cena principal (`Main Scene`) de qualquer projeto Zyris, atuando como o Super Node que gerencia o ciclo de vida global, estados de jogo e a persistência espacial.

## 2. Componentes Principais

### `LSSNode` (O Super Node)

- **Papel:** Deve ser usado como o nó raiz da cena principal do jogo.
- **Responsabilidade:** Orquestra a transição entre estados globais e gerencia os `LSSRoot` (Universos) ativos.
- **Funcionalidade:** Atua como o ponto de entrada único, garantindo que sistemas críticos (como UI global e Sound Engine) persistam entre trocas de mundo.

### `LSSServer` (O Singleton)

- **Papel:** Servidor autoritativo acessível via API.
- **GSM (Game State Machine):** Gerencia estados determinísticos:
  - `BOOT`: Inicialização de sistemas core.
  - `MENU`: Interface principal e gerenciamento de perfis.
  - `LOADING`: Tela de transição com suporte a background loading.
  - `GAMEPLAY`: O estado ativo de simulação.
  - `TRANSITION`: Troca controlada entre zonas ou níveis.

### `LSSRoot` (World/Universe Container)

- Permite a coexistência de múltiplos "universos" ou mundos isolados.
- Facilita transições de mapa complexas sem interromper a lógica global do `LSSNode`.

### `StreamingZone`

- Sistema de volumes espaciais (3D/2D).
- Gerencia o carregamento e descarregamento assíncrono de pedaços (`chunks`) do mundo baseado na proximidade do jogador ou gatilhos lógicos.

## 3. Integração com Save Server (v2)

O LSS depende intrinsecamente do Save Server para o processo de **State Hydration**:

1. **Snapshot:** Antes de descarregar uma `StreamingZone`, o LSS solicita ao Save Server que capture o estado atual dos objetos nela contidos.
2. **Hydration:** Ao carregar uma zona, os dados salvos são injetados automaticamente nos novos nós antes de serem adicionados à `SceneTree`.

## 4. Diferenciais Técnicos

- **Performance Nativa:** Travessia de árvore e gerenciamento de memória implementados em C++ para evitar gargalos em mundos densos.
- **Determinismo:** O ciclo de vida dos estados é rígido, prevenindo condições de corrida durante o carregamento.
- **Hierarquia Limpa:** Resolve o problema de gerenciamento de cenas globais do Godot vanilla através de uma estrutura centralizada e autoritativa.
