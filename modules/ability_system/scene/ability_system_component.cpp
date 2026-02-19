/**************************************************************************/
/*  ability_system_component.cpp                                          */
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

#include "modules/ability_system/scene/ability_system_component.h"
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/ability_system_ability_spec.h"
#include "modules/ability_system/core/ability_system_effect_spec.h"
#include "modules/ability_system/core/ability_system_task.h"
#include "modules/ability_system/resources/ability_system_ability_container.h"
#include "modules/ability_system/resources/ability_system_attribute_set.h"
#include "modules/ability_system/resources/ability_system_cue.h"
#include "modules/ability_system/resources/ability_system_tag_container.h"

#include "scene/2d/animated_sprite_2d.h"
#include "scene/2d/audio_stream_player_2d.h"
#include "scene/2d/gpu_particles_2d.h"
#include "scene/2d/marker_2d.h"
#include "scene/2d/physics/area_2d.h"
#include "scene/2d/physics/character_body_2d.h"
#include "scene/2d/physics/ray_cast_2d.h"
#include "scene/2d/physics/shape_cast_2d.h"
#include "scene/2d/sprite_2d.h"
#include "scene/3d/audio_stream_player_3d.h"
#include "scene/3d/gpu_particles_3d.h"
#include "scene/3d/marker_3d.h"
#include "scene/3d/physics/area_3d.h"
#include "scene/3d/physics/character_body_3d.h"
#include "scene/3d/physics/ray_cast_3d.h"
#include "scene/3d/physics/shape_cast_3d.h"
#include "scene/3d/sprite_3d.h"
#include "scene/animation/animation_player.h"

