/**************************************************************************/
/*  ability_system_task.cpp                                               */
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

#include "ability_system_task.h"
#include "modules/ability_system/scene/ability_system_component.h"

void AbilitySystemTask::_bind_methods() {
	ADD_SIGNAL(MethodInfo("completed"));

	ClassDB::bind_method(D_METHOD("activate"), &AbilitySystemTask::activate);
	ClassDB::bind_method(D_METHOD("end_task"), &AbilitySystemTask::end_task);

	ClassDB::bind_static_method("AbilitySystemTask", D_METHOD("wait_delay", "owner", "delay"), &AbilitySystemTask::wait_delay);
	ClassDB::bind_static_method("AbilitySystemTask", D_METHOD("play_montage", "owner", "animation_name"), &AbilitySystemTask::play_montage);

	BIND_ENUM_CONSTANT(TASK_GENERIC);
	BIND_ENUM_CONSTANT(TASK_WAIT_DELAY);
	BIND_ENUM_CONSTANT(TASK_PLAY_MONTAGE);
	BIND_ENUM_CONSTANT(TASK_WAIT_EVENT);
}

Ref<AbilitySystemTask> AbilitySystemTask::wait_delay(AbilitySystemComponent *p_owner, float p_delay) {
	Ref<AbilitySystemTask> task;
	task.instantiate();
	task->set_owner(p_owner);
	task->task_type = TASK_WAIT_DELAY;
	task->delay_remaining = p_delay;
	return task;
}

Ref<AbilitySystemTask> AbilitySystemTask::play_montage(AbilitySystemComponent *p_owner, const StringName &p_anim) {
	Ref<AbilitySystemTask> task;
	task.instantiate();
	task->set_owner(p_owner);
	task->task_type = TASK_PLAY_MONTAGE;
	task->animation_name = p_anim;
	return task;
}

void AbilitySystemTask::activate() {
	if (task_type == TASK_PLAY_MONTAGE && owner) {
		owner->play_montage(animation_name);
		started = true;
	} else if (task_type == TASK_WAIT_DELAY && delay_remaining <= 0) {
		end_task();
	}
}

void AbilitySystemTask::tick(float p_delta) {
	if (finished) {
		return;
	}

	switch (task_type) {
		case TASK_WAIT_DELAY: {
			delay_remaining -= p_delta;
			if (delay_remaining <= 0) {
				end_task();
			}
		} break;

		case TASK_PLAY_MONTAGE: {
			if (!started) {
				break;
			}
			if (!owner) {
				end_task();
				break;
			}

			// Verify if any montage-capable node exists in cache
			bool has_node = owner->animation_player != nullptr || owner->animated_sprite_2d != nullptr || owner->animated_sprite_3d != nullptr;

			if (!has_node || !owner->is_montage_playing(animation_name)) {
				end_task();
			}
		} break;

		default:
			break;
	}
}

void AbilitySystemTask::end_task() {
	if (finished) {
		return;
	}
	finished = true;
	emit_signal("completed");
}

AbilitySystemTask::AbilitySystemTask() {
}

AbilitySystemTask::~AbilitySystemTask() {
}
