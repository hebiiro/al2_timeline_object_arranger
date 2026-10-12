#pragma once

namespace apn::timeline_object_arranger
{
	//
	// このクラスはBPMずれを修正します。
	//
	struct fix_bpm_t : action_t
	{
		//
		// 指定されたフレーム番号を修正して返します。
		//
		int32_t fix_frame(int32_t frame)
		{
			auto right_frame = frame_per_time;
			auto wrong_frame = ceil(right_frame);

			return (int32_t)ceil(frame * right_frame / wrong_frame);
		}

		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// カーソルフレームの位置を取得します。
			auto current_frame = edit_info.frame;

			// カーソルフレームの修正位置を算出します。
			auto fixed_current_frame = fix_frame(current_frame);

			// カーソルフレームのズレ幅を算出します。
			auto diff_current_frame = current_frame - fixed_current_frame;

			// 選択ノードを走査します。
			for (auto& node : selection)
			{
#if 0
				// オブジェクトの位置を整数で取得します。
				auto object_layer_frame = edit->get_object_layer_frame(node->object_handle);

				auto frame_begin = object_layer_frame.start;
				auto frame_end = object_layer_frame.end;
#else
				// オブジェクトの位置を取得します。
				auto frame_begin = time_to_frame(node->before.time_start);
				auto frame_end = time_to_frame(node->before.time_end);
#endif
				// カーソルフレームを使用する場合は
				if (hive.flag_use_current_frame)
				{
					// カーソルフレームのズレ幅を加算します。
					frame_begin += diff_current_frame;
					frame_end += diff_current_frame;
				}

				// オブジェクトの位置を修正します。
				node->after.time_start = frame_to_time(fix_frame(frame_begin));
				node->after.time_end = frame_to_time(fix_frame(frame_end + 1) - 1);
			}

			return __super::execute();
		}
	};
}
