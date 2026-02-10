/**************************************************************************/
/*  save_server.h                                                         */
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
#include "core/object/class_db.h"
#include "core/os/mutex.h"
#include "core/os/semaphore.h"
#include "core/os/thread.h"
#include "core/templates/list.h"
#include "core/variant/dictionary.h"
#include "core/variant/typed_array.h"

class Node;
class Snapshot;

class SaveServer : public Object {
	GDCLASS(SaveServer, Object);

public:
	enum SaveFormat {
		FORMAT_TEXT,
		FORMAT_BINARY
	};

	enum SaveResult {
		OK,
		ERR_FILE_LOCKED,
		ERR_ENCRYPTION,
		ERR_DISK_FULL,
		ERR_INVALID_DATA,
		ERR_CHECKSUM_MISMATCH,
		ERR_VERSION_MISMATCH
	};

	enum IntegrityCheckLevel {
		INTEGRITY_NONE,
		INTEGRITY_SIGNATURE,
		INTEGRITY_STRICT
	};

private:
	enum TaskType {
		TASK_SAVE,
		TASK_LOAD
	};

	struct SaveTask {
		TaskType type = TASK_SAVE;
		String slot_name;
		Dictionary data; // In: Snapshot to save, Out: Loaded snapshot
		Dictionary metadata;
		Ref<Resource> thumbnail;
		SaveFormat format;
		String encryption_key;
		bool compression_enabled;

		// For Async Load
		ObjectID target_node_id;
		Callable user_callback;
	};

	static SaveServer *singleton;

	// Threading
	Thread save_thread;
	Mutex mutex;
	Semaphore semaphore;
	SafeFlag exit_thread;
	List<SaveTask> queue;

	// Configuration
	SaveFormat current_format = FORMAT_TEXT;
	String encryption_key;
	bool compression_enabled = true;
	bool backup_enabled = true;
	IntegrityCheckLevel integrity_level = INTEGRITY_SIGNATURE;
	String save_path = "user://saves/";

	struct Migration {
		String from;
		String to;
		Callable callback;
	};
	List<Migration> migrations;

	// ... threading ...
	static void _save_thread_func(void *p_userdata);
	void _process_queue();
	void _finish_load_async(ObjectID p_node_id, const Dictionary &p_data, const Callable &p_callback);

	Error _save_to_disk(const SaveTask &p_task);
	Ref<Snapshot> _read_snapshot_from_disk(const String &p_slot_name);
	Dictionary _load_from_disk(const String &p_slot_name);

	String _calculate_checksum(const Dictionary &p_data);
	void _apply_migrations(Ref<Snapshot> p_snapshot);

protected:
	static void _bind_methods();

public:
	static SaveServer *get_singleton();
	static void register_settings();

	// Slot Management
	void save_slot(const String &p_slot_name, const Dictionary &p_data, bool p_async = true, const Dictionary &p_metadata = Dictionary(), Ref<Resource> p_thumbnail = Ref<Resource>());
	Dictionary load_slot(const String &p_slot_name);
	bool has_slot(const String &p_slot_name) const;
	void delete_slot(const String &p_slot_name);

	// Snapshot API (Node-based)
	bool save_snapshot(Node *p_root, const String &p_slot_name, bool p_async = true, const TypedArray<StringName> &p_tags = TypedArray<StringName>(), const Dictionary &p_metadata = Dictionary(), Ref<Resource> p_thumbnail = Ref<Resource>());
	void load_snapshot(Node *p_root, const String &p_slot_name, const Callable &p_callback = Callable());
	bool has_snapshot(const String &p_slot_name) const { return has_slot(p_slot_name); }

	// Migrations
	void register_migration(const String &p_from, const String &p_to, const Callable &p_callback);

	// Configuration
	void set_save_format(SaveFormat p_format);
	SaveFormat get_save_format() const;

	void set_encryption_key(const String &p_key);
	String get_encryption_key() const;

	void set_compression_enabled(bool p_enabled);
	bool is_compression_enabled() const;

	void set_save_path(const String &p_path);
	String get_save_path() const;

	void set_backup_enabled(bool p_enabled);
	bool is_backup_enabled() const;

	void set_integrity_check_level(IntegrityCheckLevel p_level);
	IntegrityCheckLevel get_integrity_check_level() const;

	SaveServer();
	~SaveServer();
};

VARIANT_ENUM_CAST(SaveServer::SaveFormat);
VARIANT_ENUM_CAST(SaveServer::SaveResult);
VARIANT_ENUM_CAST(SaveServer::IntegrityCheckLevel);
