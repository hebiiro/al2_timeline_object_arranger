#pragma once

namespace apn::timeline_object_arranger
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct config_io_t : config_io_base2_t
	{
		//
		// ノードからコンフィグを読み込みます。
		//
		virtual BOOL read_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			read_bool(root, "flag_use_current_frame", hive.flag_use_current_frame);
			read_int(root, "sub_time_mode", hive.time.mode);
			read_string(root, "shift_time", hive.time.elements[hive.time.c_index.c_shift].sec);
			read_string(root, "align_time", hive.time.elements[hive.time.c_index.c_align].sec);
			read_string(root, "stretch_time", hive.time.elements[hive.time.c_index.c_stretch].sec);
			read_string(root, "relative_space_time", hive.time.elements[hive.time.c_index.c_relative_space].sec);
			read_string(root, "absolute_space_time", hive.time.elements[hive.time.c_index.c_absolute_space].sec);
			read_int(root, "move_vert_layer", hive.move_vert_layer);

			return __super::read_node(root);
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		virtual BOOL write_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			write_bool(root, "flag_use_current_frame", hive.flag_use_current_frame);
			write_int(root, "sub_time_mode", hive.time.mode);
			write_string(root, "shift_time", hive.time.elements[hive.time.c_index.c_shift].sec);
			write_string(root, "align_time", hive.time.elements[hive.time.c_index.c_align].sec);
			write_string(root, "stretch_time", hive.time.elements[hive.time.c_index.c_stretch].sec);
			write_string(root, "relative_space_time", hive.time.elements[hive.time.c_index.c_relative_space].sec);
			write_string(root, "absolute_space_time", hive.time.elements[hive.time.c_index.c_absolute_space].sec);
			write_int(root, "move_vert_layer", hive.move_vert_layer);

			return __super::write_node(root);
		}
	} config_io;
}
