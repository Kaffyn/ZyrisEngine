# Game Design Document (GDD) - Before You Forget

## 1. Visão Geral (Overview)

**Título:** Before You Forget
**Gênero:** Rogue-lite Top-down Semi-procedural
**Plataforma Alvo:** PC (Steam/Day 1), Mobile (PlayStore/Day 1). Futuramente Xbox, PlayStation, Nintendo.
**Motor:** Zyris Engine (Showcase Oficial)
**Status:** Em Desenvolvimento (Pre-Alpha)

## 2. Conceito (Concept)

**Antes Que Você Se Esqueça** é um roguelite onde o jogador perde tudo ao morrer — exceto as memórias acumuladas junto ao seu companion, **Zyris**.

Diferente de jogos tradicionais onde você mantêm moedas ou upgrades físicos, aqui a **progressão é baseada em conhecimento persistente**. O jogo é projetado especificamente para demonstrar o potencial arquitetural do **SaveServer** (memória) e do sistema de **Virtual Input Devices** (controles mobile) da Zyris Engine.

_"Persistência não é recurso. É arquitetura."_

## 3. Personagens (Characters)

### A Protagonista

Uma exploradora em um mundo fragmentado que luta contra a amnésia inevitável causada pelo ciclo de renascimento. A cada morte, ela esquece suas habilidades de combate e perde seus itens físicos.

### Zyris (O Companion)

Um cão androide azul (mascote da engine) que funciona como o "banco de dados" persistente do jogador.

- **Personalidade:** Leal, protetor e tecnicamente onisciente.
- **Função Mecânica:** Ele **nunca esquece**. As memórias da run anterior ficam armazenadas nele.
- **Exemplo de Gameplay:** Se o jogador morre 3 vezes para uma armadilha de espinhos, na próxima run o Zyris latirá e iluminará a armadilha antes do jogador pisar nela.

## 4. Mundo (World)

- **Câmera:** Top-down.
- **Estilo Visual:** Minimalista, Fundo Escuro, Azul Neon (Identidade Zyris), Efeitos Holográficos.
- **Estrutura:**
  - **Arena:** Uma cena principal procedural leve que reseta completamente a cada morte.
  - **HUB:** Uma área segura pequena que aparece após a morte, onde o jogador visualiza as memórias recuperadas e interage com NPCs salvos.

## 5. Objetivos do Projeto (Project Goals)

### Metas Técnicas (Showcase)

1. **SaveServer:** Demonstrar o uso de `@persistent_group("memory")` para salvar apenas dados específicos (conhecimento) enquanto descarta o resto (estado do mundo/player).
2. **Virtual Input:** Provar que a Zyris Engine oferece controles de toque de nível comercial (joystick, botões, haptics) sem plugins externos.
3. **Performance Mobile:** Rodar liso em Android com iluminação 2D e partículas.

### Metas de Negócios & Marketing

- **Conteúdo & Comunidade:**
  - **Devlogs:** Série de vídeos documentando o desenvolvimento para promover a engine.
  - **Making Of:** Gravação de reuniões e bastidores para produção de um **curta sobre o desenvolvimento**.
- **Dogfooding:** O jogo deve ser desenvolvido usando a versão atual da engine (v2). À medida que novos módulos (GAS, LSS) ficarem prontos na engine, o jogo será refatorado para usá-los.
- **Monetização:**
  - **Early Access:** Gratuito (Day 1 na Steam/PlayStore).
  - **Versão Final 1.0:** Pago (Target: $10 USD).

## 6. Mecânicas Core (Core Mechanics)

### Loop Principal

1. **Explorar:** Navegar a arena, combater inimigos.
2. **Aprender:** Encontrar segredos ou morrer para novos perigos.
3. **Morrer (Inevitável):** Perda total de itens, armas e HP.
4. **Persistir:** O SaveServer armazena o grupo "memory" (dados do Zyris).
5. **Recomeçar:** Nova run, mas o Zyris agora tem counters ou flags que desbloqueiam avisos ou habilidades.

### Estrutura Técnica de Persistência

O jogo usa grupos de persistência para separar o que é efêmero do que é eterno.

```gdscript
@persistent_group("memory")
# Salvo entre runs:
# - companion_memory (Dicionário de fatos aprendidos)
# - rescued_npcs (Lista de IDs)
# - bosses_defeated (Flags de progressão)

@persistent_group("run_state")
# NÃO SALVO ao morrer (apenas em save, se houver):
# - player_hp
# - inventory
# - current_weapon
```

## 7. Combate & Inimigos

### Player

- **Movimento:** Livre (Virtual Joystick).
- **Ações:** Ataque Básico, Dash (Cooldown), Interagir.

### Zyris (AI)

- **Comportamento:** Seguir, Atacar (Leve), Proteger.
- **Evolução:** Ganha comportamentos baseados nas memórias (ex: "Shield" se o player morre muito por projéteis).

### Inimigos (MVP)

- **Chaser:** Persegue rápido.
- **Shooter:** Ataque à distância.
- **Exploder:** Kamikaze.
- **Bosses:** Possuem padrões que, uma vez "aprendidos" pelo Zyris, são telegrafados visualmente na próxima luta.

## 8. Roteiro (Roadmap)

### Fase 1: MVP (8 Semanas) - Foco Engine V2

- **Semana 1-2:** Movimentação, Combate Básico, Inputs Virtuais.
- **Semana 3-4:** Inimigos, Loop de Morte e Reset de Cena.
- **Semana 5-6:** Implementação do **SaveServer** ( A "Memória").
- **Semana 7:** Boss Principal.
- **Semana 8:** Polimento e UI Mobile.

### Fase 2: Futuro (V3+)

- Integração de **Behavior Trees** para IAs complexas.
- Migração para **GAS** (Gameplay Ability System).
- Expansão de mundo com **LSS** (Level Streaming).
