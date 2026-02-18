/**************************************************************************/
/*  gameplay_ability.h                                                    */
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
#include "modules/gas/core/gameplay_tag_container.h"

class AbilitySystemComponent;
class GameplayCue;

class GameplayAbility : public Resource {
	GDCLASS(GameplayAbility, Resource);

	Ref<GameplayTagContainer> ability_tags;
	Ref<GameplayTagContainer> cancel_tags;
	Ref<GameplayTagContainer> block_tags;
	Ref<GameplayTagContainer> activation_required_tags;
	Ref<GameplayTagContainer> activation_blocked_tags;

	Dictionary costs; // Attribute name -> cost value
	real_t cooldown_duration = 0.0;
	StringName cooldown_tag;


	Ref<GameplayCue> cue_resource; // Visual/Audio feedback

	AbilitySystemComponent *asc = nullptr;

protected:
	static void _bind_methods();

	GDVIRTUAL1RC(bool, _can_activate, Object *)
	GDVIRTUAL0(_activate_ability)

public:
	bool can_activate(AbilitySystemComponent *p_asc) const;
	void activate_ability(AbilitySystemComponent *p_asc);
	void end_ability();

	void set_ability_tags(const Ref<GameplayTagContainer> &p_tags) { ability_tags = p_tags; }
	Ref<GameplayTagContainer> get_ability_tags() const { return ability_tags; }

	void set_cancel_tags(const Ref<GameplayTagContainer> &p_tags) { cancel_tags = p_tags; }
	Ref<GameplayTagContainer> get_cancel_tags() const { return cancel_tags; }

	void set_block_tags(const Ref<GameplayTagContainer> &p_tags) { block_tags = p_tags; }
	Ref<GameplayTagContainer> get_block_tags() const { return block_tags; }

	void set_activation_required_tags(const Ref<GameplayTagContainer> &p_tags) { activation_required_tags = p_tags; }
	Ref<GameplayTagContainer> get_activation_required_tags() const { return activation_required_tags; }

	void set_activation_blocked_tags(const Ref<GameplayTagContainer> &p_tags) { activation_blocked_tags = p_tags; }
	Ref<GameplayTagContainer> get_activation_blocked_tags() const { return activation_blocked_tags; }

	void set_costs(const Dictionary &p_costs) { costs = p_costs; }
	Dictionary get_costs() const { return costs; }

	void set_cooldown_duration(real_t p_duration) { cooldown_duration = p_duration; }
	real_t get_cooldown_duration() const { return cooldown_duration; }

	void set_cooldown_tag(const StringName &p_tag) { cooldown_tag = p_tag; }
	StringName get_cooldown_tag() const { return cooldown_tag; }

	void set_cue_resource(const Ref<GameplayCue> &p_cue) { cue_resource = p_cue; }
	Ref<GameplayCue> get_cue_resource() const { return cue_resource; }
};
