# Game Design Document: Zyris Teste

## 1. Project Overview

**Zyris Teste** is a 2D mini-game collection inspired by classic virtual pet games like Pou. The game stars **Zyris**, a sleek blue robot dog, serving as a technical demonstration and a fun experience within the Zyris Engine, focusing on modularity, persistent data, and responsive virtual inputs.

- **Protagonist:** Zyris, the blue robot dog.
- **Theme:** Retro-Modern Mini-games.
- **Target Audience:** Casual players.
- **Core Loop:** Play Mini-games -> Earn Currency -> Unlock Content -> Maintain Pet.

---

## 2. Core Systems

### 2.1 The Hub (Virtual Pet Home)

The central scene where the player interacts with **Zyris**.

- **States:** Hunger, Energy, Fun, Health.
- **Interaction:** Feeding, Petting, and accessing the Mini-games menu.
- **Aesthetics:** Zyris is a high-tech blue robot dog with expressive digital eyes.
- **Customization:** Shop for skins and backgrounds using earned coins.

### 2.2 Persistence (SaveServer Integration)

Utilizing the `SaveServer` core feature:

- **Persistent Data:**
  - `total_coins`: int
  - `unlocked_games`: Array[String]
  - `pet_stats`: Dictionary
  - `high_scores`: Dictionary (per game)
- **Implementation:** Automatic saving via `@persistent` variables and periodic snapshots.

### 2.3 Input System

Designed for cross-platform compatibility:

- **Standard Controls:** Keyboard/Gamepad support.
- **Virtual Controls:** Extensive use of `VirtualButton` and `VirtualJoystick` components to ensure a premium mobile-first feel.

---

## 3. Mini-Games List

### 3.1 Snake

- **Objective:** Eat food to grow longer without hitting walls or your own tail.
- **Mechanic:** Discrete 2D grid movement.
- **Progression:** Speed increases as the snake grows.

### 3.2 Pong

- **Objective:** Defeat an AI opponent by bouncing a ball past their paddle.
- **Mechanic:** Physics-based collisions and paddle movement.
- **Variation:** Power-ups (larger paddle, faster ball).

### 3.3 Flappy

- **Objective:** Navigate through narrow gaps in pipes.
- **Mechanic:** Single "Jump" input using physics impulse.
- **Style:** Parallax scrolling background.

### 3.4 Jump in Sky

- **Objective:** Jump upwards as high as possible on platforms.
- **Mechanic:** Automatic jump on collision with platform; horizontal movement control.
- **Features:** Breaking platforms, moving platforms, and spring boosters.

### 3.5 Platformer

- **Objective:** Reach the end of a level or survive an endless run.
- **Mechanics:** Run, Jump, Collect Coins, Stomp Enemies.
- **Architecture:** Uses standard `CharacterBody2D` with custom Zyris physics tweaks.

---

## 4. UI/UX Aesthetics

Following the **Rich Aesthetics** mandate:

- **Color Palette:** Vibrant, cohesive HSL-based colors.
- **Typography:** Modern sans-serif (e.g., Outfit or Inter).
- **Effects:** Smooth gradients, glassmorphism for menus, and subtle micro-animations (bounce-in buttons).
- **Navigation:** Seamless transitions between the Hub and Mini-games.

---

## 5. Technical Roadmap

1. **Foundation:** Implement the Hub and `SaveServer` logic.
2. **Input:** Configure `VirtualInput` maps for all mini-games.
3. **Mini-game Phase 1:** Snake and Pong (Core logic tests).
4. **Mini-game Phase 2:** Flappy and Jump in Sky (Physics tests).
5. **Mini-game Phase 3:** Platformer (Complex state machine).
6. **Polish:** UI overlays, Sound Engine integration, and visual juice.
