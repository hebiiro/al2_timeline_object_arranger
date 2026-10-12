#pragma once

namespace apn::timeline_object_arranger
{
	//
	// このクラスは現在位置の近くにある中間点を削除します。
	//
	struct erase_midpt_t : action_t
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// カーソルフレームの位置を取得します。
			auto current_frame = edit_info.frame;

			// 選択ノードを走査します。
			for (auto& node : selection)
			{
				// オブジェクトを取得します。
				auto object_handle = node->object_handle;

				// 中間点の数を取得します。
				auto nb_sections = edit->get_object_section_num(object_handle);

				// 中間点が存在しない場合は何もしません。
				if (nb_sections < 2) continue;

				// 最寄りの中間点です。
				auto nearest_section = 0;

				// 最小距離です。
				auto min_distance = 0;

				// 中間点を走査します。
				for (decltype(nb_sections) i = 1; i < nb_sections; i++)
				{
					// 中間点のフレーム位置を取得します。
					auto section_frame = edit->get_object_section_frame(object_handle, i);

					// カーソルフレームと中間点との距離を算出します。
					auto distance = abs(current_frame - section_frame);

					// 最初に処理される中間点、または距離が最小より小さい場合は
					if (nearest_section == 0 || distance < min_distance)
					{
						// この中間点を最寄りの中間点に設定します。
						nearest_section = i;
						min_distance = distance;
					}
				}

				// 最寄りの中間点を削除します。
				edit->delete_object_section(object_handle, nearest_section);
			}
		}
	};
}
