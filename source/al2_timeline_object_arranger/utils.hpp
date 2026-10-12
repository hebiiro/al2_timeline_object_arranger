#pragma once

namespace apn::timeline_object_arranger
{
	//
	// このクラスは各種計算を実行します。
	//
	struct utils_t
	{
		//
		// イプシロンです。
		//
		inline static constexpr auto k_epsilon = 0.0001;

		//
		// 計算に必要な変数です。
		// コンストラクタで初期化されます。
		//
		EDIT_INFO edit_info = {};
		int fps = {};
		double bpm = {};
		double frame_per_time = {};

		//
		// コンストラクタです。
		//
		utils_t()
		{
			MY_TRACE_FUNC("");

			// aviutl2の編集情報を取得します。
			editor.handle->get_edit_info(&edit_info, sizeof(edit_info));

			// フレームレートを取得します。
			if (edit_info.rate != 0 && edit_info.scale != 0)
				fps = edit_info.rate / edit_info.scale;
			MY_TRACE_INT(fps);

			// BPMを取得します。
			bpm = edit_info.grid_bpm_tempo;
			MY_TRACE_REAL(bpm);

			// 1区間当たりのフレーム数を取得します。
			if (bpm != 0)
				frame_per_time = fps / (bpm / 60.0);
			else
				frame_per_time = 0;
			MY_TRACE_REAL(frame_per_time);
		}

		//
		// 浮動小数点数を整数に変換して返します。
		//
		inline static int to_int(double x)
		{
			return (int)(x + k_epsilon);
		}

		//
		// 指定された文字列を時間に変換して返します。
		//
		inline static double str_to_time(const std::wstring& str)
		{
			std::wstring s(str);

			auto sep1 = s.find(L':');
			if (sep1 == s.npos) {
				return
					wcstod(&s[0], nullptr); // 秒
			}
			s[sep1] = L'\0';

			auto sep2 = s.find(L':', sep1 + 1);
			if (sep2 == s.npos) {
				return
					wcstod(&s[0], nullptr) * 60 + // 分
					wcstod(&s[sep1 + 1], nullptr); // 秒
			}
			s[sep2] = L'\0';

			return
				wcstod(&s[0], nullptr) * 60 * 60 + // 時
				wcstod(&s[sep1 + 1], nullptr) * 60 + // 分
				wcstod(&s[sep2 + 1], nullptr); // 秒
		}

		//
		// 指定された時間を文字列に変換して返します。
		//
		inline static std::wstring time_to_str(double time)
		{
			return my::format(L"{/02d}:{/02d}:{/05.2f}",
				to_int(time) / 60 / 60, // 時
				to_int(time) / 60 % 60, // 分
				std::fmod(time, 60)); // 秒
		}

		//
		// 指定された時間を文字列に変換して返します。
		//
		inline static std::wstring time_to_str(double time_start, double time_end)
		{
			return my::format(L"{/}-{/}", time_to_str(time_start), time_to_str(time_end));
		}

		//
		// フレームを時間に変換して返します。
		//
		double frame_to_time(int frame) const
		{
			return (double)frame / fps;
		}

		//
		// 時間をフレームに変換して返します。
		//
		int time_to_frame(double time) const
		{
			return to_int(time * fps);
		}

		//
		// BPMを時間に変換して返します。
		//
		double bpm_to_time(int bpm) const
		{
			return 60.0 * bpm / this->bpm;
		}

		//
		// 時間を開始位置(時間)に変換して返します。
		//
		double to_time_start(double time) const
		{
			return time + frame_to_time(1);
		}

		//
		// 時間を終了位置(時間)に変換して返します。
		//
		double to_time_end(double time) const
		{
			return time - frame_to_time(1);
		}

		//
		// 基準開始位置(時間)を返します。
		//
		double get_base_time_start(double time)
		{
			if (hive.flag_use_current_frame)
				return frame_to_time(edit_info.frame);
			else
				return time;
		}

		//
		// 基準終了位置(時間)を返します。
		//
		double get_base_time_end(double time)
		{
			if (hive.flag_use_current_frame)
				return frame_to_time(edit_info.frame - 1);
			else
				return time;
		}
	};
}
