#pragma once

namespace apn::timeline_object_arranger
{
	template <typename T>
	struct reverse_iteration_helper_t
	{
		using collection_t = T;
		using iterator = collection_t::reverse_iterator;
		using const_iterator = collection_t::const_reverse_iterator;

		collection_t& collection;

		reverse_iteration_helper_t(collection_t& collection) : collection(collection) {}
		iterator begin(){ return collection.rbegin(); }
		const_iterator begin() const { return collection.rbegin(); }
		iterator end(){ return collection.rend(); }
		const_iterator end() const { return collection.rend(); }
	};

	template <typename T>
	inline auto reverse_iteration(T& collection) { return reverse_iteration_helper_t<T>(collection); }

	//
	// このクラスはタイムラインアイテムの位置を変化させる処理を実行します。
	//
	struct action_t : utils_t
	{
		//
		// このクラスはノードです。
		//
		struct node_t
		{
			//
			// このクラスは編集前後の値です。
			//
			struct object_layer_time_t {
				int layer_set;
				double time_start, time_end;
			} before, after;

			//
			// オブジェクトのハンドルです。
			//
			OBJECT_HANDLE object_handle;

			//
			// コンストラクタです。
			//
			node_t(const action_t* action, OBJECT_HANDLE object_handle, const OBJECT_LAYER_FRAME& object_layer_frame)
				: object_handle(object_handle)
			{
				// オブジェクトの編集前の位置を取得しておきます。
				before.layer_set = object_layer_frame.layer;
				before.time_start = action->frame_to_time(object_layer_frame.start);
				before.time_end = action->frame_to_time(object_layer_frame.end);
				after = before;
			}
		};

		//
		// このクラスはレイヤーです。
		//
		struct layer_t
		{
			//
			// レイヤー番号です。
			//
			int layer_set;

			//
			// ノードのコレクションです。
			//
			std::vector<std::shared_ptr<node_t>> nodes;

			//
			// コンストラクタです。
			//
			explicit layer_t(int layer_set) : layer_set(layer_set) {}
		};

		//
		// このクラスは範囲です。
		//
		struct bound_t
		{
			int top_layer = INT_MAX;
			int bottom_layer = INT_MIN;
			double time_start = DBL_MAX;
			double time_end = DBL_MIN;

			//
			// 指定されたノードが含まれるように範囲を拡張します。
			//
			void inflate(const std::shared_ptr<node_t>& node)
			{
				// 一番上のレイヤーを取得します。
				top_layer = std::min(top_layer, node->before.layer_set);

				// 一番下のレイヤーを取得します。
				bottom_layer = std::max(bottom_layer, node->before.layer_set);

				// 最小の開始位置を取得します。
				time_start = std::min(time_start, node->before.time_start);

				// 最大の終了位置を取得します。
				time_end = std::max(time_end, node->before.time_end);
			}
		};

		//
		// ノードのコレクションです。
		// キーはオブジェクトハンドルです。
		//
		std::unordered_map<OBJECT_HANDLE, std::shared_ptr<node_t>> nodes;

		//
		// レイヤーのコレクションです。
		// キーはレイヤーのインデックスです。
		//
		std::unordered_map<int, std::shared_ptr<layer_t>> layers;

		//
		// 選択ノードのコレクションです。
		//
		std::unordered_set<std::shared_ptr<node_t>> selection;

		//
		// 選択ノードの範囲です。
		//
		bound_t bound;

		//
		// 編集セクションです。
		//
		EDIT_SECTION* edit = {};

		//
		// エラーが発生した場合用のエラーメッセージです。
		//
		std::wstring error_message;

		//
		// 指定されたノードが選択ノードの場合はtrueを返します。
		//
		bool is_selected(const std::shared_ptr<node_t>& node) const
		{
			return selection.contains(node);
		}

