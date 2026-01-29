# Zyris Engine

[![Godot Version](https://img.shields.io/badge/Godot-4.6.0--stable-blue.svg)](https://github.com/godotengine/godot)
[![Zyris Version](https://img.shields.io/badge/4.6.0-zyris.1-orange.svg)](https://github.com/Kaffyn/ZyrisEngine/tree/Zyris)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE.txt)

> [!TIP]
> **Read this in other languages / Leia isto em outros idiomas:**
> [**English**](README.en.md) | [**Português**](README.md)

> [!NOTE]
> **Why is the main README in Portuguese?**
> Zyris is an initiative by **MachiTwo** and Brazilians are directly contributing to it. Therefore, we decided to keep the main README in Portuguese while the project is small. As Zyris grows and attracts more international contributors, we will switch to using English as the main language.

**Zyris** is a powerful game engine distribution based on the [Godot Engine](https://godotengine.org), developed by **MachiTwo** and contributors. Our mission is to expand the boundaries of open-source game development by implementing advanced features and optimizations that rival industry-leading engines like Unreal Engine and Unity.

## Vision

Zyris is not a fork intended for upstream contributions to the Godot Engine. Instead, we are a **parallel distribution** that:

- **Extends Godot's capabilities** with cutting-edge features
- **Optimizes performance** for high-level game development
- **Implements proprietary innovations** while maintaining open-source principles
- **Focuses on AAA production workflows and tools**

All credit for the base engine goes to the [official Godot Engine repository](https://github.com/godotengine/godot) and its incredible community of contributors.

## Versioning and Synchronization Policy

Zyris performs manual rebases only on major Godot versions (e.g., `4.5 → 4.6`).

Minor updates are not tracked.

### Version Format

**Zyris Version Format** - `Godot-X.Y.Z-zyris.N`

- **X.Y.Z** - Official Godot Engine version (e.g., `4.6.0`)

- **N** - Zyris subversion increment (e.g., `zyris.1`, `zyris.2`)

**Current Version** - `4.6.0-zyris.1`

## Roadmap

Zyris is implementing a comprehensive set of systems. Below is our development roadmap:

### Implemented

- [x] **Virtual Input Devices** - Cross-platform Input Abstraction Layer

    A complete virtual input system integrated directly into the engine core, providing fluid touch controls for mobile and hybrid devices.

    **Interface Nodes:**
  - `VirtualButton` - Configurable touch button with visual feedback and action mapping
  - `VirtualJoystick` - Analog control with customizable deadzones and sensitivity
  - `VirtualDPad` - Directional pad with support for 4 and 8 directions
  - `VirtualTouchPad` - Multi-touch gesture area for camera and viewport control

    **Key Features:**
  - **Haptic Feedback Integration** - Vibration support for mobile devices and gamepads
  - **Input Device Tracking** - `LastInputType` API automatically detects and tracks the active input method (Touch, Keyboard/Mouse, Gamepad)
  - **Dynamic UI Adaptation** - Virtual controls automatically appear/hide based on the detected device
  - **Visibility Modes** - `ALWAYS`, `TOUCHSCREEN_ONLY`, `WHEN_PAUSED` for flexible UI behavior
  - **Action Mapping** - Direct integration with Godot's InputMap system
  - **Polymorphic Input System** - The `InputEvent` base class now uses virtual methods for action matching and event comparison, allowing custom virtual events to integrate seamlessly into the engine core as first-class citizens.
  - **Editor Integration** - Full inspector support with visual configuration
  - **Performance Optimized** - Minimal overhead with efficient event handling

- [x] **Save Server** - Persistence and Serialization Orchestrator

     A robust, high-performance persistence system integrated into the engine core, allowing for secure and asynchronous saving and loading of complex states.

     **Key Features:**

- **Declarative Protocol (@persistent)** - Full automation via GDScript annotations and C++ flags. Supports strong typing and hierarchical organization via `@persistent_group`.
- **Threaded Architecture** - I/O operations, ZSTD compression, and AES-256 encryption run on dedicated threads, ensuring fluid autosaves without stuttering.
- **Hybrid Architecture (Manifest/Satellites)** - Supports partial persistence through tags, allowing sub-sets of data (e.g., inventory, world) to be saved in modular satellite files.
- **Dynamic Spawning & Cleanup** - Automatic reconstruction of nodes instantiated at runtime and removal of orphans (dead enemies) to ensure total parity between the scene and the snapshot.
- **Incremental System (Amend Save)** - Inspired by Git, it tracks modified objects and applies surgical "patches" to the snapshot, reducing disk writes by up to 95%.
- **Global ID Registry (persistence_id)** - Hierarchy abstraction that detaches data from NodePaths, preventing save breakage if nodes are renamed or moved.
- **Data Evolution (Migrations)** - Versioning system that allows registering data transformations (`register_migration`) to maintain compatibility with legacy saves.
- **Safety and Integrity** - Rotative backup system, SHA-256 checksum validation, and *Flush* mechanism on shutdown to prevent data corruption.

### In Development

- [ ] **Library (Bottom Panel)** - Grid-based Content Manager
  - **Native Coexistence** - Functions as an additional panel to the traditional FileSystem, focused on productivity without altering the file structure.
  - **Grid View** - Direct content display organized by specialized tabs (Resources, Scenes, Scripts, Assets), eliminating deep folder navigation.
  - **Context Focusing** - Intelligent filtering that displays only what is relevant to the current development moment.

- [ ] **Level Streaming System (LSS)** - World and State Orchestration
  - **GSM (Game State Machine)** - The `LSSServer` manages the application lifecycle through deterministic states (`BOOT`, `MENU`, `LOADING`, `GAMEPLAY`, `TRANSITION`).
  - **StreamingZone** - System of spatial volumes that manage asynchronous loading of world chunks based on player position.
  - **Super Node Architecture** - The `LSSRoot` allows the coexistence of multiple worlds or isolated "universes", facilitating map transitions without interrupting core logic.
  - **State Hydration** - Integrated with the Save Server to ensure loaded zones recover their exact state instantly.

- [ ] **Gameplay Ability System (GAS)** - Data-Oriented Gameplay Framework

    A high-performance native implementation of the GAS pattern, designed to scale from simple mechanics to complex RPG combat systems.

    **Architecture:**
  - `AbilitySystemComponent`: The central processor attached to entities (Player, Enemies).
  - `AttributeSet`: Defines vital statistics (HP, Mana, Stamina) with replication and modifier calculations.
  - `GameplayAbility`: Modular ability logic (Jump, Shoot, Magic).
  - `GameplayEffect`: Attribute alteration rules (Damage, Healing, Buffs/Debuffs).

    **Editor Integration (EditorPlugin):**
    GAS uses a **Modal Editor (Popup)** triggered via Inspector, similar to the `AudioStreamInteractive` system.
  - **Floating Graph Window** - Abilities are edited in a dedicated overlaid window, allowing quick visual access without losing the context of the main 3D/2D scene.
  - **Edit Button** - The inspector provides direct access to the visual editor when an ability Resource is selected.
  - **Tag Management** - Visual `GameplayTags` selector to define complex interactions (e.g., "Stun" cancels "Casting") without hard-coding.
  - **Live Debug** - Real-time visualization of ability execution flow and effect stacks.

    **Differentials:**
  - **Determinism** - Optimized for multiplayer games with prediction and reconciliation.
  - **Data-Driven** - Designers can create entire ability variations just by changing configurations in the Editor.

- [ ] **Behavior Tree System** - Modular and Reactive AI

    A robust node-based AI implementation, focused on creating complex behaviors through simple and reusable visual logic.

    **Architecture:**
  - `BTAgent`: The orchestrator attached to the NPC, managing memory (`Blackboard`) and tree ticking.
  - **Composites** - Flow nodes (`Sequence`, `Selector`) that define decision making.
  - **Decorators** - Conditional logic and modifiers (`Cooldown`, `Loop`, `Blackboard Check`) that wrap other nodes.
  - **Leafs** - The actual work units (`MoveTo`, `Wait`, `ActivateAbility`).

    **Editor Integration (EditorPlugin):**
    Behavior Trees use a **Dedicated Visual Editor** that operates in exclusive mode.
  - **Visual Board** - Infinite canvas to organize complex AI hierarchies with visual clarity, supporting drag-and-drop of nodes.
  - **Live Debugging** - Visual tracking of execution flow in real-time (nodes light up while active).
  - **Sub-Trees** - Capability to create nodes that encapsulate entire trees, allowing massive reuse (e.g., a "Combat" tree used inside various different AIs).

    **Differentials:**
  - **Native & GAS** - Designed to trigger `GameplayAbilities` directly via action nodes.
  - **Performance** - Tree traversal in pure C++, eliminating script bottlenecks in dense AIs.

- [ ] **Inventory System** - Item and Transaction Management
  - **InventoryServer** - Authoritative singleton that validates all item movements, preventing inconsistent states or cheating.
  - **Equipment Bridge (GAS)** - When equipping an item, the system automatically injects/removes *Gameplay Abilities* and *Effects* on the bearer's component, without manual scripts.
  - **Smart UI Nodes** - Set of `Control` nodes (`InventoryGrid`, `EquipmentSlot`) that manage drag-and-drop and automatic synchronization with the server.
  - **Loot Tables** - Procedural generation integrated with the GAS luck and tag system.

- [ ] **Camera System (vCam)** - Cinematographic Arbitration
  - **Virtual Camera Resources** - Camera profiles defined as Resources, containing FOV, Follow Target, LookAt, and constraints.
  - **Priority Arbiter** - The `CameraServer` evaluates in real-time which vCam has the highest priority to take control of the main viewport with configurable blends.
  - **Procedural Shake** - Trauma system based on Perlin noise, where shake profiles are editable and cumulative Resources.
  - **Viewport Integration** - Uses **Custom 3D Gizmos** to draw and visually adjust transition zones, tracking paths, and influence volumes directly in the scene.

- [ ] **Audio System Improvements**

    Zyris expands and overhauls its native audio system, acting directly on existing components (`AudioServer`, `AudioStream`, `AudioStreamPlayer`, `AudioBus`, `AudioEffect`), without introducing parallel servers or external naming conventions.

    **Scope of Improvements:**
  - **Overhaul of AudioStreamPlayers and Listeners**
    - Players now support high-level sound logic.
    - Direct integration with events, states, and gameplay parameters.
    - More expressive behavior without abandoning Godot's model.

  - **Expansion of existing Audio Resources**
    - `AudioStream` with metadata and dynamic parameters.
    - Support for variations, layers, and contextual resolution.

  - **Introduction of new Audio Resources**
    - New types strictly follow Godot's standard inheritance and existing naming.
    - Direct integration with the AudioServer.

  - **Advanced Mixing and DSP**
    - DSP per stream, bus, or global (Equalization, filters, compression, and temporal effects).
    - Dynamic control via code or events.

  - **Sound Events**
    - Sounds are no longer triggered just by files; execution based on intent and context.
    - Dynamic audio resolution at runtime.

  - **Gameplay Integration**
    - Direct integration with GAS, Behavior Trees, LSS, and Save Server.
    - Game states influence mixing, execution, and sound transitions.

    These improvements elevate Zyris audio to a modern, reactive, and scalable level, maintaining conceptual compatibility with the base engine.

- [ ] **Sounds Engine** - Audio Middleware & Design

    Native middleware solution that revolutionizes the audio workflow, replacing direct file playback ("Play Sound") with an architecture based on **Logical Events**, similar to FMOD Studio and Wwise.

    **System Philosophy:**
    Instead of the programmer choosing *which* file to play, they only signal *what* happened (e.g., `PlayEvent("Footsteps")`). The Sounds Engine decides the auditory result based on logic pre-configured by the Sound Designer.

    **Key Features:**
  - **Probabilistic and Conditional Logic** - Sound changes dynamically depending on game parameters (e.g., Floor surface, player health, wind speed) without extra code.
  - **Automatic Variation** - Pitch, Volume, and Container randomization system to avoid the "machine gun effect" (robotic repetition) in repetitive sounds.
  - **Hierarchical Mixing** - Professional Bus (Group) and VCA systems with Sidechaining (e.g., lowering music volume when someone speaks) and mixing snapshots.
  - **Real-Time Profiler** - Connect the editor to the running game to adjust volume curves, effects, and visualize voice consumption live.

    This ensures total independence for the audio team and gameplay code agnostic to sound implementation.

- [ ] **AOT Export System** - SDK-based Architecture

    The AOT Export System is one of the central pillars of the Zyris vision, designed to deliver high-performance native execution without altering Godot's development flow.

    AOT does not replace the VM during development. It acts exclusively at export time, removing the VM only from the final built game and generating native binaries targeted at the destination hardware.

    **Key Benefits:**
  - **Native Performance** - By removing the Virtual Machine (VM) from the final executable, the game runs directly on hardware, eliminating script interpretation overhead.
  - **Security (Anti-Hack)** - Compilation to native machine code makes reverse engineering exponentially harder compared to script or bytecode decompilation, protecting intellectual property.
  - **Export-Only** - The development flow remains agile with Hot-Reloading, while AOT acts only in the final build pipeline.

    To ensure sustainability, scalability, and ease of contribution, AOT is conceived as an external SDK, integrated into the engine's export pipeline.

    **Model Guidelines:**
  - Execution only on export (does not participate in editor or debug)
  - Editor remains lightweight and decoupled from compiler cost
  - Contributions facilitated via dedicated repository
  - Architecture aligned with professional SDK models (Android SDK, NDK)

    The SDK uses Python as an orchestration layer, responsible for coordinating complex compilation pipelines, IR transformation, and integration with native toolchains.

## Project Showcase

To demonstrate the full power of the Zyris Engine, we maintain a set of projects that serve both as stress tests and as a showcase of the implemented technologies. Our strategy follows the cycle: **GDScript Prototyping → Validation → Native C++ Implementation → Game Refactoring.**

### 1. NeonShooter (2D GAS Showcase)

**Status:** Functional (GAS Lite Implementation)

- **Technologies:** GAS Lite (GDScript), SaveServer, Virtual Input Devices, Screen Shake System.
- **Highlight:** Demonstrates how complex ability and persistence systems can be seamlessly integrated into a high-speed 2D environment.

### 3. Horizon Zero (3D Cyber-Movement Shooter)

**Status:** Planned / In Development

- **Technologies:** Jolt Physics, 3D GAS, Behavior Tree (AI), vCam (Virtual Camera), Particle Stress Test.
- **Highlight:** Focused on validating the performance of physics-based movement (Wall-run, momentum) and complex reactive AIs, exploring the full potential of the Forward+ renderer.

## Contribution

Zyris welcomes contributions from developers who share our vision of creating a world-class game engine.

### Development Philosophy

We follow the **"Godot Way"** for engine core development:

- Strict adherence to Godot's C++ style guidelines
- Use of engine types (`Vector<T>`, `String`, `Ref<T>`)
- Performance-focused approach with cache locality optimization
- Comprehensive documentation for all public APIs

See our [Development Manifesto](.github/DEVELOPMENT.md) for detailed guidelines.

### Prerequisites

To contribute to engine development, you will need the following tools installed:

- **Git**: For version control.
- **Python 3.x**: Required for the build system (SCons).
- **SCons**: Build system used.
- **Pre-commit**: To ensure code formatting and style before commit.

## License

Zyris Engine is licensed under the **MIT License**, maintaining compatibility with Godot Engine licensing.

Copyright (c) 2025-present Zyris contributors
Copyright (c) 2014-present Godot Engine contributors
Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur

See LICENSE.txt and COPYRIGHT.txt for full details.

## Acknowledgments

Zyris is built upon the incredible work of the **Godot Engine** community. We are deeply grateful to:

- **Juan Linietsky** and **Ariel Manzur** - Founders of Godot Engine
- All **Godot Engine contributors** - For creating a fantastic foundation
- The **open-source community** - For making projects like this possible

For the full list of Godot contributors, see [AUTHORS.md](AUTHORS.md) and [DONORS.md](DONORS.md).

## Links

- **Godot Engine Official**: <https://godotengine.org>
- **Godot GitHub**: <https://github.com/godotengine/godot>
- **Zyris Repository**: <https://github.com/Kaffyn/ZyrisEngine>
- **Issue Tracker**: <https://github.com/Kaffyn/ZyrisEngine/issues>

---

**Zyris Engine** - Expanding the boundaries of open-source game development
