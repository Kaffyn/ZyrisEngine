/**************************************************************************/
/*  ability_system_ability_container.cpp                                  */
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

#include "ability_system_ability_container.h"

#include "modules/ability_system/resources/ability_system_ability.h"
#include "modules/ability_system/resources/ability_system_cue.h"
#include "modules/ability_system/resources/ability_system_effect.h"

void AbilitySystemAbilityContainer::_bind_methods() {
	// This class acts as a data-driven Archetype/Preset.
	// Designers use it to define specific configs (Player, Zombie, etc.) without touching code.
	ClassDB::bind_method(D_METHOD("set_granted_abilities", "abilities"), &AbilitySystemAbilityContainer::set_granted_abilities);
	ClassDB::bind_method(D_METHOD("get_granted_abilities"), &AbilitySystemAbilityContainer::get_granted_abilities);

	ClassDB::bind_method(D_METHOD("set_innate_effects", "effects"), &AbilitySystemAbilityContainer::set_innate_effects);
	ClassDB::bind_method(D_METHOD("get_innate_effects"), &AbilitySystemAbilityContainer::get_innate_effects);

	ClassDB::bind_method(D_METHOD("set_innate_tags", "tags"), &AbilitySystemAbilityContainer::set_innate_tags);
	ClassDB::bind_method(D_METHOD("get_innate_tags"), &AbilitySystemAbilityContainer::get_innate_tags);

	ClassDB::bind_method(D_METHOD("set_initial_attributes", "attributes"), &AbilitySystemAbilityContainer::set_initial_attributes);
	ClassDB::bind_method(D_METHOD("get_initial_attributes"), &AbilitySystemAbilityContainer::get_initial_attributes);

	ClassDB::bind_method(D_METHOD("set_cues", "cues"), &AbilitySystemAbilityContainer::set_cues);
	ClassDB::bind_method(D_METHOD("get_cues"), &AbilitySystemAbilityContainer::get_cues);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "granted_abilities", PROPERTY_HINT_TYPE_STRING, vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemAbility")), "set_granted_abilities", "get_granted_abilities");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "innate_effects", PROPERTY_HINT_TYPE_STRING, vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemEffect")), "set_innate_effects", "get_innate_effects");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "innate_tags", PROPERTY_HINT_TYPE_STRING, vformat("%s/%s:%s", Variant::STRING_NAME, PROPERTY_HINT_NONE, "")), "set_innate_tags", "get_innate_tags");
	ADD_PROPERTY(PropertyInfo(Variant::DICTIONARY, "initial_attributes", PROPERTY_HINT_TYPE_STRING, vformat("%d/%d:%s;%d/%d:%s", Variant::STRING_NAME, PROPERTY_HINT_NONE, "", Variant::FLOAT, PROPERTY_HINT_NONE, "")), "set_initial_attributes", "get_initial_attributes");
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "cues", PROPERTY_HINT_TYPE_STRING, vformat("%s/%s:%s", Variant::OBJECT, PROPERTY_HINT_RESOURCE_TYPE, "AbilitySystemCue")), "set_cues", "get_cues");
}

AbilitySystemAbilityContainer::AbilitySystemAbilityContainer() {
}

AbilitySystemAbilityContainer::~AbilitySystemAbilityContainer() {
}
