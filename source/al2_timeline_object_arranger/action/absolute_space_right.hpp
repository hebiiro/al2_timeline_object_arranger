#pragma once

namespace apn::timeline_object_arranger
{
	//
	// このクラスは指定された間隔(絶対値)が空くように
	// タイムラインアイテムを右に向かって配置します。
	//
	struct absolute_space_right_t : action_t
	{
		//
		// アクションを実行します。
		//
		virtual void execute() override
		{
			MY_TRACE_FUNC("");

			// ずらす量(時間)を取得します。
			auto shift_time = str_to_time(hive.time.elements[hive.time.c_index.c_absolute_space].sec);
			MY_TRACE_REAL(shift_time);

			// 基準位置(時間)を取得します。
			auto base_time_start = get_base_time_start(bound.time_start);
			MY_TRACE_REAL(base_time_start);

			// レイヤーを走査します。
			for (auto& pair : layers)
			{
				// レイヤーを取得します。
				auto& layer = pair.second;

				// 移動幅です。
				auto amplitude = 0;

				// 追加の移動幅です。
				auto add = 0.0;

				// ノードを走査します。
				for (auto& node : layer->nodes)
				{
					// 選択ノードの場合は
					if (is_selected(node))
					{
						MY_TRACE_INT(amplitude);
						MY_TRACE_REAL(add);

						// 位置をずらします。
						auto range = node->before.time_end - node->before.time_start;
						node->after.time_start = base_time_start + shift_time * amplitude + add;
						node->after.time_end = base_time_start + shift_time * amplitude + add + range;

						MY_TRACE_REAL(node->after.time_start);
						MY_TRACE_REAL(node->after.time_end);

						// 移動幅を増やします。
						amplitude++;

						// 追加の移動幅を増やします。
						add += to_time_start(range);
					}
				}
			}

			return __super::execute();
		}
	};
}
