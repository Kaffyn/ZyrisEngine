# Virtual Input Devices Implementation

This document describes the implementation of the **Virtual Input Devices** system in Zyris Engine. This system provides a high-performance, polymorphic abstraction layer for touch-based inputs, integrating them as first-class citizens alongside keyboard, mouse, and joypad inputs.

## 1. Core Architecture

The system is built on two new `InputEvent` types and a specialized `Input` state management.

### New Input Event Types (`core/input/input_event.h`)

- **`InputEventVirtualButton`**: Represents a state-based virtual button (pressed/released). It carries a `button_index` (0-15 by default).
- **`InputEventVirtualMotion`**: Represents a virtual axis deflection. It carries an `axis` index and an `axis_value` (floating point, usually -1.0 to 1.0).

These events inherit from `InputEvent` and implement polymorphic methods like `action_match` and `is_match`, allowing them to work seamlessly with the engine's `InputMap`.

### Input Singleton Extensions (`core/input/input.h`)

The `Input` singleton was extended to track the state of "Virtual Devices":

- **`VirtualDeviceState`**: A struct containing a set of pressed virtual buttons and a map of virtual axis values.
- **`virtual_device_states`**: A map that allows multiple virtual devices to coexist (though typically only device 0 is used).
- **`is_virtual_button_pressed()`** and **`get_virtual_axis_value()`**: New API methods to query virtual state directly.

The `_parse_input_event_impl` method in `input.cpp` now updates these states whenever a virtual event is parsed. It also triggers `_set_last_input_type(LAST_INPUT_TOUCH)` to support automatic UI adaptation.

## 2. Engine Integration

### Input Map Integration (`core/input/input_map.cpp/h`)

The `InputMap` class now defines standard indices for virtual buttons and axes to ensure consistency across the engine:

- **`VirtualButton`**: `DPAD_UP` (12), `DPAD_DOWN` (13), `DPAD_LEFT` (14), `DPAD_RIGHT` (15).
- **`VirtualAxis`**: `LEFT_X` (0), `LEFT_Y` (1), `RIGHT_X` (2), `RIGHT_Y` (3).

The default engine actions (`ui_left`, `ui_right`, `ui_up`, `ui_down`) were updated in `load_default()` to include these virtual mappings. For example, `ui_left` is now triggered by:

- `InputEventVirtualButton` with index 14.
- `InputEventVirtualMotion` with axis 0 and value -1.0.

This ensures that UI navigation works out-of-the-box with virtual controls.

### Serialization (`core/input/input_event_codec.cpp`)

The `InputEventCodec` was updated to support the new event types, ensuring they can be saved to project settings, used in RPCs, or recorded in input streams.

### Editor Support (`editor/settings/`)

- **`EventListenerLineEdit`**: Updated to capture virtual events during "listen" mode in the Input Map editor.
- **`InputEventConfigurationDialog`**: Updated to allow manual configuration and addition of Virtual Button and Virtual Motion events to actions.

## 3. GUI Node System (`scene/gui/`)

The system provides a hierarchy of nodes for easy implementation in games.

### `VirtualDevice` (Base Class)

- Handles touch tracking and visibility logic.
- **Visibility Modes**: `ALWAYS` or `TOUCHSCREEN_ONLY` (automatically hides if the last input was keyboard/mouse/joypad).
- Proxies touch/mouse events into a clean `_on_touch_down`, `_on_touch_up`, and `_on_drag` interface for subclasses.

### Specialized Nodes

- **`VirtualButton`**: A button that maps to a `button_index`. It mimics `BaseButton` but is optimized for touch and virtual event dispatch. Supports textures, text, and icons.
- **`VirtualJoystick`**: Provides a standard joystick interface.
  - **Modes**: `FIXED` (stays in place) or `DYNAMIC` (re-centers to where the user first touches).
  - **Zones**: Configurable `deadzone_size` and `clamp_zone_size` (percentage of radius).
- **`VirtualDPad`**: A 4/8-way directional pad that maps to 4 distinct virtual button indices.
- **`VirtualTouchPad`**: An invisible or decorative area for gesture tracking (e.g., camera control). It generates `InputEventVirtualMotion` based on relative drag movement.
- **`VirtualJoystickDynamic`**: An extension of `VirtualJoystick` specifically tailored for "touch anywhere" joystick behavior.

## 4. Automatic Input Detection

The system introduces **`LastInputType`** tracking in `Input`:

- `LAST_INPUT_KEYBOARD_MOUSE`
- `LAST_INPUT_JOYPAD`
- `LAST_INPUT_TOUCH`

Whenever a hardware or virtual event is received, the engine updates this state and provides it via `Input::get_singleton()->get_last_input_type()`. The `VirtualDevice` nodes listen for this change to toggle their visibility if configured to `TOUCHSCREEN_ONLY`.

## 5. Performance and Best Practices

- **Atomic Events**: Virtual nodes dispatch events directly to `Input::parse_input_event()`, benefiting from the engine's built-in event accumulation and flushing.
- **Polymorphism**: By extending `InputEvent`, virtual devices don't require special "hacks" in game logic; use `Input.is_action_pressed()` as usual.
- **Cache Locality**: State tracking in `Input` uses optimized containers (`RBSet`, `HashMap`) to ensure minimal overhead during high-frequency input polling.
