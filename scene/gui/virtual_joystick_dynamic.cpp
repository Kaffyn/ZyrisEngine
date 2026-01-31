/**************************************************************************/
/*  virtual_joystick_dynamic.cpp                                          */
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

#include "virtual_joystick_dynamic.h"

#include "core/config/engine.h"

Vector2 VirtualJoystickDynamic::_get_effective_default_position() const {
	return (default_position == Vector2()) ? (get_size() / 2.0f) : default_position;
}

void VirtualJoystickDynamic::_notification(int p_what) {
	VirtualJoystick::_notification(p_what);

	switch (p_what) {
		case NOTIFICATION_READY:
		case NOTIFICATION_RESIZED: {
			if (visible_by_default) {
				Vector2 target_pos = _get_effective_default_position();
				base_pos = target_pos;
				tip_pos = target_pos;
			}
		} break;
	}

	if (p_what == NOTIFICATION_RESIZED) {
		notify_property_list_changed();
	}
}

void VirtualJoystickDynamic::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "joystick_size") {
		// Dynamic limit based on rect width (or max dimension)
		float limit = MAX(1.0f, get_size().width);
		p_property.hint_string = vformat("1,%f,1", limit);
	}
}

void VirtualJoystickDynamic::_draw_joystick() {
	if (Engine::get_singleton()->is_editor_hint()) {
		// Draw the region debug rectangle in the editor
		draw_rect(Rect2(Point2(), get_size()), Color(1, 1, 1, 0.2), true);
		draw_rect(Rect2(Point2(), get_size()), Color(1, 1, 1, 0.5), false);

		if (!visible_by_default) {
			// In editor, if not visible by default, only draw the debug region
			return;
		}
	}
	// At runtime, we need to override the drawing to use our joystick_size
	if (!Engine::get_singleton()->is_editor_hint() && !is_pressed() && get_joystick_mode() == JOYSTICK_MODE_DYNAMIC && !visible_by_default) {
		// Don't draw dynamic joystick if not touched (runtime) and not visible by default
		return;
	}

	// joystick_size is the diameter, so radius is half
	float radius = get_joystick_size() / 2.0f;

	Vector2 drawing_base = get_base_pos();
	Vector2 drawing_tip = is_pressed() ? get_tip_pos() : drawing_base;

	// Access parent's theme cache through a local variable reference
	auto &cache = VirtualJoystick::theme_cache;

	// Resolve Textures (respecting base class properties - manual only)
	Ref<Texture2D> b_tex = get_base_texture();
	Ref<Texture2D> t_tex = get_tip_texture();

	// Resolve Colors (property multiplies theme color if both set, or just use property)
	Color b_col = get_base_color() * cache.base_color_theme;
	Color t_col = get_tip_color() * cache.tip_color_theme;

	Rect2 base_rect = Rect2(drawing_base - Vector2(radius, radius), Vector2(radius * 2, radius * 2));

	// Draw Base
	if (b_tex.is_valid()) {
		draw_texture_rect(b_tex, base_rect, false, b_col);
	} else if (cache.base_style.is_valid()) {
		cache.base_style->draw(get_canvas_item(), base_rect);
	}

	// Draw Tip
	float t_radius = radius * get_tip_scale();
	Rect2 tip_rect = Rect2(drawing_tip - Vector2(t_radius, t_radius), Vector2(t_radius * 2, t_radius * 2));

	if (t_tex.is_valid()) {
		draw_texture_rect(t_tex, tip_rect, false, t_col);
	} else if (cache.tip_style.is_valid()) {
		cache.tip_style->draw(get_canvas_item(), tip_rect);
	}
}

void VirtualJoystickDynamic::set_visible_by_default(bool p_visible) {
	visible_by_default = p_visible;
	if (visible_by_default && !is_pressed()) {
		Vector2 target_pos = _get_effective_default_position();
		base_pos = target_pos;
		tip_pos = target_pos;
	}
	queue_redraw();
}

bool VirtualJoystickDynamic::is_visible_by_default() const {
	return visible_by_default;
}

void VirtualJoystickDynamic::set_default_position(const Vector2 &p_pos) {
	default_position = p_pos;
	if (visible_by_default && !is_pressed()) {
		Vector2 target_pos = _get_effective_default_position();
		base_pos = target_pos;
		tip_pos = target_pos;
	}
	queue_redraw();
}

Vector2 VirtualJoystickDynamic::get_default_position() const {
	return default_position;
}

