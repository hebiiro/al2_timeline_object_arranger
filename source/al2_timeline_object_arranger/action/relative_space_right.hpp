#pragma once

namespace apn::timeline_object_arranger
{
	//
	// このクラスは指定された間隔(相対値)が空くように
	// タイムラインアイテムを右に移動します。
	//
	struct relative_space_right_t : action_t
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// ずらす量(時間)を取得します。
			auto shift_time = str_to_time(hive.time.elements[hive.time.c_index.c_relative_space].sec);
			MY_TRACE_REAL(shift_time);

			// 基準位置(時間)を取得します。
			auto base_time_end = get_base_time_end(bound.time_end);
			MY_TRACE_REAL(base_time_end);

			// レイヤーを走査します。
			for (auto& pair : layers)
			{
				// レイヤーを取得します。
				auto& layer = pair.second;
				MY_TRACE_INT(layer->layer_set);

				// 移動幅です。
				auto amplitude = 0;

				// ノードを走査します。
				for (auto& node : layer->nodes)
				{
					// 選択ノードの場合は
					if (is_selected(node))
					{
						MY_TRACE_INT(amplitude);

						// 位置をずらします。
						node->after.time_start = node->before.time_start + shift_time * amplitude;
						node->after.time_end = node->before.time_end + shift_time * amplitude;

						// 移動幅を増やします。
						amplitude++;
					}
				}
			}

			return __super::execute();
		}
	};
}
