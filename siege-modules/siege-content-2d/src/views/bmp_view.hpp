#ifndef BITMAPWINDOW_HPP
#define BITMAPWINDOW_HPP

#include <siege/platform/win/desktop/common_controls.hpp>
#include <siege/platform/win/desktop/window_factory.hpp>
#include <siege/platform/win/core/com/collection.hpp>
#include <siege/platform/win/desktop/drawing.hpp>
#include <cassert>
#include <sstream>
#include <vector>
#include <memory>
#include <istream>
#include <spanstream>
#include <oleacc.h>
#include "bmp_controller.hpp"

namespace siege::views
{
	struct bmp_view : win32::window_ref
	{
		win32::static_control static_image;
		win32::list_view palettes_list;
		win32::window zoom;
		win32::window frame_selector;

		win32::static_control frame_label;
		win32::static_control frame_value;

		win32::static_control zoom_label;
		win32::static_control zoom_value;

		bmp_controller controller;

		win32::gdi_bitmap current_bitmap;

		bmp_view(win32::hwnd_t self, const CREATESTRUCTW&) : win32::window_ref(self)
		{
		}

		auto on_create(const win32::create_message& info)
		{
			auto this_module = win32::window_factory(ref());

			std::wstring temp = L"menu.pal";

			frame_selector = *this_module.CreateWindowExW(::CREATESTRUCTW{
							.style = WS_VISIBLE | WS_CHILD | UDS_HORZ | UDS_SETBUDDYINT,
							.lpszName = L"Frame Selector",
							.lpszClass = UPDOWN_CLASSW
				});

			frame_label = *this_module.CreateWindowExW<win32::static_control>(::CREATESTRUCTW{
							.style = WS_VISIBLE | WS_CHILD | SS_LEFT,
							.lpszName = L"Frame: "
				});

			frame_value = *this_module.CreateWindowExW<win32::static_control>(::CREATESTRUCTW{
							.style = WS_VISIBLE | WS_CHILD | SS_LEFT,
							.lpszName = L""
				});


			::SendMessageW(frame_selector, UDM_SETBUDDY, win32::wparam_t(win32::hwnd_t(frame_value)), 0);
			::SendMessageW(frame_selector, UDM_SETRANGE, 0, MAKELPARAM(1, 1));

			zoom = *this_module.CreateWindowExW(::CREATESTRUCTW{
							.style = WS_VISIBLE | WS_CHILD | UDS_SETBUDDYINT,
							.lpszName = L"Zoom",
							.lpszClass = UPDOWN_CLASSW
				});

			zoom_label = *this_module.CreateWindowExW<win32::static_control>(
				::CREATESTRUCTW{
				  .style = WS_VISIBLE | WS_CHILD | SS_LEFT,
				  .lpszName = L"Zoom: ",
				});

			zoom_value = *this_module.CreateWindowExW<win32::static_control>(
				::CREATESTRUCTW{
				  .style = WS_VISIBLE | WS_CHILD | SS_LEFT,
				});

			::SendMessageW(zoom, UDM_SETBUDDY, win32::wparam_t(win32::hwnd_t(zoom_value)), 0);

			::SendMessageW(zoom, UDM_SETRANGE, 0, MAKELPARAM(100, 1));

			palettes_list = [&] {
				auto palettes_list = *this_module.CreateWindowExW<win32::list_view>(::CREATESTRUCTW{
							.style = WS_VISIBLE | WS_CHILD | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_NOCOLUMNHEADER,
							.lpszName = L"Palettes"
					});

				palettes_list.SetView(win32::list_view::view_type::details_view);
				assert(palettes_list.EnableGroupView(true));
				assert(palettes_list.SetTileViewInfo(LVTILEVIEWINFO{
					.dwFlags = LVTVIF_FIXEDWIDTH,
					.sizeTile = SIZE {.cx = this->GetClientSize()->cx, .cy = 50},
					}));

				palettes_list.SetExtendedListViewStyle(0,
					LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_AUTOSIZECOLUMNS);

				palettes_list.win32::list_view::InsertColumn(-1, LVCOLUMNW{
					.pszText = const_cast<wchar_t*>(L"")
					});

				std::vector<win32::list_view_group> groups{
					{L"main.dpl", {{ L"palette 1"}, { L"palette 2"}, { L"palette 3"}}},
					{L"test.pal", {{ L"palette 1"}}},
					{L"other.ipl", {{ L"palette 1"}}},
				};

				palettes_list.InsertGroups(groups);

				return palettes_list;
				}();

				static_image = *this_module.CreateWindowExW<win32::static_control>(::CREATESTRUCTW{
					  .hwndParent = *this,
					  .style = WS_VISIBLE | WS_CHILD | SS_BITMAP | SS_REALSIZECONTROL
					});

				return 0;
		}

