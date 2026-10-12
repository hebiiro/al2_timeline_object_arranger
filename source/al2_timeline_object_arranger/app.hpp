#pragma once

namespace apn::timeline_object_arranger
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct app_t : app_base_t, app_interface_t
	{
		//
		// プラグインの初期化処理を実行します。
		//
		virtual BOOL on_initialize_plugin(DWORD version) override
		{
			MY_TRACE_FUNC("");

			// 各種開始処理を実行します。
			debug.init();
			config_io.init();
			main_window.init(idd_main);

			// コンフィグをファイルから読み込みます。
			config_io.read();

			return TRUE;
		}

		//
		// プラグインの後始末処理を実行します。
		//
		virtual BOOL on_uninitialize_plugin() override
		{
			MY_TRACE_FUNC("");

			// 各種終了処理を実行します。
			main_window.exit();
			config_io.exit();
			debug.exit();

			return TRUE;
		}

		//
		// プラグインを登録します。
		//
		virtual BOOL on_register_plugin(HOST_APP_TABLE* host) override
		{
			MY_TRACE_FUNC("");

			// メインウィンドウを登録します。
			host->register_window_client(version_base->get_name().c_str(), main_window);

			return TRUE;
		}

		virtual void shift_down() override { return shift_down_t().call_edit_section(); }
		virtual void shift_up() override { return shift_up_t().call_edit_section(); }
		virtual void align_left() override { return align_left_t().call_edit_section(); }
		virtual void align_right() override { return align_right_t().call_edit_section(); }
		virtual void stretch_left() override { return stretch_left_t().call_edit_section(); }
		virtual void stretch_right() override { return stretch_right_t().call_edit_section(); }
		virtual void relative_space_left() override { return relative_space_left_t().call_edit_section(); }
		virtual void relative_space_right() override { return relative_space_right_t().call_edit_section(); }
		virtual void absolute_space_left() override { return absolute_space_left_t().call_edit_section(); }
		virtual void absolute_space_right() override { return absolute_space_right_t().call_edit_section(); }
		virtual void move_vert_down() override { move_vert_down_t().call_edit_section(); }
		virtual void move_vert_up() override { move_vert_up_t().call_edit_section(); }
		virtual void fix_bpm() override { return fix_bpm_t().call_edit_section(); }
		virtual void erase_midpt() override { return erase_midpt_t().call_edit_section(); }
	} app_impl;
}
