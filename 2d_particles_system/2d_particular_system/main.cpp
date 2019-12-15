#include <Windows.h>
#include <cstdint>
#include <limits>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>
#include <random>

#undef min
#undef max

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
#include "mouse_input.cpp"
#include "timer.cpp"


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
	window_class.lpszClassName = L"Paricylar system";
	window_class.lpfnWndProc = win_callback;

	RegisterClass(&window_class);

	HWND window = CreateWindow(window_class.lpszClassName, L"particylar system", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	goOutShader shader;
	Mouse_Input mouse;
	Timer timer;
	particles_buffer particles(2000);

	float interval = 0.1;

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
				mouse.buttons[LBUTTON].change = !mouse.buttons[LBUTTON].is_dawn;
				mouse.buttons[LBUTTON].is_dawn = true;
			}break;
			case WM_LBUTTONUP:
			{
				mouse.buttons[LBUTTON].change = mouse.buttons[LBUTTON].is_dawn;
				mouse.buttons[LBUTTON].is_dawn = false;
			}break;
			default:
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			}
		}

		// Simulate
		interval -= timer.elapsed;

		//if (interval < 0)
		{
			add_particles(particles, rand() % 10, mouse.pos, 0.1, vec2f(0, 1), PI*2, float(rand() % 20 + 30) / 100, float(rand() % 1000) / 1000);
			interval = 0.02;
		}

		particles.calculate();




		// Draw
		// clear screen
		draw_filled_rect(0, 0, surface.width, surface.height, Color(0, 0, 0));

		// draw particles
		for (int i = 0; i < particles.actives; i++)
		{
			uni_life_time = particles[i].life_time / particles[i].whole_life;
			draw_filled_shape(shapes[particles[i].shape], &particles[i], &shader);
		}
		
		// Render
		StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		// Timer
		timer.update();
		elapsed = timer.elapsed;

		// Log
		char log[128];
		sprintf_s(log, "fps: %d ftime: %.3f sec  particles: %d \n", timer.FPS, timer.elapsed, particles.actives);
		OutputDebugStringA(log);

	}

	return 0;
}