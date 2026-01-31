/**************************************************************************/
/*  virtual_joystick_dynamic.h                                            */
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

#pragma once

#include "scene/gui/virtual_joystick.h"

class VirtualJoystickDynamic : public VirtualJoystick {
	GDCLASS(VirtualJoystickDynamic, VirtualJoystick);

	bool visible_by_default = false;
	Vector2 default_position;
	float joystick_size = 100.0f;

protected:
	void _notification(int p_what);
	void _validate_property(PropertyInfo &p_property) const;
	virtual void _draw_joystick() override;
	static void _bind_methods();

	// Helper to get the effective default position (either manual or auto-centered)
	Vector2 _get_effective_default_position() const;

#ifdef TOOLS_ENABLED
	virtual int _edit_get_handle_count() const override;
	virtual String _edit_get_handle_name(int p_idx) const override;
	virtual Variant _edit_get_handle_value(int p_idx) const override;
	virtual void _edit_set_handle(int p_idx, const Variant &p_value) override;
#endif

public:
	void set_visible_by_default(bool p_visible);
	bool is_visible_by_default() const;

	void set_default_position(const Vector2 &p_pos);
	Vector2 get_default_position() const;

	void set_joystick_size(float p_size);
	float get_joystick_size() const;

	virtual float get_joystick_radius() const override;

	virtual void _on_touch_down(int p_index, const Vector2 &p_pos) override;
	virtual void _on_touch_up(int p_index, const Vector2 &p_pos) override;
	virtual void _reset_joystick() override;

	VirtualJoystickDynamic();
};
