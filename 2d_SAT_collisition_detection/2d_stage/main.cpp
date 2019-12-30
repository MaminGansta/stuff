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

int cur_shape = 0;
std::vector<std::pair<int, int>> overlapped;


#include "render_stuff.cpp"
#include "geometry.cpp"
#include "particles.cpp"
#include "shader.cpp"
#include "draw.cpp"
#include "input.cpp"
#include "timer.cpp"
#include "triangulation_stuff.cpp"
#include "SAT.cpp"


struct Ship : public object
{
	std::vector<vec2f> pts{ vec2f(0.03, 0), vec2f(-0.02, 0.02), vec2f(-0.02, -0.02) };
	//std::vector<vec2f> pts{ vec2f(0.01, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04),vec2f(-0.04, 0.05), vec2f(-0.03, -0.01), vec2f(-0.1, 0.02) , vec2f(-0.05, -0.02), vec2f(0.01, -0.07), vec2f(0.05, -0.02) , vec2f(0.1, 0), vec2f(0.02, 0.01) };

	float angle{ PI / 2 };
	float speed_x{ 0 };
	float speed_y{ 0 };
	float speed_dif{ 0.5 };

	void calculate()
	{
		if (fabs(pos.x) > 0.7)
			pos.x = sgn(-pos.x) * 0.69;

		if (fabs(pos.y) > 0.5)
			pos.y = sgn(-pos.y) * 0.49;


		pos.x += speed_x * elapsed;
		pos.y += speed_y * elapsed;
		rotate = Matrix22f(cosf(angle), -sinf(angle),
			sinf(angle), cosf(angle));
	}

};

struct Shape : public object
{
	std::vector<vec2f> local;
	std::vector<vec2f> global;
	std::vector<vec3i> faces;
	float angle{ PI / 2 };

	Shape(std::vector<vec2f> local, vec2f pos) : local(local), global(local), object(pos){}

	void calculate()
	{
		for (int i = 0; i < local.size(); i++)
			global[i] = pos + scale * local[i];
	}

};


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
	std::vector<vec2f> asteroids[] = {
		std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
		std::vector<vec2f> { vec2f(0.02, 0.04), vec2f(0, 0.02), vec2f(-0.05, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.01), vec2f(0.05, -0.02), vec2f(0.05, 0) },
		std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
		std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.02, 0.05), vec2f(0, 0.01), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
		std::vector<vec2f> { vec2f(0.01, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04),vec2f(-0.04, 0.05), vec2f(-0.03, -0.01), vec2f(-0.1, 0.02) , vec2f(-0.05, -0.02), vec2f(0.01, -0.07), vec2f(0.05, -0.02) , vec2f(0.1, 0), vec2f(0.02, 0.01) }
	};

	Ship ship;

	std::vector<Shape> shapes;
	for (int i = 0; i < 5; i++)
	{
		shapes.push_back(Shape(asteroids[i], vec2f(-0.5 + 0.2*i, 0)));
		shapes[i].faces = triangulate(asteroids[i]);
		shapes[i].scale = Matrix22f(1, 0, 0, 1);
	}


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
#define MAX_SPEED 1
		// input handler
		if (input.buttons[BUTTON_UP].is_down)
		{
			ship.speed_x += ship.speed_dif * cosf(ship.angle) * elapsed;
			ship.speed_x = fabs(ship.speed_x) > MAX_SPEED ? sgn(ship.speed_x) * MAX_SPEED : ship.speed_x;
			ship.speed_y += ship.speed_dif * sinf(ship.angle) * elapsed;
			ship.speed_y = fabs(ship.speed_y) > MAX_SPEED ? sgn(ship.speed_y) * MAX_SPEED : ship.speed_y;
		}
		else
		{
			ship.speed_x = fabs(ship.speed_x) < 0.00001 ? 0 : ship.speed_x + sgn(-ship.speed_x) * ship.speed_dif * elapsed;
			ship.speed_y = fabs(ship.speed_y) < 0.00001 ? 0 : ship.speed_y + sgn(-ship.speed_y) * ship.speed_dif * elapsed;
		}

		if (input.buttons[BUTTON_LEFT].is_down)
			ship.angle += PI * elapsed;

		if (input.buttons[BUTTON_RIGHT].is_down)
			ship.angle -= PI * elapsed;

		ship.calculate();


		// global for ship
		std::vector<vec3i> ship_faces = triangulate(ship.pts);
		std::vector<vec2f> ship_global_pts;
		for (vec2f local : ship.pts)
		{
			ship_global_pts.push_back(ship.pos + ship.rotate * local);
		}

		// shape matrix prepatring
		for (int i = 0; i < shapes.size(); i++)
			shapes[i].calculate();


		overlapped.clear();
		// SAT
		for (int i = 0; i < shapes.size(); i++)
		{
			float dist = (shapes[i].pos - ship.pos).norm();
			cur_shape = i;
			
			if (dist < 0.2)
			if (SAT_shape(ship_global_pts, ship_faces, shapes[i].global, shapes[i].faces))
			{
				shape_color = Color(255, 0, 0);
				break;
			}
			else
				shape_color = Color(255, 255, 255);
		}

		// Draw
		// clear screen
		draw_filled_rect(0, 0, surface.width, surface.height, Color(0, 0, 0));

		// draw ship
		draw_shape(ship.pts, ship, &shader);

		// draw asteroids
		for (int i = 0; i < shapes.size(); i++)
		{
			draw_shape(shapes[i].local, shapes[i], &shader);

			// draw shape tringlulation
			//for (int j = 0; j < shapes[i].faces.size(); j++)
			//{
			//	std::vector<vec2f> pts;
			//	for (int a = 0; a < 3; a++)
			//		pts.push_back(shapes[i].local[shapes[i].faces[j][a]]);
			//	draw_shape(pts, shapes[i], &shader);
			//}
		}

		// draw overlapped parts
		for (int i = 0; i < overlapped.size(); i++)
		{
			std::vector<vec2f> pts;
			for (int j = 0; j < 3; j++)
			{
				pts.push_back(shapes[overlapped[i].first].local[shapes[overlapped[i].first].faces[overlapped[i].second][j]]);
			}

			shape_color = Color(255, 0, 0);
			draw_shape(pts, shapes[overlapped[i].first], &shader);
		}

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