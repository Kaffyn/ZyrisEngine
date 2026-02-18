/**************************************************************************/
/*  gameplay_cue.cpp                                                      */
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

#include "modules/gas/resources/gameplay_cue.h"
#include "modules/gas/scene/ability_system_component.h"

void GameplayCue::execute(AbilitySystemComponent *p_asc, const Vector3 &p_location) {
	if (p_asc == nullptr) {
		return;
	}

	// GDScript Override
	GDVIRTUAL_CALL(_execute_cue, p_asc, p_location);
}

void GameplayCue::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_cue_tag", "tag"), &GameplayCue::set_cue_tag);
	ClassDB::bind_method(D_METHOD("get_cue_tag"), &GameplayCue::get_cue_tag);

	ClassDB::bind_method(D_METHOD("set_vfx_scene", "scene"), &GameplayCue::set_vfx_scene);
	ClassDB::bind_method(D_METHOD("get_vfx_scene"), &GameplayCue::get_vfx_scene);

	ClassDB::bind_method(D_METHOD("set_sfx", "sfx"), &GameplayCue::set_sfx);
	ClassDB::bind_method(D_METHOD("get_sfx"), &GameplayCue::get_sfx);

	ClassDB::bind_method(D_METHOD("set_camera_shake_intensity", "intensity"), &GameplayCue::set_camera_shake_intensity);
	ClassDB::bind_method(D_METHOD("get_camera_shake_intensity"), &GameplayCue::get_camera_shake_intensity);

	ClassDB::bind_method(D_METHOD("set_animation_name", "name"), &GameplayCue::set_animation_name);
	ClassDB::bind_method(D_METHOD("get_animation_name"), &GameplayCue::get_animation_name);

	ClassDB::bind_method(D_METHOD("execute", "asc", "location"), &GameplayCue::execute);

	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "cue_tag"), "set_cue_tag", "get_cue_tag");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "vfx_scene", PROPERTY_HINT_RESOURCE_TYPE, "PackedScene"), "set_vfx_scene", "get_vfx_scene");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "sfx", PROPERTY_HINT_RESOURCE_TYPE, "AudioStream"), "set_sfx", "get_sfx");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "camera_shake_intensity"), "set_camera_shake_intensity", "get_camera_shake_intensity");
	ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "animation_name"), "set_animation_name", "get_animation_name");

	GDVIRTUAL_BIND(_execute_cue, "asc", "location");
}

GameplayCue::GameplayCue() {
}
