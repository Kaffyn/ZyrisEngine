/**************************************************************************/
/*  gameplay_cue_spec.cpp                                                 */
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

#include "modules/gas/core/gameplay_cue_spec.h"
#include "modules/gas/scene/ability_system_component.h"
#include "scene/3d/audio_stream_player_3d.h"
#include "scene/main/node.h"
#include "scene/main/window.h"
#include "scene/animation/animation_player.h"

void GameplayCueSpec::invoke(AbilitySystemComponent *p_asc, const Vector3 &p_location) {
	if (cue.is_null() || p_asc == nullptr) {
		return;
	}

	Node *root = p_asc->get_tree()->get_root(); // Or p_asc->get_parent() -> get_scene()
	// Usually better to attach to root or specific container for transients

	// VFX
	if (cue->get_vfx_scene().is_valid()) {
		vfx_instance = cue->get_vfx_scene()->instantiate();
		Node3D *node_3d = Object::cast_to<Node3D>(vfx_instance);
		if (node_3d) {
			node_3d->set_position(p_location);
			root->add_child(vfx_instance);
		} else {
			memdelete(vfx_instance);
			vfx_instance = nullptr;
		}
	}

	// SFX
	if (cue->get_sfx().is_valid()) {
		Node *cached_ap = p_asc->get_audio_player_node();
		if (cached_ap) {
			cached_ap->call("set_stream", cue->get_sfx());
			cached_ap->call("play");
		} else {
			AudioStreamPlayer3D *player = memnew(AudioStreamPlayer3D);
			player->set_stream(cue->get_sfx());
			player->set_position(p_location);
			player->set_autoplay(true);
			root->add_child(player);
			sfx_instance = player;
		}
	}

	// Animation
	if (cue->get_animation_name() != StringName()) {
		AnimationPlayer *cached_anim = p_asc->get_animation_player_node();
		if (cached_anim) {
			if (cached_anim->has_animation(cue->get_animation_name())) {
				cached_anim->play(cue->get_animation_name());
			}
		}
	}

	// Custom Logic
	cue->execute(p_asc, p_location);
}

void GameplayCueSpec::remove() {
	if (vfx_instance) {
		vfx_instance->queue_free();
		vfx_instance = nullptr;
	}
	if (sfx_instance) {
		sfx_instance->queue_free();
		sfx_instance = nullptr;
	}
}

void GameplayCueSpec::_bind_methods() {
	ClassDB::bind_method(D_METHOD("invoke", "asc", "location"), &GameplayCueSpec::invoke);
	ClassDB::bind_method(D_METHOD("remove"), &GameplayCueSpec::remove);
	ClassDB::bind_method(D_METHOD("set_cue", "cue"), &GameplayCueSpec::set_cue);
	ClassDB::bind_method(D_METHOD("get_cue"), &GameplayCueSpec::get_cue);
}

GameplayCueSpec::GameplayCueSpec() {
}

GameplayCueSpec::~GameplayCueSpec() {
	// Don't auto-remove instances here because RefCounted might die quickly if not held,
	// but we might want the effect to linger (fire and forget).
	// However, usually we want to clean up. For now, let's play safe and not auto-delete on destructor
	// unless we implement explicit lifecycle management.
}
