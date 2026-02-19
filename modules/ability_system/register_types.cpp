/**************************************************************************/
/*  register_types.cpp                                                    */
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

#include "register_types.h"

#include "core/config/engine.h"
#include "core/object/class_db.h"
#include "modules/ability_system/core/ability_system.h"
#include "modules/ability_system/core/ability_system_ability_spec.h"
#include "modules/ability_system/core/ability_system_cue_spec.h"
#include "modules/ability_system/core/ability_system_effect_spec.h"
#include "modules/ability_system/core/ability_system_magnitude_calculation.h"
#include "modules/ability_system/core/ability_system_target_data.h"
#include "modules/ability_system/core/ability_system_task.h"
#include "modules/ability_system/resources/ability_system_ability.h"
#include "modules/ability_system/resources/ability_system_ability_container.h"
#include "modules/ability_system/resources/ability_system_attribute_set.h"
#include "modules/ability_system/resources/ability_system_cue.h"
#include "modules/ability_system/resources/ability_system_effect.h"
#include "modules/ability_system/resources/ability_system_tag.h"
#include "modules/ability_system/resources/ability_system_tag_container.h"
#include "modules/ability_system/scene/ability_system_component.h"

#ifdef TOOLS_ENABLED
#include "editor/editor_node.h"
#include "modules/ability_system/editor/ability_system_editor_plugin.h"
#endif

static AbilitySystem *ability_system_singleton = nullptr;

void initialize_ability_system_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		ClassDB::register_class<AbilitySystem>();
		ClassDB::register_class<AbilitySystemTag>();
		ClassDB::register_class<AbilitySystemTagContainer>();
		ClassDB::register_class<AbilitySystemAttributeSet>();
		ClassDB::register_class<AbilitySystemAbilityContainer>();
		ClassDB::register_class<AbilitySystemEffect>();
		ClassDB::register_class<AbilitySystemEffectSpec>();
		ClassDB::register_class<AbilitySystemAbility>();
		ClassDB::register_class<AbilitySystemAbilitySpec>();
		ClassDB::register_class<AbilitySystemComponent>();
		ClassDB::register_class<AbilitySystemCue>();
		ClassDB::register_class<AbilitySystemCueSpec>();
		ClassDB::register_class<AbilitySystemMagnitudeCalculation>();
		ClassDB::register_class<AbilitySystemTargetData>();
		ClassDB::register_class<AbilitySystemTask>();

		ability_system_singleton = memnew(AbilitySystem);
		Engine::get_singleton()->add_singleton(Engine::Singleton("AbilitySystem", AbilitySystem::get_singleton()));
	}
#ifdef TOOLS_ENABLED
	if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
		ClassDB::register_class<AbilitySystemEditorPlugin>();
		EditorPlugins::add_by_type<AbilitySystemEditorPlugin>();
	}
#endif
}

void uninitialize_ability_system_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		if (ability_system_singleton) {
			memdelete(ability_system_singleton);
		}
	}
}