		auto on_size(win32::size_message sized)
		{
			auto left_size = SIZE{ .cx = (sized.client_size.cx / 3) * 2, .cy = sized.client_size.cy };
			auto right_size = SIZE{ .cx = sized.client_size.cx - left_size.cx, .cy = sized.client_size.cy };

			const auto top_left_height = left_size.cy / 20;
			const auto top_left_width = left_size.cx / 6;

			auto x = 0;

			frame_label.SetWindowPos(POINT{ .x = x });
			frame_label.SetWindowPos(SIZE{ .cx = left_size.cx / 6, .cy = top_left_height });
			x += top_left_width;

			frame_value.SetWindowPos(POINT{ .x = x });
			frame_value.SetWindowPos(SIZE{ .cx = left_size.cx / 6, .cy = top_left_height });
			x += top_left_width;

			frame_selector.SetWindowPos(POINT{ .x = x });
			frame_selector.SetWindowPos(SIZE{ .cx = left_size.cx / 6, .cy = top_left_height });
			x += top_left_width;

			zoom_label.SetWindowPos(POINT{ .x = x });
			zoom_label.SetWindowPos(SIZE{ .cx = left_size.cx / 6, .cy = top_left_height });
			x += top_left_width;

			zoom_value.SetWindowPos(POINT{ .x = x });
			zoom_value.SetWindowPos(SIZE{ .cx = left_size.cx / 6, .cy = top_left_height });
			x += top_left_width;

			zoom.SetWindowPos(POINT{ .x = x });
			zoom.SetWindowPos(SIZE{ .cx = left_size.cx / 6, .cy = top_left_height });

			static_image.SetWindowPos(POINT{ .y = top_left_height });
			static_image.SetWindowPos(SIZE{ .cx = left_size.cx, .cy = left_size.cy - top_left_height });

			palettes_list.SetWindowPos(POINT{ .x = left_size.cx });
			palettes_list.SetWindowPos(right_size);
			palettes_list.SetColumnWidth(0, right_size.cx);

			return 0;
		}

		std::optional<LRESULT> on_get_object(win32::get_object_message message)
		{
			if (message.object_id == OBJID_NATIVEOM)
			{
				auto collection = std::make_unique<win32::com::OwningCollection<std::unique_ptr<IStream, void(*)(IStream*)>>>();

				auto result = LresultFromObject(__uuidof(IDispatch), message.flags, static_cast<IDispatch*>(collection.get()));

				collection.release()->Release();

				return result;
			}

			return std::nullopt;
		}

		auto on_copy_data(win32::copy_data_message<char> message)
		{
			std::spanstream stream(message.data);

			if (bmp_controller::is_bmp(stream))
			{
				auto task = controller.load_palettes_async(std::nullopt, [](auto) {
					return std::set<std::filesystem::path>{};
					}, [](auto) {
						return std::vector<char>{};
						});
				auto count = controller.load_bitmap(stream);

				if (count > 0)
				{
					::SendMessageW(frame_selector, UDM_SETRANGE, 0, MAKELPARAM(count, 1));
					auto size = static_image.GetClientSize();
					BITMAPINFO info{
								.bmiHeader {
									.biSize = sizeof(BITMAPINFOHEADER),
									.biWidth = LONG(size->cx),
									.biHeight = LONG(size->cy),
									.biPlanes = 1,
									.biBitCount = 32,
									.biCompression = BI_RGB
								}
					};
					auto wnd_dc = ::GetDC(nullptr);

					void* pixels = nullptr;
					current_bitmap.reset(::CreateDIBSection(wnd_dc, &info, DIB_RGB_COLORS, &pixels, nullptr, 0));

					controller.convert(0, std::make_pair(size->cx, size->cy), 32, std::span(reinterpret_cast<std::byte*>(pixels), size->cx * size->cy * 4));

					static_image.SetImage(current_bitmap.get());

					return TRUE;
				}

				return FALSE;
			}

			return FALSE;
		}
	};
}

#endif