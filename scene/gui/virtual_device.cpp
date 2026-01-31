/**************************************************************************/
/*  virtual_device.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "virtual_device.h"

#include "core/config/engine.h"
#include "core/input/input.h"
#include "scene/main/viewport.h"
#include "scene/theme/theme_db.h"
#include "servers/display/display_server.h"

void VirtualDevice::_update_theme_item_cache() {
}

void VirtualDevice::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (!Engine::get_singleton()->is_editor_hint()) {
				if (!Input::get_singleton()->is_connected(SNAME("last_input_device_changed"), callable_mp(this, &VirtualDevice::_on_input_type_changed))) {
					Input::get_singleton()->connect(SNAME("last_input_device_changed"), callable_mp(this, &VirtualDevice::_on_input_type_changed));
				}

				// Initial check
				if (visibility_mode == VISIBILITY_TOUCHSCREEN_ONLY) {
					_on_input_type_changed(Input::get_singleton()->get_last_input_type());
				}
			}
			_update_theme_item_cache();
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			_update_theme_item_cache();
			queue_redraw();
		} break;
		case NOTIFICATION_MOUSE_ENTER: {
			hovering = true;
			queue_redraw();
		} break;
		case NOTIFICATION_MOUSE_EXIT: {
			hovering = false;
			queue_redraw();
		} break;
		case NOTIFICATION_VISIBILITY_CHANGED:
		case NOTIFICATION_EXIT_TREE: {
			if (p_what == NOTIFICATION_EXIT_TREE && !Engine::get_singleton()->is_editor_hint()) {
				if (Input::get_singleton()->is_connected(SNAME("last_input_device_changed"), callable_mp(this, &VirtualDevice::_on_input_type_changed))) {
					Input::get_singleton()->disconnect(SNAME("last_input_device_changed"), callable_mp(this, &VirtualDevice::_on_input_type_changed));
				}
			}

			if (p_what == NOTIFICATION_VISIBILITY_CHANGED && is_visible_in_tree()) {
				break;
			}
			// Reset state if hidden or removed
			if (pressed) {
				pressed = false;
				current_touch_index = -1;
				pressed_state_changed();
			}
			hovering = false;
			queue_redraw();
		} break;
	}
}

void VirtualDevice::_on_input_type_changed(int p_type) {
	if (visibility_mode != VISIBILITY_TOUCHSCREEN_ONLY) {
		return;
	}

	if (p_type == Input::LAST_INPUT_TOUCH) {
		show();
	} else if (p_type == Input::LAST_INPUT_KEYBOARD_MOUSE || p_type == Input::LAST_INPUT_JOYPAD) {
		hide();
	} else if (p_type == Input::LAST_INPUT_UNKNOWN) {
		// Fallback to DisplayServer capability check if we don't have recent history
		if (!DisplayServer::get_singleton()->is_touchscreen_available()) {
			hide();
		} else {
			show();
		}
	}
}

void VirtualDevice::gui_input(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());

	Ref<InputEventScreenTouch> st = p_event;
	if (st.is_valid()) {
		if (st->is_pressed()) {
			// Touch Down
			if (current_touch_index == -1) { // Accept new touch if we aren't busy
				current_touch_index = st->get_index();
				pressed = true;
				_on_touch_down(current_touch_index, st->get_position());
				pressed_state_changed();
				queue_redraw();
				accept_event();
			}
		} else {
			// Touch Up
			if (st->get_index() == current_touch_index) {
				pressed = false;
				current_touch_index = -1;
				_on_touch_up(st->get_index(), st->get_position());
				pressed_state_changed();
				queue_redraw();
				// Don't accept event here always, maybe we want it to propagate?
				// Usually controls consume their interactions.
				accept_event();
			}
		}
		return;
	}

	Ref<InputEventScreenDrag> sd = p_event;
	if (sd.is_valid()) {
		if (sd->get_index() == current_touch_index) {
			_on_drag(sd->get_index(), sd->get_position(), sd->get_relative());
			accept_event();
		}
		return;
	}

	// Mouse fallback for testing on Desktop
	Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid()) {
		if (mb->is_pressed() && mb->get_button_index() == MouseButton::LEFT) {
			if (current_touch_index == -1) {
				current_touch_index = -2; // mouse identifier
				pressed = true;
				_on_touch_down(current_touch_index, mb->get_position());
				pressed_state_changed();
				queue_redraw();
				accept_event();
			}
		} else if (!mb->is_pressed() && mb->get_button_index() == MouseButton::LEFT) {
			if (current_touch_index == -2) {
				pressed = false;
				current_touch_index = -1;
				_on_touch_up(-2, mb->get_position());
				pressed_state_changed();
				queue_redraw();
				accept_event();
			}
		}
		return;
	}

	Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_valid()) {
		if (current_touch_index == -2) {
			_on_drag(-2, mm->get_position(), mm->get_relative());
			accept_event();
		}
	}
}

void VirtualDevice::_on_touch_down(int p_index, const Vector2 &p_pos) {
	// Virtual
}

void VirtualDevice::_on_touch_up(int p_index, const Vector2 &p_pos) {
	// Virtual
}

void VirtualDevice::_on_drag(int p_index, const Vector2 &p_pos, const Vector2 &p_relative) {
	// Virtual
}

void VirtualDevice::pressed_state_changed() {
	// Virtual hook
}

VirtualDevice::DrawMode VirtualDevice::get_draw_mode() const {
	if (pressed) {
		if (hovering) {
			return DRAW_HOVER_PRESSED;
		}
		return DRAW_PRESSED;
	}
	if (hovering) {
		return DRAW_HOVER;
	}
	return DRAW_NORMAL;
}

void VirtualDevice::set_device(int p_device) {
	device = p_device;
}

int VirtualDevice::get_device() const {
	return device;
}

void VirtualDevice::set_visibility_mode(VisibilityMode p_mode) {
	visibility_mode = p_mode;
	if (visibility_mode == VISIBILITY_TOUCHSCREEN_ONLY && is_inside_tree() && !Engine::get_singleton()->is_editor_hint()) {
		_on_input_type_changed(Input::get_singleton()->get_last_input_type());
	} else if (visibility_mode == VISIBILITY_ALWAYS) {
		show();
	}
}

VirtualDevice::VisibilityMode VirtualDevice::get_visibility_mode() const {
	return visibility_mode;
}

bool VirtualDevice::is_pressed() const {
	return pressed;
}

bool VirtualDevice::is_hovered() const {
	return hovering;
}

VirtualDevice::VirtualDevice() {
	set_mouse_filter(MOUSE_FILTER_STOP);
}

void VirtualDevice::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_device", "device"), &VirtualDevice::set_device);
	ClassDB::bind_method(D_METHOD("get_device"), &VirtualDevice::get_device);
	ClassDB::bind_method(D_METHOD("set_visibility_mode", "mode"), &VirtualDevice::set_visibility_mode);
	ClassDB::bind_method(D_METHOD("get_visibility_mode"), &VirtualDevice::get_visibility_mode);
	ClassDB::bind_method(D_METHOD("is_pressed"), &VirtualDevice::is_pressed);
	ClassDB::bind_method(D_METHOD("get_draw_mode"), &VirtualDevice::get_draw_mode);
	ClassDB::bind_method(D_METHOD("_on_input_type_changed", "type"), &VirtualDevice::_on_input_type_changed);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "device"), "set_device", "get_device");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "visibility_mode", PROPERTY_HINT_ENUM, "Always,TouchScreen Only"), "set_visibility_mode", "get_visibility_mode");

	BIND_ENUM_CONSTANT(VISIBILITY_ALWAYS);
	BIND_ENUM_CONSTANT(VISIBILITY_TOUCHSCREEN_ONLY);

	BIND_ENUM_CONSTANT(DRAW_NORMAL);
	BIND_ENUM_CONSTANT(DRAW_PRESSED);
	BIND_ENUM_CONSTANT(DRAW_HOVER);
	BIND_ENUM_CONSTANT(DRAW_HOVER_PRESSED);
}
