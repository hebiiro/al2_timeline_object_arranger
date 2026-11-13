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
		version_t() : version_base_t(L"整列", L"🐍整列🔖", L"r3") {}
	} version;
}
