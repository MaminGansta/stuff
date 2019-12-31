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
#include "SAT.cpp"
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
	window_class.lpszClassName = "Asteroids";
	window_class.lpfnWndProc = win_callback;

	RegisterClass(&window_class);

	HWND window = CreateWindow(window_class.lpszClassName, "Asteroids", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, 0, 0, hInstance, 0);
	HDC hdc = GetDC(window);

	Mouse_Input mouse;
	Key_Input input;
	Timer timer;

	Ship ship;
	line_shader ship_shader;

	// stars
	Star_shader star_shader;
	particles_buffer stars(20);
	for (int i = 0; i < 20; i++)
		add_particles(stars, 1, vec2f(float(rand() % 50) / 50 - 0.5, float(rand() % 50) / 50 - 0.5), 0.03);

	// rocket flame
	flame_shader flame_shader;
	particles_buffer flame(1000);
	particles_buffer asteroids_blow(200);

	// asteroids
	Asteroid_buffer asteroids(30);
	float asteroid_add_deley = 0;

	// bullets
	bullet_buffer<10> bullets;
	float shot_delay = 0;

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
				case VK_SPACE:
				{
					input.buttons[BUTTON_SPACE].changed = input.buttons[BUTTON_DOWN].is_down != is_down;
					input.buttons[BUTTON_SPACE].is_down = is_down;
				}break;
				case VK_ESCAPE:
				{
					running = false;
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


			for (int i = 0; i < 5; i++)
				add_particles(flame, rand() % 3, ship.pos, 0.05, vec2f(-cosf(ship.angle), -sinf(ship.angle)), PI / 2.5, float(rand() % 30 + 20) / 100, float(rand() % 130) / 100);
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
		stars.calculate();
		flame.calculate();
		asteroids.calculate();
		bullets.calculate();

		for (int i = 0; i < asteroids.actives; i++)
		{
			float dist = (asteroids[i].pos - ship.pos).norm();

			if (dist < 0.2)
				if (SAT_shape(ship.global, ship.faces, asteroids[i].global, asteroids[i].faces))
				{
					uni_shape_color = Color(255, 0, 0);
					break;
				}
				else
					uni_shape_color = Color(255, 255, 255);
		}

		// add asteroid if nessecery
		if (asteroid_add_deley < 0)
		{
			add_asteroid(asteroids, 1, PI / 3);
			asteroid_add_deley = 0.4;
		}
		asteroid_add_deley -= elapsed;

		// add bullet
		shot_delay -= elapsed;
		if (input.buttons[BUTTON_SPACE].is_down && input.buttons[BUTTON_SPACE].changed && shot_delay < 0)
		{
			bullets.add_bullet(ship.global[0], ship.angle);
			shot_delay = 0.3;
		}

		// bullet handle
		for (int j = 0; j < bullets.actives; j++)
		{
			for (int i = 0; i < asteroids.actives; i++)
			{
				float dist = (bullets[j].pos - asteroids[i].pos).norm();

				if (dist < 0.2)
				{
					for (int a = 0; a < asteroids[i].faces.size(); a++)
					{
						vec3i face = asteroids[i].faces[a];
						vec3f out;
						if (fbarycentric(vec3f(asteroids[i].global[face.x]), vec3f(asteroids[i].global[face.y]), vec3f(asteroids[i].global[face.z]), vec3f(bullets[j].pos), &out))
						{
							// add small asteroids
							if (asteroids[i].scale[0][0] != 0.5)
								for  (int v = 0; v < 2; v++)
									add_asteroid(asteroids, 0.5, 2 * PI, asteroids[i].pos);

							for (int h = 0; h < 30; h++)
								add_particles(asteroids_blow, rand() % 3, asteroids[i].pos, 0.02, vec2f(0,1), 2*PI, float(rand() % 40 + 30) / 100, float(rand() % 130) / 100);

							asteroids.delete_asteroid(i--);
							bullets.delete_bullet(j--);
							

							break;
						}
					}
				}
			}
		}

		// ateroids handler
		for (int i = 0; i < asteroids.actives; i++)
		{
			if (fabs(asteroids[i].pos.x) > 0.8 || fabs(asteroids[i].pos.y) > 0.6)
			{
				using std::swap;
				swap(asteroids[i--], asteroids[--asteroids.actives]);
			}
		}

		// asteroids blow up particles calculation
		asteroids_blow.calculate();
		
		// Draw
		// clear screen
		draw_filled_rect(0, 0, surface.width, surface.height, Color(0, 0, 0));

		// draw stars
		for (int i = 0; i < stars.actives; i++)
		{
			uni_star_life = stars[i].life_time;
			draw_filled_shape(shapes[stars[i].shape], stars[i], &star_shader);
		}

		//draw flame
		uni_particles_color = Color(255, 60, 0);
		for (int i = 0; i < flame.actives; i++)
		{
			uni_life_time = flame[i].life_time > 1 ? 1 : flame[i].life_time;
			draw_filled_shape(shapes[flame[i].shape], flame[i], &flame_shader);
		}

		// draw ship
		draw_shape(ship.local, ship, &ship_shader);

		// draw asteroids
		for (int i = 0; i < asteroids.actives; i++)
			draw_shape(asteroids[i].local, asteroids[i], &ship_shader);

		// draw bullets
		for (int i = 0; i < bullets.actives; i++)
			draw_filled_circle(bullets[i].pos, 0.005, Color(255, 255, 255));

		// draw asteroid blow up
		uni_particles_color = Color(255, 255, 255);
		for (int i = 0; i < asteroids_blow.actives; i++)
		{
			uni_life_time = asteroids_blow[i].life_time > 1 ? 1 : asteroids_blow[i].life_time;
			draw_filled_shape(shapes[asteroids_blow[i].shape], asteroids_blow[i], &flame_shader);
		}



		// Render
		StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		// Timer
		timer.update();
		elapsed = timer.elapsed;

		// Log
		char log[128];
		sprintf_s(log, "fps: %d ftime: %.3f sec  flame_particles: %d \n", timer.FPS, timer.elapsed, flame.actives);
		OutputDebugStringA(log);

	}

	return 0;
}