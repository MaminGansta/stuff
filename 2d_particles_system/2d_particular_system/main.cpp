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

#define mod 2 // 0 1 2 : flame , black hole, gravity

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

#if mod == 0
	particles_buffer particles(1000);
#elif mod == 1
	// lamda behevior
	particles_buffer particles(4900, [](std::vector<particle>& buffer, size_t& actives) {

		for (int i = 0; i < actives; i++)
		{
			//buffer[i].speed = buffer[i].whole_speed * (buffer[i].life_time / buffer[i].whole_life);
			float dist = pow(mouse_pos.x - buffer[i].pos.x, 2) + pow(mouse_pos.y - buffer[i].pos.y, 2);
			float force = dist < 0.0005? 0 : 0.006  / dist;

			buffer[i].speed_dir =  (mouse_pos - buffer[i].pos).normalize();
			buffer[i].speed = force;

			buffer[i].pos += buffer[i].speed_dir * buffer[i].speed * elapsed;
			buffer[i].rotate = Matrix22f(cosf(buffer[i].rotation_angle), -sinf(buffer[i].rotation_angle),
				sinf(buffer[i].rotation_angle), cosf(buffer[i].rotation_angle));
			buffer[i].scale[0][0] = buffer[i].scale_indx;
			buffer[i].scale[1][1] = buffer[i].scale_indx;
		}});

	// add some particles
	for (int i = 0; i < 70; i++)
		for (int j = 0; j < 70; j++)
			add_particles(particles, 1, vec2f(-0.7 + 0.02 * j, -0.7 + 0.02 * i), 0.03, vec2f(0, 1), PI * 2);
#elif mod == 2
	// lamda behevior
#define max_speed 0.6
	particles_buffer particles(40000, [](std::vector<particle>& buffer, size_t& actives) {

		for (int i = 0; i < actives; i++)
		{
			//buffer[i].speed = buffer[i].whole_speed * (buffer[i].life_time / buffer[i].whole_life);
			float dist = pow(mouse_pos.x - buffer[i].pos.x, 4) + pow(mouse_pos.y - buffer[i].pos.y, 4);

			float force = dist < 0.01 ? 0 : 0.004 / dist;
			float force_x = force*(mouse_pos.x - buffer[i].pos.x);
			float force_y = force*(mouse_pos.y - buffer[i].pos.y);

			buffer[i].speed += force_x;
			buffer[i].speed = fabs(buffer[i].speed) > max_speed ? max_speed * sgn(buffer[i].speed) : buffer[i].speed;
			buffer[i].speed_y += force_y;
			buffer[i].speed_y = fabs(buffer[i].speed_y) > max_speed ? max_speed * sgn(buffer[i].speed_y) : buffer[i].speed_y;

			buffer[i].pos.x += buffer[i].speed * elapsed;
			buffer[i].pos.y += buffer[i].speed_y * elapsed;

			buffer[i].rotate = Matrix22f(cosf(buffer[i].rotation_angle), -sinf(buffer[i].rotation_angle),
				sinf(buffer[i].rotation_angle), cosf(buffer[i].rotation_angle));
			buffer[i].scale[0][0] = buffer[i].scale_indx;
			buffer[i].scale[1][1] = buffer[i].scale_indx;
		}});

	// add some particles
	for (int i = 0; i < 200; i++)
		for (int j = 0; j < 200; j++)
			add_particles(particles, 1, vec2f(-2 + 0.02 * j, -2 + 0.02 * i), 0.01, vec2f(0, 1), PI * 2);
#endif




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
		// clear screen
		draw_filled_rect(0, 0, surface.width, surface.height, Color(0, 0, 0));

#if mod == 0
		// Simulate
		add_particles(particles, rand() % 30, mouse.pos, 0.1, vec2f(0, 1), 2*PI, float(rand() % 30 + 20) / 100, float(rand() % 200) / 100);
		particles.calculate();
		// Draw
		// draw particles
		for (int i = 0; i < particles.actives; i++)
		{
			uni_life_time = particles[i].life_time / particles[i].whole_life;
			draw_filled_shape(shapes[particles[i].shape], &particles[i], &shader);
		}
#elif mod == 1 || mod == 2
		mouse_pos = mouse.pos;
		particles.calculate();
		for (int i = 0; i < particles.actives; i++)
		{
			uni_dist = (mouse_pos - particles[i].pos).norm();
			uni_dist = uni_dist > 1 ? 1 : uni_dist;
			draw_filled_shape(shapes[particles[i].shape], &particles[i], &shader);
		}
#endif
		
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