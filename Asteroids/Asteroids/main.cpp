#include <windows.h>
#include <Winuser.h>

#include <stdint.h>
#include <cstring>
#include <cmath>
#include <limits>

#undef min
#undef max

#define MAX(a, b) (a > b? a: b)
#define MIN(a, b) (a < b? a: b)


// STL
#include <vector>
#include <string>



// Global varibals
bool running = true;

const int width = 800;
const int height = 800;
const int depth = 255;

// headers

// Unity build
#include "render_stuff.cpp"
#include "geometry.cpp"
#include "draw.cpp"
#include "input.cpp"
#include "timer.cpp"

#include "game_stuff.cpp"





LRESULT CALLBACK win_callback(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lParam)
{
	LRESULT res = 0;

	switch (uMsg)
	{
		case WM_CLOSE:
		case WM_DESTROY:
		{
			running = false;
		} break;

		case WM_SIZE:
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			surface.width = rect.right - rect.left;
			surface.height = rect.bottom - rect.top;
		
			int size = surface.width * surface.height * sizeof(unsigned int);
		
			if (surface.memory) VirtualFree(surface.memory, 0 , MEM_RELEASE);
			surface.memory = (uint32_t*)VirtualAlloc(0, size * sizeof(uint32_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		
			surface.bitmap_info.bmiHeader.biSize = sizeof(surface.bitmap_info.bmiHeader);
			surface.bitmap_info.bmiHeader.biWidth = surface.width;
			surface.bitmap_info.bmiHeader.biHeight = surface.height;
			surface.bitmap_info.bmiHeader.biPlanes = 1;
			surface.bitmap_info.bmiHeader.biBitCount = 32;
			surface.bitmap_info.bmiHeader.biCompression = BI_RGB;
		
		} break;

		default:
		{
			res = DefWindowProc(hwnd, uMsg, wparam, lParam);
		}
	}
	return res;
}



int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPiv, LPSTR args, int someshit)
{
	// create window class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Asteroids";
	window_class.lpfnWndProc = win_callback;

	// reg window
	RegisterClass(&window_class);

	// create window
	HWND window = CreateWindow(window_class.lpszClassName, "Asteroids", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, hInst, 0);
	HDC hdc = GetDC(window);

	// game vaars
	Ship ship;


	// input
	Key_input Kinput;

	Timer timer;

	while (running)
	{
		// Input
		MSG msg;
		while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
				case WM_KEYUP:
				case WM_KEYDOWN:
				{
					uint32_t vk_code = (uint32_t)msg.wParam;
					bool is_down = ((msg.lParam & (1 << 31)) == 0);

					switch (vk_code)
					{
						case VK_LEFT:
						{
							Kinput.buttons[BUTTON_LEFT].changed = true;// input.buttons[BUTTON_LROTATE].is_down != is_down;
							Kinput.buttons[BUTTON_LEFT].is_down = is_down;
						}break;
						case VK_RIGHT:
						{
							Kinput.buttons[BUTTON_RIGHT].changed = true;// input.buttons[BUTTON_RROTATE].is_down != is_down;
							Kinput.buttons[BUTTON_RIGHT].is_down = is_down;
						}break;
						case VK_UP:
						{
							Kinput.buttons[BUTTON_UP].changed = true;// input.buttons[BUTTON_LROTATE].is_down != is_down;
							Kinput.buttons[BUTTON_UP].is_down = is_down;
						}break;
						case VK_SPACE:
						{
							Kinput.buttons[BUTTON_SHOOT].changed = true;// input.buttons[BUTTON_RROTATE].is_down != is_down;
							Kinput.buttons[BUTTON_SHOOT].is_down = is_down;
						}break;
					}
				}break;
				default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}break;
			}
		}

		// input handler
		if ()


		// Simulate



		// Draw

		// Clear sreen
		drawRect(0, 0, surface.width, surface.height, Color(0, 0, 0));


		draw_shape(ship.pts);

		
		// Render
		StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		// Timer
		timer.update();
		
		// Log
		char log[64];
		sprintf_s(log, "fps: %d ftime: %.3f sec\n", timer.FPS, timer.elapsed);
		OutputDebugString(log);
	}
		
	return 0;
}