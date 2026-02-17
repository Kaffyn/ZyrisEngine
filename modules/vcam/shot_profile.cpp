/**************************************************************************/
/*  shot_profile.cpp                                                      */
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

#include "shot_profile.h"

void ShotProfile::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_focal_length", "focal_length"), &ShotProfile::set_focal_length);
	ClassDB::bind_method(D_METHOD("get_focal_length"), &ShotProfile::get_focal_length);

	ClassDB::bind_method(D_METHOD("set_aperture", "aperture"), &ShotProfile::set_aperture);
	ClassDB::bind_method(D_METHOD("get_aperture"), &ShotProfile::get_aperture);

	ClassDB::bind_method(D_METHOD("set_composition_rule", "rule"), &ShotProfile::set_composition_rule);
	ClassDB::bind_method(D_METHOD("get_composition_rule"), &ShotProfile::get_composition_rule);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "focal_length", PROPERTY_HINT_RANGE, "1.0,1000.0,0.1"), "set_focal_length", "get_focal_length");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "aperture", PROPERTY_HINT_RANGE, "0.1,64.0,0.1"), "set_aperture", "get_aperture");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "composition_rule", PROPERTY_HINT_ENUM, "None,Rule of Thirds,Golden Ratio,Lead Room"), "set_composition_rule", "get_composition_rule");

	BIND_ENUM_CONSTANT(RULE_NONE);
	BIND_ENUM_CONSTANT(RULE_OF_THIRDS);
	BIND_ENUM_CONSTANT(RULE_GOLDEN_RATIO);
	BIND_ENUM_CONSTANT(RULE_LEAD_ROOM);
}

void ShotProfile::set_focal_length(float p_focal_length) {
	focal_length = p_focal_length;
}

float ShotProfile::get_focal_length() const {
	return focal_length;
}

void ShotProfile::set_aperture(float p_aperture) {
	aperture = p_aperture;
}

float ShotProfile::get_aperture() const {
	return aperture;
}

void ShotProfile::set_composition_rule(CompositionRule p_rule) {
	composition_rule = p_rule;
}

ShotProfile::CompositionRule ShotProfile::get_composition_rule() const {
	return composition_rule;
}

ShotProfile::ShotProfile() {
}