void VirtualJoystickDynamic::_on_touch_down(int p_index, const Vector2 &p_pos) {
	if (visible_by_default) {
		float radius = get_joystick_size() / 2.0f;
		if (p_pos.distance_to(base_pos) <= radius) {
			// Touch is inside the current base area, don't reposition
			tip_pos = p_pos;
			_update_input_vector();
			queue_redraw();
			return;
		}
	}
	// Normal dynamic behavior: reposition base to touch point
	VirtualJoystick::_on_touch_down(p_index, p_pos);
}

void VirtualJoystickDynamic::_on_touch_up(int p_index, const Vector2 &p_pos) {
	VirtualJoystick::_on_touch_up(p_index, p_pos);
	// Note: Parent already called _reset_joystick(), but we may need to adjust positions
	// if visible_by_default is true (handled in our override of _reset_joystick)
}

void VirtualJoystickDynamic::_reset_joystick() {
	// Call parent to reset input vectors and send zero events
	VirtualJoystick::_reset_joystick();

	// Override position behavior for visible_by_default mode
	if (visible_by_default) {
		Vector2 target_pos = _get_effective_default_position();
		base_pos = target_pos;
		tip_pos = target_pos;
	}
}

// ...
float VirtualJoystickDynamic::get_joystick_radius() const {
	return joystick_size / 2.0f;
}

void VirtualJoystickDynamic::set_joystick_size(float p_size) {
	joystick_size = p_size;
	queue_redraw();
}

float VirtualJoystickDynamic::get_joystick_size() const {
	return joystick_size;
}

// ... Gizmo implementation
#ifdef TOOLS_ENABLED
int VirtualJoystickDynamic::_edit_get_handle_count() const {
	return 4; // Size + 2 Base + 1 Position
}

String VirtualJoystickDynamic::_edit_get_handle_name(int p_idx) const {
	switch (p_idx) {
		case 0:
			return "Joystick Size";
		case 1:
			return "Deadzone";
		case 2:
			return "Clamp Zone";
		case 3:
			return "Default Position";
	}
	return "";
}

Variant VirtualJoystickDynamic::_edit_get_handle_value(int p_idx) const {
	Vector2 center = _get_effective_default_position();
	float radius = joystick_size / 2.0f;

	switch (p_idx) {
		case 0:
			return center + Vector2(radius, 0);
		case 1:
			return center + Vector2(get_deadzone_size(), 0);
		case 2:
			return center + Vector2(get_clamp_zone_size(), 0);
		case 3:
			return center;
	}
	return Variant();
}

void VirtualJoystickDynamic::_edit_set_handle(int p_idx, const Variant &p_value) {
	Vector2 center = _get_effective_default_position();
	Vector2 pos = p_value;
	float val = (pos - center).x;

	switch (p_idx) {
		case 0:
			set_joystick_size(MAX(1.0f, val * 2.0f));
			break;
		case 1:
			set_deadzone_size(MAX(0.0f, val));
			break;
		case 2:
			set_clamp_zone_size(MAX(1.0f, val));
			break;
		case 3:
			set_default_position(pos);
			break;
	}
}
#endif

void VirtualJoystickDynamic::_bind_methods() {
#ifdef TOOLS_ENABLED
#endif

	ClassDB::bind_method(D_METHOD("set_visible_by_default", "visible"), &VirtualJoystickDynamic::set_visible_by_default);
	ClassDB::bind_method(D_METHOD("is_visible_by_default"), &VirtualJoystickDynamic::is_visible_by_default);

	ClassDB::bind_method(D_METHOD("set_default_position", "position"), &VirtualJoystickDynamic::set_default_position);
	ClassDB::bind_method(D_METHOD("get_default_position"), &VirtualJoystickDynamic::get_default_position);

	ClassDB::bind_method(D_METHOD("set_joystick_size", "size"), &VirtualJoystickDynamic::set_joystick_size);
	ClassDB::bind_method(D_METHOD("get_joystick_size"), &VirtualJoystickDynamic::get_joystick_size);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "visible_by_default"), "set_visible_by_default", "is_visible_by_default");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "default_position"), "set_default_position", "get_default_position");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "joystick_size", PROPERTY_HINT_RANGE, "1,1024,1"), "set_joystick_size", "get_joystick_size");
}

VirtualJoystickDynamic::VirtualJoystickDynamic() {
	// Defaults to Dynamic mode to act as a region that spawns the joystick
	set_joystick_mode(JOYSTICK_MODE_DYNAMIC);

	// clamp_zone_size is a multiplier of the radius (1.0 = 100% of radius)
	// This allows the tip to move within the full joystick area
	set_clamp_zone_size(1.0f);

	// Default to a transparent control that covers an area
	// Users should likely set the rect to be large (e.g. half screen)
}