		//
		// オブジェクトからノードを取得して返します。
		// 内部的に使用されます。
		//
		std::shared_ptr<node_t> get_node(OBJECT_HANDLE object_handle)
		{
			auto it = nodes.find(object_handle);
			if (it == nodes.end()) return nullptr;
			return it->second;
		}

		//
		// 選択ノードを追加します。
		// 内部的に使用されます。
		//
		void select_node(const std::shared_ptr<node_t>& node)
		{
			MY_TRACE_FUNC("");

			selection.insert(node);
			bound.inflate(node);
		}

		//
		// 選択ノードを追加します。
		// 内部的に使用されます。
		//
		void select_object(OBJECT_HANDLE object_handle)
		{
			MY_TRACE_FUNC("{/}", object_handle);

			// オブジェクトハンドルが無効の場合は何もしません。
			if (!object_handle) return;

			// 引数で与えられたオブジェクトを選択します。
			if (auto node = get_node(object_handle)) select_node(node);
		}

		//
		// ノードのコレクションを初期化します。
		// 現在シーン内の全オブジェクトに相当します。
		// 内部的に使用されます。
		//
		void init_nodes()
		{
			MY_TRACE_FUNC("");

			// 現在シーン内の全レイヤーを走査します。
			for (decltype(edit_info.layer_max) i = 0; i <= edit_info.layer_max; i++)
			{
				// オブジェクトを検索するフレーム位置です。
				auto find_frame = decltype(edit_info.frame_max) {};

				// レイヤー内の全オブジェクトを走査します。
				while (auto object_handle = edit->find_object(i, find_frame))
				{
					// オブジェクトが配置されているレイヤーとフレームを取得します。
					auto object_layer_frame = edit->get_object_layer_frame(object_handle);

					// ノードのインスタンスを作成します。
					auto node = std::make_shared<node_t>(this, object_handle, object_layer_frame);

					// ノードをコレクションに追加します。
					nodes[object_handle] = node;

					// レイヤーを取得します。
					auto& layer = layers[object_layer_frame.layer];

					// レイヤーが取得できなかった場合は新規作成します。
					if (!layer) layer = std::make_shared<layer_t>(object_layer_frame.layer);

					// ノードをレイヤーに追加します。
					layer->nodes.emplace_back(node);

					// オブジェクトを検索するフレーム位置を進めます。
					find_frame = object_layer_frame.end + 1;
				}
			}
		}

		//
		// レイヤーのコレクションを初期化します。
		// 現在シーン内のオブジェクトが配置されている全レイヤーに相当します。
		// 内部的に使用されます。
		//
		void init_layers()
		{
			MY_TRACE_FUNC("");

			// レイヤーを走査します。
			for (auto& pair : layers)
			{
				// レイヤーを取得します。
				auto& layer = pair.second;

				// レイヤー内のノードを時間順に並べ替えます。
				std::sort(std::begin(layer->nodes), std::end(layer->nodes),
					[](const auto& x, const auto& y)
				{
					if (x->before.time_start == y->before.time_start)
						return x->before.time_end < y->before.time_end;
					else
						return x->before.time_start < y->before.time_start;
				});
			}
		}

		//
		// 編集対象を初期化します。
		// 選択オブジェクトまたはフォーカスオブジェクトに相当します。
		// 内部的に使用されます。
		//
		void init_selection()
		{
			MY_TRACE_FUNC("");

			// 選択オブジェクトの数を取得します。
			auto nb_selected_objects = edit->get_selected_object_num();
			MY_TRACE_INT(nb_selected_objects);

			// 選択オブジェクトが存在する場合は
			if (nb_selected_objects)
			{
				// 選択オブジェクトを走査します。
				for (decltype(nb_selected_objects) i = 0; i < nb_selected_objects; i++)
				{
					// 選択オブジェクトを選択します。
					select_object(edit->get_selected_object(i));
				}
			}
			// 選択オブジェクトが存在しない場合は
			else
			{
				// フォーカスオブジェクトを選択します。
				select_object(edit->get_focus_object());
			}
		}

