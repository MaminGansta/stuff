#include <Windows.h>
#include <cstdint>
#include <limits>
#include <cmath>
#include <vector>
#include <algorithm>
#include <utility>
#include <random>
#include <map>

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
#include "triangulation_stuff.cpp"



int orientation(vec2f p, vec2f q, vec2f r)
{
	float val = (q.y - p.y) * (r.x - q.x) -
		(q.x - p.x) * (r.y - q.y);

	if (val == 0) return 0;  // colinear 
	return (val > 0) ? 1 : 2; // clock or counterclock wise 
}

std::vector<vec2f> convexHull(std::vector<vec2f>& points)
{
	// There must be at least 3 points 
	if (points.size() < 3) return points;

	// Initialize Result 
	std::vector<vec2f> hull;

	// Find the leftmost point 
	int l = 0;
	for (int i = 1; i < points.size(); i++)
		if (points[i].x < points[l].x)
			l = i;

	int p = l, q;
	do
	{
		hull.push_back(points[p]);

		q = (p + 1) % points.size();
		for (int i = 0; i < points.size(); i++)
		{
			if (orientation(points[p], points[i], points[q]) == 2)
				q = i;
		}

		p = q;

	} while (p != l);  // While we don't come to first point 

	return hull;
}


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
	window_class.lpszClassName = "triangulation";
	window_class.lpfnWndProc = win_callback;

	RegisterClass(&window_class);

	HWND window = CreateWindow(window_class.lpszClassName, "triangulation", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	Mouse_Input mouse;
	Key_Input input;
	Timer timer;


	lines_shader shader;

	// some shapes
	std::vector<vec2f> shapes[] = {
		std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
		std::vector<vec2f> { vec2f(0.02, 0.04), vec2f(0, 0.02), vec2f(-0.05, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.01), vec2f(0.05, -0.02), vec2f(0.05, 0) },
		std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
		std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.02, 0.05), vec2f(0, 0.01), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
		std::vector<vec2f> { vec2f(0.01, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04),vec2f(-0.04, 0.05), vec2f(-0.03, -0.01), vec2f(-0.1, 0.02) , vec2f(-0.05, -0.02), vec2f(0.01, -0.07), vec2f(0.05, -0.02) , vec2f(0.1, 0), vec2f(0.02, 0.01) }
	};


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
		static int a = 4;
		//a = (a + 1) % 5;
		std::vector<vec2f> shape = shapes[a];

		object obj(vec2f(0, 0));
		obj.scale = Matrix22f(3, 0, 0, 3);

		std::vector<vec3i> faces = triangulate(shape);

		// Draw
		// clear screen
		draw_filled_rect(0, 0, surface.width, surface.height, Color(0, 0, 0));

		draw_shape(shape, obj, &shader);

		// draw lines
		for (int i = 0; i < faces.size(); i++)
		{
			std::vector<vec2f> pts;
			pts.push_back(shape[faces[i].x]);
			pts.push_back(shape[faces[i].y]);
			pts.push_back(shape[faces[i].z]);

			draw_shape(pts, obj, &shader);
			draw_filled_circle(shape[faces[i].x] * 3, 0.005, Color(255, 0, 0));
			Sleep(500);
			StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		}

		// Render
		//StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

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