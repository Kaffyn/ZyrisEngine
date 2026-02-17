/**************************************************************************/
/*  vcam_server.h                                                         */
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

#include "core/object/class_db.h"
#include "core/object/ref_counted.h"
#include "core/templates/vector.h"

class vCam3D;
class vCam2D;

class vCamServer : public Object {
	GDCLASS(vCamServer, Object);

	static vCamServer *singleton;

	Vector<vCam3D *> vcams_3d;
	Vector<vCam2D *> vcams_2d;

	vCam3D *active_vcam_3d = nullptr;
	vCam2D *active_vcam_2d = nullptr;

	float trauma = 0.0f;
	float trauma_decay = 0.8f;
	float time_passed = 0.0f;

	void _update_arbitration();
	void _update_trauma(float p_delta);

protected:
	static void _bind_methods();

public:
	static vCamServer *get_singleton() { return singleton; }

	void update(float p_delta);

	vCam3D *get_active_vcam_3d() const { return active_vcam_3d; }
	vCam2D *get_active_vcam_2d() const { return active_vcam_2d; }

	void add_trauma(float p_amount);
	void set_trauma(float p_amount);
	float get_trauma() const;

	Vector3 get_shake_offset_3d() const;
	float get_shake_offset_2d() const;

	void register_vcam_3d(vCam3D *p_vcam);
	void unregister_vcam_3d(vCam3D *p_vcam);

	void register_vcam_2d(vCam2D *p_vcam);
	void unregister_vcam_2d(vCam2D *p_vcam);

	vCamServer();
	~vCamServer();
};
