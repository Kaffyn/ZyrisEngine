/**************************************************************************/
/*  gameplay_cue.h                                                        */
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

#include "core/io/resource.h"
#include "scene/resources/packed_scene.h"
#include "scene/resources/audio_stream.h"

class AbilitySystemComponent;

class GameplayCue : public Resource {
	GDCLASS(GameplayCue, Resource);

	StringName cue_tag;
	Ref<PackedScene> vfx_scene;
	Ref<AudioStream> sfx;
	real_t camera_shake_intensity = 0.0;

	StringName animation_name;

protected:
	static void _bind_methods();

	GDVIRTUAL2(_execute_cue, Object *, Vector3)

public:
	void set_cue_tag(const StringName &p_tag) { cue_tag = p_tag; }
	StringName get_cue_tag() const { return cue_tag; }

	void set_vfx_scene(const Ref<PackedScene> &p_scene) { vfx_scene = p_scene; }
	Ref<PackedScene> get_vfx_scene() const { return vfx_scene; }

	void set_sfx(const Ref<AudioStream> &p_sfx) { sfx = p_sfx; }
	Ref<AudioStream> get_sfx() const { return sfx; }

	void set_camera_shake_intensity(real_t p_intensity) { camera_shake_intensity = p_intensity; }
	real_t get_camera_shake_intensity() const { return camera_shake_intensity; }

	void set_animation_name(const StringName &p_name) { animation_name = p_name; }
	StringName get_animation_name() const { return animation_name; }

	void execute(AbilitySystemComponent *p_asc, const Vector3 &p_location);

	GameplayCue();
};