		//
		// アクションを適用可能かどうかチェックします。
		//
		void check()
		{
			MY_TRACE_FUNC("");

			// エラーメッセージです。
			std::wstring error;

			// 選択ノードを走査します。
			for (const auto& mover : selection)
			{
				if (mover->after.time_start < 0)
				{
					error += my::format(
						L"レイヤー{/}のオブジェクトの開始位置が不正な値になっています\n",
						mover->after.layer_set + 1);

					continue;
				}

				if (mover->after.time_end < 0)
				{
					error += my::format(
						L"レイヤー{/}のオブジェクトの終了位置が不正な値になっています\n",
						mover->after.layer_set + 1);

					continue;
				}

				if (mover->after.time_start >= mover->after.time_end)
				{
					error += my::format(
						L"レイヤー{/}のオブジェクトの長さが不正な値になっています\n",
						mover->after.layer_set + 1);

					continue;
				}

				// ここで、他のノードが選択ノードの移動を妨げていないか確認します。
				{
					// 他のノードを走査します。
					for (const auto& pair : nodes)
					{
						// 他のノードを取得します。
						const auto& node = pair.second;

						// 他のノードと選択ノードが同じオブジェクトを保有している場合は何もしません。
						if (node->object_handle == mover->object_handle) continue;

						// ノードと選択ノードが異なるレイヤーに存在している場合は何もしません。
						if (node->after.layer_set != mover->after.layer_set) continue;

						// ノードと選択ノードが重なっている場合は
						if (node->after.time_start <= mover->after.time_end &&
							node->after.time_end >= mover->after.time_start)
						{
							error += my::format(L"レイヤー{/}で[{/}]のオブジェクトが交差しています\n",
								mover->after.layer_set + 1, time_to_str(mover->after.time_start, mover->after.time_end));
						}
					}
				}
			}

			// エラーメッセージが有効の場合は、例外としてスローします。
			if (!error.empty()) throw error;
		}

		//
		// 選択ノードのオブジェクトを実際に移動させます。
		//
		void apply()
		{
			MY_TRACE_FUNC("");

			// 選択オブジェクトが存在しない場合は
			if (selection.empty())
				throw std::wstring(L"オブジェクトを選択してください");

			// 実際にオブジェクトを移動します。
			move_objects();
		}

		//
		// 編集セクションを開始します。
		//
		void call_edit_section()
		{
			MY_TRACE_FUNC("");

			// 編集セクションを開始します。
			editor.handle->call_edit_section_param(
				this, [](void* p, EDIT_SECTION* edit)
			{
				// thisポインタを取得します。
				auto action = (action_t*)p;

				// 編集セクションを開始します。
				action->call_edit_section_internal(edit);
			});

			// エラーが発生した場合は、エラーメッセージを表示します。
			if (error_message.length())
				hive.message_box(error_message);
		}

		//
		// 編集セクションを開始します。
		// 内部的に使用されます。
		//
		void call_edit_section_internal(EDIT_SECTION* edit)
		{
			MY_TRACE_FUNC("{/hex}", edit);

			// 編集セクションを取得しておきます。
			this->edit = edit;

			try
			{
				// ノードのコレクションを初期化します。
				init_nodes();

				// レイヤーのコレクションを初期化します。
				init_layers();

				// 編集対象を初期化します。
				init_selection();

				// アクションを実行します。
				execute();
			}
			catch (const std::wstring& error)
			{
				// 例外が発生した場合は、エラーメッセージを格納します。
				error_message = error;
			}
		}

		//
		// アクションを実行します。
		//
		virtual void execute()
		{
			// アクションを適用可能かどうかチェックします。
			check();

			// 選択ノードのオブジェクトを実際に移動させます。
			apply();
		}

		//
		// 安全なレイヤー番号を返します。
		//
		inline int get_safe_layer(int layer) { return layer + (edit_info.layer_max + 1); }

		//
		// 元のレイヤー番号を返します。
		//
		inline int get_unsafe_layer(int layer) { return layer - (edit_info.layer_max + 1); }

