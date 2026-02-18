/**************************************************************************/
/*  ability_system_cue.h                                                  */
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
#include "core/object/gdvirtual.gen.inc"

class AbilitySystemCue : public Resource {
	GDCLASS(AbilitySystemCue, Resource);

public:
	enum CueEventType {
		ON_EXECUTE, // Instant
		ON_ACTIVE, // Continuous while effect/ability is active
		ON_REMOVE // When ending
	};

protected:
	static void _bind_methods();

	GDVIRTUAL2(_on_execute, Object *, Dictionary)
	GDVIRTUAL2(_on_active, Object *, Dictionary)
	GDVIRTUAL2(_on_remove, Object *, Dictionary)

private:
	CueEventType event_type = ON_EXECUTE;
	StringName cue_tag;

public:
	void set_event_type(CueEventType p_type) { event_type = p_type; }
	CueEventType get_event_type() const { return event_type; }

	void set_cue_tag(const StringName &p_tag) { cue_tag = p_tag; }
	StringName get_cue_tag() const { return cue_tag; }

	// Called by AbilitySystemComponent to dispatch to the correct virtual
	void execute(Object *p_owner, const Dictionary &p_data) {
		switch (event_type) {
			case ON_EXECUTE:
				GDVIRTUAL_CALL(_on_execute, p_owner, p_data);
				break;
			case ON_ACTIVE:
				GDVIRTUAL_CALL(_on_active, p_owner, p_data);
				break;
			case ON_REMOVE:
				GDVIRTUAL_CALL(_on_remove, p_owner, p_data);
				break;
		}
	}

	AbilitySystemCue();
	~AbilitySystemCue();
};

VARIANT_ENUM_CAST(AbilitySystemCue::CueEventType);
