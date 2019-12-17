#include <Windows.h>
#include <cstdint>
#include <limits>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>
#include <random>

#define MAX(a, b) (a > b? a: b)
#define MIN(a, b) (a < b? a: b)
#define PI 3.14159265359

// global variables
bool running = true;

#include "render_stuff.cpp"
#include "geometry.cpp"
#include "particles.cpp"
#include "shader.cpp"
#include "draw.cpp"
#include "input.cpp"
#include "timer.cpp"

#include "game_stuff.cpp"


LRESULT CALLBACK win_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT res = 0;
	switch (uMsg)
	{
		case WM_DESTROY:
		case WM_CLOSE:
		{
			running = false;
		}break;
		case WM_SIZE:
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			surface.width = rect.right - rect.left;
			surface.height = rect.bottom - rect.top;

			int size = surface.width * surface.height * sizeof(unsigned int);

			if (surface.memory) VirtualFree(surface.memory, 0, MEM_RELEASE);
			surface.memory = (uint32_t*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			surface.bitmap_info.bmiHeader.biSize = sizeof(surface.bitmap_info.bmiHeader);
			surface.bitmap_info.bmiHeader.biWidth = surface.width;
			surface.bitmap_info.bmiHeader.biHeight = surface.height;
			surface.bitmap_info.bmiHeader.biPlanes = 1;
			surface.bitmap_info.bmiHeader.biBitCount = 32;
			surface.bitmap_info.bmiHeader.biCompression = BI_RGB;
		} break;
		default:
		{
			res = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		return res;
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Paricylar system";
	window_class.lpfnWndProc = win_callback;

	RegisterClass(&window_class);

	HWND window = CreateWindow(window_class.lpszClassName, "particylar system", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	Mouse_Input mouse;
	Key_Input input;
	Timer timer;

	Ship ship;
	Ship_shader ship_shader;


	while (running)
	{
		MSG msg;
		while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
				case WM_MOUSEMOVE:
				{
					int x = LOWORD(msg.lParam);
					int y = HIWORD(msg.lParam);
					mouse.pos.x = float(x - surface.width / 2) / surface.height;
					mouse.pos.y = float(surface.height - y - surface.height / 2) / surface.height;
				}break;
				case WM_LBUTTONDOWN:
				{
					mouse.buttons[LBUTTON].changed = !mouse.buttons[LBUTTON].is_down;
					mouse.buttons[LBUTTON].is_down = true;
				}break;
				case WM_LBUTTONUP:
				{
					mouse.buttons[LBUTTON].changed = mouse.buttons[LBUTTON].is_down;
					mouse.buttons[LBUTTON].is_down = false;
				}break;
				case WM_KEYUP:
				case WM_KEYDOWN:
				{
					uint32_t vk_code = (uint32_t)msg.wParam;
					bool is_down = ((msg.lParam & (1 << 31)) == 0);

					switch (vk_code)
					{
						case VK_LEFT:
						{
							input.buttons[BUTTON_LEFT].changed = input.buttons[BUTTON_LEFT].is_down != is_down;
							input.buttons[BUTTON_LEFT].is_down = is_down;

						}break;
						case VK_RIGHT:
						{
							input.buttons[BUTTON_RIGHT].changed = input.buttons[BUTTON_RIGHT].is_down != is_down;
							input.buttons[BUTTON_RIGHT].is_down = is_down;

						}break;
						case VK_UP:
						{
							input.buttons[BUTTON_UP].changed = input.buttons[BUTTON_UP].is_down != is_down;
							input.buttons[BUTTON_UP].is_down = is_down;

						}break;
						case VK_DOWN:
						{
							input.buttons[BUTTON_DOWN].changed = input.buttons[BUTTON_DOWN].is_down != is_down;
							input.buttons[BUTTON_DOWN].is_down = is_down;

						}break;
					}
				}
				default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
		
		//  Simulate

		// input handler
		if (input.buttons[BUTTON_UP].is_down)
		{
			ship.speed_x += ship.speed_dif * cosf(ship.angle);
			ship.speed_y += ship.speed_dif * sinf(ship.angle);
		}
		else
		{
			ship.speed_x = fabs(ship.speed_x) < 0.00000000001 ? 0 : ship.speed_x + sgn(-ship.speed_x) * ship.speed_dif * fabs(cosf(ship.angle));
			ship.speed_y = fabs(ship.speed_y) < 0.00000000001 ? 0 : ship.speed_y + sgn(-ship.speed_y) * ship.speed_dif * fabs(sinf(ship.angle));
		}

		if (input.buttons[BUTTON_LEFT].is_down)
			ship.angle += PI * elapsed;

		if (input.buttons[BUTTON_RIGHT].is_down)
			ship.angle -= PI* elapsed;

		ship.calculate();

		// collision detection




		// Draw
		// clear screen
		draw_filled_rect(0, 0, surface.width, surface.height, Color(0, 0, 0));

		draw_shape(ship.pts, ship, &ship_shader);


		// Render
		StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		// Timer
		timer.update();
		elapsed = timer.elapsed;

		// Log
		char log[128];
		sprintf_s(log, "fps: %d ftime: %.3f sec \n", timer.FPS, timer.elapsed);
		OutputDebugStringA(log);

	}

	return 0;
}