		//
		// 選択オブジェクトを実際に移動します。
		//
		virtual void move_objects()
		{
			// ソート済み選択ノードのコレクションです。
			std::vector<std::shared_ptr<node_t>> sorted_selection;
			{
				// 選択ノードをソート済みコレクションにコピーします。
				for (const auto& mover : selection) sorted_selection.emplace_back(mover);

				// ソート済みコレクションを編集後順にソートします。
				std::sort(sorted_selection.begin(), sorted_selection.end(), [](const auto& lhs, const auto& rhs)
				{
					if (lhs->after.layer_set == rhs->after.layer_set)
						return lhs->after.time_start < rhs->after.time_start;
					else
						return lhs->after.layer_set < rhs->after.layer_set;
				});
			}

			// 選択オブジェクトを実際の位置に移動します。
			auto result = [&]()
			{
				// まず、全ての選択オブジェクトを安全なレイヤーに移動します。
				{
					// 選択ノードを走査します。
					for (const auto& mover : sorted_selection)
					{
						// 安全なレイヤーを取得します。
						auto safe_layer = get_safe_layer(mover->before.layer_set);

						// 安全なレイヤーに移動します。
						auto r0 = edit->move_object(mover->object_handle,
							safe_layer, time_to_frame(mover->before.time_start));
						if (!r0) return FALSE;
					}
				}

				// 選択ノードを走査します。
				for (const auto& mover : sorted_selection)
				{
					// 編集前後のオブジェクトの長さを算出します。
					struct object_layer_frame_t : OBJECT_LAYER_FRAME {
						int range;
						object_layer_frame_t(action_t* action, const node_t::object_layer_time_t& layer_time) {
							layer = layer_time.layer_set;
							start = action->time_to_frame(layer_time.time_start);
							end = action->time_to_frame(layer_time.time_end);
							range = end - start;
						}
					} before(this, mover->before), after(this, mover->after);

					// オブジェクトの位置のみを変更する場合は
					if (before.range == after.range)
					{
						// 編集後の位置にオブジェクトを移動します。
						auto r0 = edit->move_object(mover->object_handle, after.layer, after.start);
						if (!r0) return FALSE;
					}
					// オブジェクトの終了位置を変更する場合は
					else if (before.end != after.end)
					{
						// オブジェクトの中間点の数を取得します。
						auto nb_sections = edit->get_object_section_num(mover->object_handle);

						// 開始位置が揃うように、編集後の位置にオブジェクトを移動します。
						auto r0 = edit->move_object(mover->object_handle, after.layer, after.start);
						if (!r0) return FALSE;

						// オブジェクトの終了位置を変更します。
						// このとき、moverは編集後の位置順にソートされているので、
						// afterの右側には他のオブジェクトは存在しません。
						// よって、終了位置の変更(伸ばす場合)は必ず成功するはずです。
						auto r1 = edit->move_object_section(mover->object_handle, nb_sections, after.end);
						if (!r1) return FALSE;
					}
					// それ以外の場合は
					else
					{
						// 終了位置が揃うように、編集後の位置にオブジェクトを移動します。
						auto r0 = edit->move_object(mover->object_handle, after.layer, after.end - before.range);
						if (!r0) return FALSE;

						// オブジェクトの開始位置を変更します。
						// このとき、オブジェクトの左側には十分な空きがあるはずなので、
						// 開始位置の変更(伸ばす場合)は必ず成功するはずです。
						auto r1 = edit->move_object_section(mover->object_handle, 0, after.start);
						if (!r1) return FALSE;
					}
				}

				return TRUE;
			}();

			// 移動に失敗した場合は
			if (!result)
			{
				// 元の位置に戻すのはほぼ不可能なので、
				// エラーが出たことだけをユーザーに通知します。
				throw (std::wstring)
					L"オブジェクトの移動に失敗しました\n"
					L"お手数ですが、アンドゥで復元してください\n";
			}
		}
	};
}