void AbilitySystemComponent::_bind_methods() {
	ClassDB::bind_method(D_METHOD("give_ability", "ability", "level"), &AbilitySystemComponent::give_ability, DEFVAL(1));
	ClassDB::bind_method(D_METHOD("apply_ability_container", "container", "level"), &AbilitySystemComponent::apply_ability_container, DEFVAL(1));
	ClassDB::bind_method(D_METHOD("try_activate_ability_by_tag", "tag"), &AbilitySystemComponent::try_activate_ability_by_tag);
	ClassDB::bind_method(D_METHOD("make_outgoing_spec", "effect", "level"), &AbilitySystemComponent::make_outgoing_spec, DEFVAL(1.0f));
	ClassDB::bind_method(D_METHOD("apply_gameplay_effect_spec_to_self", "spec"), &AbilitySystemComponent::apply_gameplay_effect_spec_to_self);
	ClassDB::bind_method(D_METHOD("register_task", "task"), &AbilitySystemComponent::register_task);
	ClassDB::bind_method(D_METHOD("play_montage", "name"), &AbilitySystemComponent::play_montage);
	ClassDB::bind_method(D_METHOD("is_montage_playing", "name"), &AbilitySystemComponent::is_montage_playing);
	ClassDB::bind_method(D_METHOD("execute_cue", "tag", "data"), &AbilitySystemComponent::execute_cue, DEFVAL(Dictionary()));
	ClassDB::bind_method(D_METHOD("add_tag", "tag"), &AbilitySystemComponent::add_tag);
	ClassDB::bind_method(D_METHOD("remove_tag", "tag"), &AbilitySystemComponent::remove_tag);
	ClassDB::bind_method(D_METHOD("has_tag", "tag"), &AbilitySystemComponent::has_tag);
	ClassDB::bind_method(D_METHOD("get_attribute_value", "attribute_name"), &AbilitySystemComponent::get_attribute_value);
	ClassDB::bind_method(D_METHOD("set_attribute_base_value", "name", "value"), &AbilitySystemComponent::set_attribute_base_value);
	ClassDB::bind_method(D_METHOD("add_attribute_set", "set"), &AbilitySystemComponent::add_attribute_set);
	ClassDB::bind_method(D_METHOD("register_cue_resource", "cue"), &AbilitySystemComponent::register_cue_resource);
	ClassDB::bind_method(D_METHOD("unregister_cue_resource", "tag"), &AbilitySystemComponent::unregister_cue_resource);
	ClassDB::bind_method(D_METHOD("get_cue_resource", "tag"), &AbilitySystemComponent::get_cue_resource);
	ClassDB::bind_method(D_METHOD("get_owned_tags"), &AbilitySystemComponent::get_owned_tags);

	ClassDB::bind_method(D_METHOD("set_ability_container", "container"), &AbilitySystemComponent::set_ability_container);
	ClassDB::bind_method(D_METHOD("get_ability_container"), &AbilitySystemComponent::get_ability_container);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "ability_container", PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemAbilityContainer"), "set_ability_container", "get_ability_container");

	ADD_SIGNAL(MethodInfo("attribute_changed", PropertyInfo(Variant::STRING_NAME, "attribute_name"), PropertyInfo(Variant::FLOAT, "old_value"), PropertyInfo(Variant::FLOAT, "new_value")));
	ADD_SIGNAL(MethodInfo("tag_changed", PropertyInfo(Variant::STRING_NAME, "tag_name"), PropertyInfo(Variant::BOOL, "is_present")));
	ADD_SIGNAL(MethodInfo("ability_activated", PropertyInfo(Variant::OBJECT, "ability_spec", PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemAbilitySpec")));
	ADD_SIGNAL(MethodInfo("ability_ended", PropertyInfo(Variant::OBJECT, "ability_spec", PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemAbilitySpec"), PropertyInfo(Variant::BOOL, "was_cancelled")));
	ADD_SIGNAL(MethodInfo("ability_failed", PropertyInfo(Variant::STRING_NAME, "ability_name"), PropertyInfo(Variant::STRING, "reason")));

	ADD_SIGNAL(MethodInfo("effect_applied", PropertyInfo(Variant::OBJECT, "effect_spec", PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemEffectSpec")));
	ADD_SIGNAL(MethodInfo("effect_removed", PropertyInfo(Variant::OBJECT, "effect_spec", PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemEffectSpec")));

	ADD_SIGNAL(MethodInfo("gameplay_event_received", PropertyInfo(Variant::STRING_NAME, "event_tag"), PropertyInfo(Variant::DICTIONARY, "data")));

	ADD_SIGNAL(MethodInfo("cooldown_started", PropertyInfo(Variant::STRING_NAME, "ability_tag"), PropertyInfo(Variant::FLOAT, "duration")));
	ADD_SIGNAL(MethodInfo("cooldown_ended", PropertyInfo(Variant::STRING_NAME, "ability_tag")));
}

void AbilitySystemComponent::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_POST_ENTER_TREE:
		case NOTIFICATION_PARENTED: {
			_update_cache();
			update_configuration_warnings();
		} break;

		case NOTIFICATION_READY: {
			_update_cache();
			if (ability_container.is_valid()) {
				apply_ability_container(ability_container);
			}
			set_process(true);
		} break;
		case NOTIFICATION_PROCESS: {
			float delta = get_process_delta_time();
			_process_effects(delta);
			_process_tasks(delta);
		} break;
	}
}

void AbilitySystemComponent::_process_effects(float p_delta) {
	bool removed_any = false;
	for (int i = active_effects.size() - 1; i >= 0; i--) {
		Ref<AbilitySystemEffectSpec> spec = active_effects[i];
		if (spec->get_effect()->get_duration_policy() == AbilitySystemEffect::DURATION) {
			float remaining = spec->get_duration_remaining() - p_delta;
			spec->set_duration_remaining(remaining);
			if (remaining <= 0) {
				_remove_effect_at_index(i);
				removed_any = true;
			}
		}
	}
	if (removed_any) {
		_update_attribute_current_values();
	}
}

void AbilitySystemComponent::_process_tasks(float p_delta) {
	for (int i = active_tasks.size() - 1; i >= 0; i--) {
		Ref<AbilitySystemTask> task = active_tasks[i];
		if (task->is_finished()) {
			active_tasks.remove_at(i);
			continue;
		}
		task->tick(p_delta);
		if (task->is_finished()) {
			active_tasks.remove_at(i);
		}
	}
}

void AbilitySystemComponent::register_task(Ref<AbilitySystemTask> p_task) {
	ERR_FAIL_COND(p_task.is_null());
	active_tasks.push_back(p_task);
	p_task->activate();
}

void AbilitySystemComponent::_remove_effect_at_index(int p_idx) {
	Ref<AbilitySystemEffectSpec> spec = active_effects[p_idx];
	Ref<AbilitySystemEffect> effect = spec->get_effect();

	// Remove tags
	TypedArray<StringName> granted = effect->get_granted_tags();
	for (int i = 0; i < granted.size(); i++) {
		remove_tag(granted[i]);
	}

	active_effects.remove_at(p_idx);
	emit_signal("effect_removed", spec);
	// Note: Caller is responsible for calling _update_attribute_current_values() if needed
}

void AbilitySystemComponent::_update_cache() {
	character_body_2d = nullptr;
	character_body_3d = nullptr;
	animation_player = nullptr;
	animated_sprite_2d = nullptr;
	animated_sprite_3d = nullptr;
	audio_player_2d = nullptr;
	audio_player_3d = nullptr;
	target_area_2d = nullptr;
	target_area_3d = nullptr;
	sensor_ray_2d = nullptr;
	sensor_ray_3d = nullptr;
	sensor_shape_2d = nullptr;
	sensor_shape_3d = nullptr;
	vfx_particles_2d = nullptr;
	vfx_particles_3d = nullptr;
	spawn_marker_2d = nullptr;
	spawn_marker_3d = nullptr;

	Node *parent = get_parent();
	if (!parent) {
		return;
	}

	character_body_2d = Object::cast_to<CharacterBody2D>(parent);
	character_body_3d = Object::cast_to<CharacterBody3D>(parent);

	for (int i = 0; i < parent->get_child_count(); i++) {
		Node *child = parent->get_child(i);
		if (child == this) {
			continue;
		}

		if (!animation_player) {
			animation_player = Object::cast_to<AnimationPlayer>(child);
		}
		if (!animated_sprite_2d) {
			animated_sprite_2d = Object::cast_to<AnimatedSprite2D>(child);
		}
		if (!animated_sprite_3d) {
			animated_sprite_3d = Object::cast_to<AnimatedSprite3D>(child);
		}
		if (!audio_player_2d) {
			audio_player_2d = Object::cast_to<AudioStreamPlayer2D>(child);
		}
		if (!audio_player_3d) {
			audio_player_3d = Object::cast_to<AudioStreamPlayer3D>(child);
		}
		if (!target_area_2d) {
			target_area_2d = Object::cast_to<Area2D>(child);
		}
		if (!target_area_3d) {
			target_area_3d = Object::cast_to<Area3D>(child);
		}
		if (!sensor_ray_2d) {
			sensor_ray_2d = Object::cast_to<RayCast2D>(child);
		}
		if (!sensor_ray_3d) {
			sensor_ray_3d = Object::cast_to<RayCast3D>(child);
		}
		if (!sensor_shape_2d) {
			sensor_shape_2d = Object::cast_to<ShapeCast2D>(child);
		}
		if (!sensor_shape_3d) {
			sensor_shape_3d = Object::cast_to<ShapeCast3D>(child);
		}
		if (!vfx_particles_2d) {
			vfx_particles_2d = Object::cast_to<GPUParticles2D>(child);
		}
		if (!vfx_particles_3d) {
			vfx_particles_3d = Object::cast_to<GPUParticles3D>(child);
		}
		if (!spawn_marker_2d) {
			spawn_marker_2d = Object::cast_to<Marker2D>(child);
		}
		if (!spawn_marker_3d) {
			spawn_marker_3d = Object::cast_to<Marker3D>(child);
		}
	}
}

void AbilitySystemComponent::give_ability(Ref<AbilitySystemAbility> p_ability, int p_level) {
	ERR_FAIL_COND(p_ability.is_null());
	Ref<AbilitySystemAbilitySpec> spec;
	spec.instantiate();
	spec->init(p_ability, p_level);
	granted_abilities.push_back(spec);
}

void AbilitySystemComponent::apply_ability_container(Ref<AbilitySystemAbilityContainer> p_container, int p_level) {
	ERR_FAIL_COND(p_container.is_null());

	// 1. Grant Abilities
	TypedArray<AbilitySystemAbility> abilities = p_container->get_granted_abilities();
	for (int i = 0; i < abilities.size(); i++) {
		Ref<AbilitySystemAbility> ability = abilities[i];
		if (ability.is_valid()) {
			give_ability(ability, p_level);
		}
	}

	// 2. Apply Innate Effects
	TypedArray<AbilitySystemEffect> effects = p_container->get_innate_effects();
	for (int i = 0; i < effects.size(); i++) {
		Ref<AbilitySystemEffect> effect = effects[i];
		if (effect.is_valid()) {
			Ref<AbilitySystemEffectSpec> spec = make_outgoing_spec(effect, p_level);
			apply_gameplay_effect_spec_to_self(spec);
		}
	}

	// 3. Set Initial Attributes
	Dictionary attributes = p_container->get_initial_attributes();
	Array keys = attributes.keys();
	for (int i = 0; i < keys.size(); i++) {
		StringName key = keys[i];
		float value = attributes[key];
		set_attribute_base_value(key, value);
	}

	// 4. Register Cues
	TypedArray<AbilitySystemCue> cues = p_container->get_cues();
	for (int i = 0; i < cues.size(); i++) {
		Ref<AbilitySystemCue> cue = cues[i];
		if (cue.is_valid()) {
			register_cue_resource(cue);
		}
	}

	// Force update of current values after setting base values
	_update_attribute_current_values();
}

bool AbilitySystemComponent::try_activate_ability_by_tag(const StringName &p_tag) {
	for (int i = 0; i < granted_abilities.size(); i++) {
		Ref<AbilitySystemAbilitySpec> spec = granted_abilities[i];
		if (AbilitySystem::tag_matches(spec->get_ability()->get_ability_tag(), p_tag)) {
			if (spec->get_ability()->can_activate_ability(this)) {
				spec->get_ability()->activate_ability(this);
				emit_signal("ability_activated", spec);
				return true;
			}
		}
	}
	return false;
}

Ref<AbilitySystemEffectSpec> AbilitySystemComponent::make_outgoing_spec(Ref<AbilitySystemEffect> p_effect, float p_level) {
	Ref<AbilitySystemEffectSpec> spec;
	spec.instantiate();
	spec->init(p_effect, p_level);
	spec->set_source_component(this);
	return spec;
}

void AbilitySystemComponent::apply_gameplay_effect_spec_to_self(Ref<AbilitySystemEffectSpec> p_spec) {
	ERR_FAIL_COND(p_spec.is_null());
	Ref<AbilitySystemEffect> effect = p_spec->get_effect();
	ERR_FAIL_COND(effect.is_null());

	p_spec->set_target_component(this);

	if (effect->get_duration_policy() == AbilitySystemEffect::INSTANT) {
		// Instant effects modify Base Value permanently
		for (int i = 0; i < effect->get_modifier_count(); i++) {
			StringName attr = effect->get_modifier_attribute(i);
			AbilitySystemEffect::ModifierOp op = effect->get_modifier_operation(i);
			float mag = p_spec->calculate_modifier_magnitude(i);

			float current = get_attribute_value(attr);
			float next = current;

			switch (op) {
				case AbilitySystemEffect::ADD:
					next += mag;
					break;
				case AbilitySystemEffect::MULTIPLY:
					next *= mag;
					break;
				case AbilitySystemEffect::DIVIDE:
					if (mag != 0) {
						next /= mag;
					}
					break;
				case AbilitySystemEffect::OVERRIDE:
					next = mag;
					break;
			}

			set_attribute_base_value(attr, next);
		}
	} else {
		// Duration/Infinite effects are added to active list and will be part of recalculation
		active_effects.push_back(p_spec);

		// Apply tags
		TypedArray<StringName> granted = effect->get_granted_tags();
		for (int i = 0; i < granted.size(); i++) {
			add_tag(granted[i]);
		}

		_update_attribute_current_values();
	}

	// Trigger Gameplay Cues
	TypedArray<StringName> cues = effect->get_cue_tags();
	for (int i = 0; i < cues.size(); i++) {
		execute_cue(cues[i]);
	}

	emit_signal("effect_applied", p_spec);
}

void AbilitySystemComponent::play_montage(const String &p_name) {
	if (animation_player) {
		animation_player->play(p_name);
	} else if (animated_sprite_2d) {
		animated_sprite_2d->play(p_name);
	} else if (animated_sprite_3d) {
		animated_sprite_3d->play(p_name);
	}
}

bool AbilitySystemComponent::is_montage_playing(const String &p_name) const {
	if (animation_player) {
		return animation_player->is_playing() && animation_player->get_current_animation() == p_name;
	} else if (animated_sprite_2d) {
		return animated_sprite_2d->is_playing() && animated_sprite_2d->get_animation() == StringName(p_name);
	} else if (animated_sprite_3d) {
		return animated_sprite_3d->is_playing() && animated_sprite_3d->get_animation() == StringName(p_name);
	}
	return false;
}

void AbilitySystemComponent::execute_cue(const StringName &p_tag, const Dictionary &p_data) {
	// 1. Emit signal for external managers (VFX/SFX)
	emit_signal("gameplay_event_received", p_tag, p_data);

	// 2. Resolve local Cue resource and dispatch to its virtual method
	Ref<AbilitySystemCue> cue = get_cue_resource(p_tag);
	if (cue.is_valid()) {
		cue->execute(this, p_data);
	}
}

void AbilitySystemComponent::register_cue_resource(Ref<AbilitySystemCue> p_cue) {
	ERR_FAIL_COND(p_cue.is_null());
	StringName tag = p_cue->get_cue_tag();
	if (tag != StringName()) {
		// Replace if already exists or just add
		for (int i = 0; i < registered_cues.size(); i++) {
			if (registered_cues[i]->get_cue_tag() == tag) {
				registered_cues.write[i] = p_cue;
				return;
			}
		}
		registered_cues.push_back(p_cue);
	}
}

void AbilitySystemComponent::unregister_cue_resource(const StringName &p_tag) {
	for (int i = 0; i < registered_cues.size(); i++) {
		if (registered_cues[i]->get_cue_tag() == p_tag) {
			registered_cues.remove_at(i);
			return;
		}
	}
}

Ref<AbilitySystemCue> AbilitySystemComponent::get_cue_resource(const StringName &p_tag) const {
	for (int i = 0; i < registered_cues.size(); i++) {
		if (registered_cues[i]->get_cue_tag() == p_tag) {
			return registered_cues[i];
		}
	}
	return Ref<AbilitySystemCue>();
}

void AbilitySystemComponent::add_tag(const StringName &p_tag) {
	if (!owned_tags->has_tag(p_tag, true)) {
		owned_tags->add_tag(p_tag);
		emit_signal("tag_changed", p_tag, true);
	}
}

void AbilitySystemComponent::remove_tag(const StringName &p_tag) {
	if (owned_tags->has_tag(p_tag, true)) {
		owned_tags->remove_tag(p_tag);
		emit_signal("tag_changed", p_tag, false);
	}
}

bool AbilitySystemComponent::has_tag(const StringName &p_tag) const {
	return owned_tags->has_tag(p_tag);
}

float AbilitySystemComponent::get_attribute_value(const StringName &p_attribute_name) const {
	for (int i = 0; i < attribute_sets.size(); i++) {
		if (attribute_sets[i]->has_attribute(p_attribute_name)) {
			return attribute_sets[i]->get_attribute_current_value(p_attribute_name);
		}
	}
	return 0.0f;
}

void AbilitySystemComponent::set_attribute_base_value(const StringName &p_name, float p_value) {
	for (int i = 0; i < attribute_sets.size(); i++) {
		if (attribute_sets[i]->has_attribute(p_name)) {
			attribute_sets[i]->set_attribute_base_value(p_name, p_value);
			return;
		}
	}
}

void AbilitySystemComponent::add_attribute_set(Ref<AbilitySystemAttributeSet> p_set) {
	ERR_FAIL_COND(p_set.is_null());
	attribute_sets.push_back(p_set);
	// Connect to changed signals
	p_set->connect("attribute_changed", callable_mp(this, &AbilitySystemComponent::_on_attribute_set_attribute_changed));
}

void AbilitySystemComponent::_on_attribute_set_attribute_changed(const StringName &p_name, float p_old_val, float p_new_val) {
	emit_signal("attribute_changed", p_name, p_old_val, p_new_val);
}

void AbilitySystemComponent::_update_attribute_current_values() {
	// 1. Reset all current values to base values
	for (int i = 0; i < attribute_sets.size(); i++) {
		attribute_sets[i]->reset_current_values();
	}

	// 2. Apply modifiers from all active effects in order
	for (int i = 0; i < active_effects.size(); i++) {
		Ref<AbilitySystemEffectSpec> spec = active_effects[i];
		Ref<AbilitySystemEffect> effect = spec->get_effect();

		for (int j = 0; j < effect->get_modifier_count(); j++) {
			StringName attr_name = effect->get_modifier_attribute(j);

			// Find the set that owns this attribute
			for (int k = 0; k < attribute_sets.size(); k++) {
				if (attribute_sets[k]->has_attribute(attr_name)) {
					float current = attribute_sets[k]->get_attribute_current_value(attr_name);
					float mag = spec->calculate_modifier_magnitude(j);
					float next = current;

					switch (effect->get_modifier_operation(j)) {
						case AbilitySystemEffect::ADD:
							next += mag;
							break;
						case AbilitySystemEffect::MULTIPLY:
							next *= mag;
							break;
						case AbilitySystemEffect::DIVIDE:
							if (mag != 0) {
								next /= mag;
							}
							break;
						case AbilitySystemEffect::OVERRIDE:
							next = mag;
							break;
					}

					attribute_sets[k]->set_attribute_current_value(attr_name, next);
					break;
				}
			}
		}
	}
}

Ref<AbilitySystemTagContainer> AbilitySystemComponent::get_owned_tags() const {
	return owned_tags;
}

void AbilitySystemComponent::set_ability_container(Ref<AbilitySystemAbilityContainer> p_container) {
	ability_container = p_container;
	update_configuration_warnings();
}

Ref<AbilitySystemAbilityContainer> AbilitySystemComponent::get_ability_container() const {
	return ability_container;
}

Dictionary AbilitySystemComponent::get_net_state() const {
	Dictionary d;
	Dictionary attrs;
	for (int i = 0; i < attribute_sets.size(); i++) {
		TypedArray<StringName> list = attribute_sets[i]->get_attribute_list();
		for (int j = 0; j < list.size(); j++) {
			attrs[list[j]] = attribute_sets[i]->get_attribute_base_value(list[j]);
		}
	}
	d["attributes"] = attrs;
	d["tags"] = owned_tags->get_all_tags();
	return d;
}

void AbilitySystemComponent::apply_net_state(const Dictionary &p_state) {
	if (p_state.has("attributes")) {
		Dictionary attrs = p_state["attributes"];
		Array keys = attrs.keys();
		for (int i = 0; i < keys.size(); i++) {
			set_attribute_base_value(keys[i], attrs[keys[i]]);
		}
	}
	if (p_state.has("tags")) {
		TypedArray<StringName> tags = p_state["tags"];
		owned_tags->clear(); // Reset tags
		for (int i = 0; i < tags.size(); i++) {
			add_tag(tags[i]);
		}
	}
}

PackedStringArray AbilitySystemComponent::get_configuration_warnings() const {
	PackedStringArray warnings = Node::get_configuration_warnings();

	Node *parent = get_parent();
	if (parent) {
		bool is_valid_parent = Object::cast_to<CharacterBody2D>(parent) || Object::cast_to<CharacterBody3D>(parent);
		if (!is_valid_parent) {
			warnings.push_back("AbilitySystemComponent must be a child of CharacterBody2D or CharacterBody3D.");
		}
	} else {
		warnings.push_back("AbilitySystemComponent requires a CharacterBody2D or CharacterBody3D parent.");
	}

	if (ability_container.is_null()) {
		warnings.push_back("AbilitySystemComponent requires an AbilitySystemAbilityContainer to be valid.");
	}

	return warnings;
}

AbilitySystemComponent::AbilitySystemComponent() {
	owned_tags.instantiate();
}

AbilitySystemComponent::~AbilitySystemComponent() {
}
