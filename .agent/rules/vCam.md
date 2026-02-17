# Sistema de Câmera Virtual (vCam) - Módulo Independente

## 1. Visão Geral (Arquitetura de Módulo)

O sistema **vCam** da Zyris Engine é implementado como um módulo centralizado em `modules/vcam`. Esta arquitetura garante que o sistema de câmeras virtuais seja desacoplado do core da engine, facilitando a manutenção e a expansão sem sujar os servidores base (`RenderingServer`, `CameraServer`).

### Componentes Principais

1. **`vCamServer`**: O Singleton orquestrador (Diretor Virtual) que decide qual vCam domina o Viewport.
2. **`vCam3D` / `vCam2D`**: Nós especializados que definem as propriedades cinematográficas.
3. **Decoupled Integration**: As vCams não modificam o node `Camera3D` original, mas injetam transformações e estados nele via `vCamServer`.

---

## 2. O Servidor `vCamServer` (Singleton)

O `vCamServer` é o cérebro do sistema, registrado via `Engine::get_singleton()`.

### Responsabilidades

- **Registry**: Mantém uma lista de todas as vCams ativas na cena.
- **Arbitration**: Avalia em tempo real a `Priority` de cada vCam.
- **Blending**: Gerencia a interpolação entre a câmera física (`Camera3D`/`Camera2D`) e a vCam dominante.
- **Shake Mixer**: Combina múltiplas camadas de trauma e shake procedural antes de aplicar o deslocamento final.

---

## 3. Nós `vCam3D` e `vCam2D`

Ao contrário das câmeras tradicionais, os nós vCam são "olhos virtuais" que servem como beacons de configuração.

### 3.1 Propriedades de Direção

- **`Priority`**: Nível de influência (0-1000+).
- **`Shot_Profile`**: Recurso que define a óptica (Virtual Lenses, mm, f-stop).
- **`Composition_Rules`**: Definições de enquadramento (Terços, Golden Ratio).

### 3.2 Rigging & Movement

- **`Follow_Target` / `LookAt_Target`**: Alvos de rastreamento.
- **`Rig_Type`**: Define o comportamento físico (Dolly, Crane, Orbital).

---

## 4. Integração Cinematográfica

### 4.1 Blending entre vCams

Quando o `vCamServer` detecta uma mudança de prioridade, ele inicia um processo de blend:

1. Captura o estado atual da câmera física.
2. Interpola suavemente (Tween/Ease) para o novo estado definido pela `vCam_Dominante`.
3. Atualiza os `CameraAttributesPhysical` (DOF, Exposure) dinamicamente.

### 4.2 Trauma e Shake Procedural

- O trauma é aplicado à vCam ativa ou globalmente via `vCamServer`.
- O servidor processa o Perlin Noise e aplica o offset na `Camera3D` correspondente.

---

## 5. Estrutura de Arquivos (`modules/vcam`)

O módulo deve ser auto-contido:

- `vcam_server.cpp/h`: Lógica do Singleton.
- `vcam_3d.cpp/h`: Implementação do nó 3D.
- `vcam_2d.cpp/h`: Implementação do nó 2D.
- `register_types.cpp/h`: Registro das classes no `ClassDB`.
- `vcam_gizmos.cpp`: (Opcional) Implementação dos gizmos do editor.

---

## 6. Exemplo de Uso (GDScript)

```gdscript
func _ready():
    # vCamServer é acessível como um Singleton global
    vCamServer.apply_global_shake(0.5, 2.0)

    # Ativando uma câmera cinematográfica
    $CutsceneCam.priority = 500
```

---

_Este documento define o padrão para o módulo vcam. A implementação deve seguir estritamente o desacoplamento das câmeras core do Godot._
