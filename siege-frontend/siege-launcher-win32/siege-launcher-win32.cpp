// siege-launcher-win32.cpp : Defines the entry point for the application.
//

#define _CRT_SECURE_NO_WARNINGS
#include <array>
#include <optional>
#include <algorithm>
#include <vector>
#include <bit>
#include <variant>
#include <functional>
#include <thread>
#include <iostream>
#include "windows.hpp"
//#include "http_client.hpp"

std::array<wchar_t, 100> app_title;

void worker_thread_main()
{
	MSG msg;

	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		auto cracked_message = make_window_message(msg);

		std::visit(overloaded {
			[&](::message& message) -> std::optional<LRESULT> {
				if (message.message == WM_COUT)
				{
					if (message.wParam == 0)
					{
						std::wcout << reinterpret_cast<wchar_t*>(message.lParam);
					}
				}
				return std::nullopt;
			},
			[&](auto&)  -> std::optional<LRESULT> {
				return std::nullopt;
			}
		}, cracked_message);
		
		std::this_thread::yield();
	}
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	std::thread worker(worker_thread_main);

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, app_title.data(), int(app_title.size()));

	std::vector<client_control> controls;

	window main_window{WNDCLASSEXW {
		.style{CS_HREDRAW | CS_VREDRAW},
		.hInstance = hInstance,
		.hIcon = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_SIEGELAUNCHERWIN32)),
		.hCursor = LoadCursorW(hInstance, IDC_ARROW),
		.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
		.lpszMenuName = MAKEINTRESOURCEW(IDC_SIEGELAUNCHERWIN32),
		.lpszClassName{L"SiegeLauncherMainWindow"},
		.hIconSm = LoadIconW(hInstance, MAKEINTRESOURCE(IDI_SMALL)),
	}, CREATESTRUCTW {
		.cx = CW_USEDEFAULT,
		.x = CW_USEDEFAULT,
		.style = WS_OVERLAPPEDWINDOW,
		.lpszName = app_title.data()
	}, 
		[&](window& self, auto message) -> std::optional<LRESULT>
	{
		 return std::visit(overloaded {
			 [&](create_message& command) -> std::optional<LRESULT> {
					
#if _DEBUG
						AllocConsole();

						freopen("CONOUT$", "w", stdout);
						freopen("CONOUT$", "w", stderr);
#endif

					INITCOMMONCONTROLSEX settings{.dwSize{sizeof(INITCOMMONCONTROLSEX)}};
					InitCommonControlsEx(&settings);

					controls.reserve(10);

					PostThreadMessageW(GetThreadId(worker.native_handle()), WM_COUT, 0, reinterpret_cast<LPARAM>(L"Main window created"));

					auto& button_instance = controls.emplace_back(button{ CREATESTRUCTW{
						.hwndParent = self.handle,
						.cy = 100,  
						.cx = 100,
						.y = 10,       
						.x = 10,       
						.style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
						.lpszName = L"Click me"}, 
						[&](auto& self, UINT_PTR uIdSubclass, auto button_message) -> std::optional<LRESULT>
						{
							return std::visit(overloaded{
								[&](command_message& command) -> std::optional<LRESULT> {
									MessageBoxExW(self.handle, L"Hello world", L"Test Message", 0, 0);
									return TRUE;
								},
								[](auto&) -> std::optional<LRESULT> { return std::nullopt; }
							}, button_message);

							return std::nullopt;
						}
						});

					auto& edit_instance = controls.emplace_back(edit{ CREATESTRUCTW{
						.hwndParent = self.handle,              
						.cy = 100,
						.cx = 100,                
						.y = 110, 
						.x = 10,       
						.style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON}, [&](auto& self, UINT_PTR uIdSubclass, auto message) -> std::optional<LRESULT>
						{
							return std::nullopt;
						}
						});

					auto& combo_box_instance = controls.emplace_back(combo_box{CREATESTRUCTW{
						.hwndParent = self.handle,
						.cy = 100,
						.cx = 100,
						.y = 210,
						.x = 10,   
						.style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON}, [&](auto& self, UINT_PTR uIdSubclass, auto message) -> std::optional<LRESULT>
						{
							return std::nullopt;
						}
						});

				return 0;
			 },
			 [&](destroy_message& command) -> std::optional<LRESULT> {
				controls.erase(std::remove_if(controls.begin(), controls.end(), [](auto& button) {
						 return std::visit([](auto& real_control) { return !real_control.HandleMessage; }, button);
						 }), controls.end());
				PostQuitMessage(0);
				return 0;
			 },
			 [&](command_message& command) -> std::optional<LRESULT> {
				auto child_control = std::find_if(controls.begin(), controls.end(), [&](auto& control) {
					return command.handle == std::visit([](auto& real_control) { return real_control.handle; }, control);
					});

				if (child_control != controls.end())
				{
					return SendMessageW(command.handle, command_message::id, command.wparam(), command.lparam());
				}
					
				if (command.identifier == IDM_ABOUT)
				{
                    dialog::show_modal(self.handle, MAKEINTRESOURCE(IDD_ABOUTBOX), [&](dialog& self, auto dialog_message) -> INT_PTR {
						return std::visit(overloaded{
									[&](command_message& dialog_command) -> INT_PTR {
										if (dialog_command.identifier == IDOK || dialog_command.identifier == IDCANCEL)
										{
											EndDialog(self.handle, dialog_command.identifier);
											return (INT_PTR)TRUE;
										}

										return (INT_PTR)FALSE;
									},
								[](auto&) -> INT_PTR {
									return (INT_PTR)FALSE;
									}
						}, dialog_message);
						});
				}
				else if (command.identifier == IDM_EXIT)
				{
					DestroyWindow(self.handle);
					return 0;
				}

				return std::nullopt;
			},
			 [&](auto& raw_message) -> std::optional<LRESULT> {
				 return std::nullopt;
			}
		}, message);
	}
	};

	if (!main_window.handle)
	{
		return FALSE;
	}

	ShowWindow(main_window.handle, nCmdShow);
	UpdateWindow(main_window.handle);


	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIEGELAUNCHERWIN32));

	MSG msg;

	// Main message loop:
	while (GetMessageW(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
	}

	return (int)msg.wParam;
}