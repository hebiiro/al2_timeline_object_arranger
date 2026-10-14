#pragma once

namespace apn::timeline_object_arranger
{
	//
	// このクラスはこのプログラムのバージョンを管理します。
	//
	inline struct version_t : version_base_t {
		//
		// コンストラクタです。
		//
		version_t() : version_base_t(L"タイムラインオブジェクト整列", L"🐍タイムラインオブジェクト整列🔖", L"r2") {}
	} version;
}
