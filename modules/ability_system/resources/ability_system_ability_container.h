/**************************************************************************/
/*  ability_system_ability_container.h                                    */
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
#include "core/variant/typed_array.h"

class AbilitySystemAbility;
class AbilitySystemEffect;
class AbilitySystemCue;

/**
 * AbilitySystemAbilityContainer
 *
 * Acts as an Archetype or Preset for actors (Player, NPCs, Enemies).
 * This is a designer-facing Resource used to define a collection of abilities,
 * innate effects, and initial attributes in a single reusable asset.
 */
class AbilitySystemAbilityContainer : public Resource {
	GDCLASS(AbilitySystemAbilityContainer, Resource);

private:
	TypedArray<AbilitySystemAbility> granted_abilities;
	TypedArray<AbilitySystemEffect> innate_effects;
	TypedArray<StringName> innate_tags;
	Dictionary initial_attributes;
	TypedArray<AbilitySystemCue> cues;

protected:
	static void _bind_methods();

public:
	void set_granted_abilities(const TypedArray<AbilitySystemAbility> &p_abilities) { granted_abilities = p_abilities; }
	TypedArray<AbilitySystemAbility> get_granted_abilities() const { return granted_abilities; }

	void set_innate_effects(const TypedArray<AbilitySystemEffect> &p_effects) { innate_effects = p_effects; }
	TypedArray<AbilitySystemEffect> get_innate_effects() const { return innate_effects; }

	void set_innate_tags(const TypedArray<StringName> &p_tags) { innate_tags = p_tags; }
	TypedArray<StringName> get_innate_tags() const { return innate_tags; }

	void set_initial_attributes(const Dictionary &p_attributes) { initial_attributes = p_attributes; }
	Dictionary get_initial_attributes() const { return initial_attributes; }

	void set_cues(const TypedArray<AbilitySystemCue> &p_cues) { cues = p_cues; }
	TypedArray<AbilitySystemCue> get_cues() const { return cues; }

	AbilitySystemAbilityContainer();
	~AbilitySystemAbilityContainer();
};
