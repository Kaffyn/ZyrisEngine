/**************************************************************************/
/*  shake_profile.cpp                                                     */
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

#include "shake_profile.h"

void ShakeProfile::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_frequency", "frequency"), &ShakeProfile::set_frequency);
	ClassDB::bind_method(D_METHOD("get_frequency"), &ShakeProfile::get_frequency);

	ClassDB::bind_method(D_METHOD("set_amplitude", "amplitude"), &ShakeProfile::set_amplitude);
	ClassDB::bind_method(D_METHOD("get_amplitude"), &ShakeProfile::get_amplitude);

	ClassDB::bind_method(D_METHOD("set_decay", "decay"), &ShakeProfile::set_decay);
	ClassDB::bind_method(D_METHOD("get_decay"), &ShakeProfile::get_decay);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "frequency", PROPERTY_HINT_RANGE, "0.1,100.0,0.1"), "set_frequency", "get_frequency");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "amplitude", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_amplitude", "get_amplitude");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "decay", PROPERTY_HINT_RANGE, "0.0,10.0,0.01"), "set_decay", "get_decay");
}

void ShakeProfile::set_frequency(float p_frequency) {
	frequency = p_frequency;
}

float ShakeProfile::get_frequency() const {
	return frequency;
}

void ShakeProfile::set_amplitude(float p_amplitude) {
	amplitude = p_amplitude;
}

float ShakeProfile::get_amplitude() const {
	return amplitude;
}

void ShakeProfile::set_decay(float p_decay) {
	decay = p_decay;
}

float ShakeProfile::get_decay() const {
	return decay;
}

ShakeProfile::ShakeProfile() {
}
