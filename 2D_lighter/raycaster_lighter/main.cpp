#include <windows.h>
#include <array>
#include <vector>
#include <algorithm>
#include <cstdint>


// options ------------------------------------------------
#define SMALL_SCREEN  0x00 // 400 x 400  ELSE 1200 x 720
#define BIG_SCREEN  0x01

#define LINES  0x00
#define TRIANGLES  0x01

#define SCREEN_MODE  BIG_SCREEN  // CHANGE THIS PARAM TO THE BIG_SCREEN
#define DRAW_MODE  TRIANGLES     // CHANGE TO LINES

// draw defines  ------------------------------------------
#define ADDITION  0x00
#define FULL  0x01
// --------------------------------------------------------

// direction to angle
float pseudoangle(float dx, float dy)
{
	float p = dx / (fabs(dx) + fabs(dy));// #   - 1 .. 1 increasing with x
	if (dy < 0)
		return p - 1; //  #   - 2 .. 0 increasing with x
	else
		return 1 - p; //  #  0 .. 2 decreasing with x
}


// Global varibals
#define PI 3.14159265359
bool running = true;

#include "render_state.cpp"
#include "geometry.cpp"
#include "draw.cpp"
#include "timer.cpp"
#include "mouse_input.cpp"
#include "shapes.cpp"


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
	window_class.lpszClassName = "lighter";
	window_class.lpfnWndProc = win_callback;

	// reg window
	RegisterClass(&window_class);

	// create window
#if SCREEN_MODE == SMALL_SCREEN
	HWND window = CreateWindow(window_class.lpszClassName, "lighter", WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 400, 400, 0, 0, hInst, 0);
#elif SCREEN_MODE == BIG_SCREEN
	HWND window = CreateWindow(window_class.lpszClassName, "lighter",  WS_MINIMIZEBOX | WS_SYSMENU | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInst, 0);
#endif


	HDC hdc = GetDC(window);


	// shapes
	std::vector<Line> shapes;
	shapes.reserve(30);
	add_some_shapes(shapes);

	// ray
	//Line ray(Vert2f(surface.width / 2, surface.height / 2), Vec2f(), 200);

#if SCREEN_MODE == SMALL_SCREEN
	std::vector<std::vector<Line>> lighters(5, std::vector<Line>());
	for (std::vector<Line> rays : lighters)
		rays.reserve(50);
#else
	std::vector<Line> rays;
	rays.reserve(50);
#endif
	// Legasy N1  static direction of lines
	//for (int i = 0; i < 50; i++)
	//{
	//	rays.push_back(Line(Vert2f(), Vec2f(cosf(i * 2*PI / 50), sinf(i * 2*PI / 50)), 1));
	//}

	// mouse input
	Mouse_Input mouse;

	// timer
	Timer timer(false);
	while (running)
	{
		// Input
		MSG msg;
		while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
			case WM_MOUSEMOVE:
			{
				mouse.x = int(LOWORD(msg.lParam));
				mouse.y = surface.height - int(HIWORD(msg.lParam));
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

		// Simulate -----------------------------------------------------------
		if (surface.width == 0 || surface.height == 0) continue; // window is minimazed


		// calturate the ray
		//Vec2f dir(mouse.x - ray.pos.x, mouse.y - ray.pos.y);
		//ray.dir = dir.normalize();



		// 5 lighters at small screen mode
#if SCREEN_MODE == SMALL_SCREEN
		for (auto& rays : lighters)
			if (rays.size() < shapes.size())
				rays.resize(3 * shapes.size());


		// instead Legasy N1 dynamic ray diraction
		// cast to the begin of the lines
		for (int i = 0; i < lighters.size(); i++)
		{
			for (Line& ray : lighters[i])
			{
				ray.pos.x = mouse.x + cosf(i * 2*PI / 5) * 12;
				ray.pos.y = mouse.y + sinf(i * 2*PI / 5) * 12;
			}
		}


		// directions of rays + 2 rays with +-0.1
		for (auto& rays : lighters)
		{
			int j = 0;
			for (int i = 0; i < shapes.size(); i++)
			{
				// ray to corner
				rays[j].dir.x = shapes[i].pos.x - rays[i].pos.x;
				rays[j++].dir.y = shapes[i].pos.y - rays[i].pos.y;


				// two rays +- small angle
				// - A where A < PI / 100
				rays[j++].dir = Vec2f(shapes[i].pos.x - rays[i].pos.x - 0.05, shapes[i].pos.y - rays[i].pos.y - 0.05).normalize();

				//  + A
				rays[j++].dir = Vec2f(shapes[i].pos.x - rays[i].pos.x + 0.05, shapes[i].pos.y - rays[i].pos.y + 0.05).normalize();

			}
		}


		// calculate the distance to secment of shape
		for (auto& rays : lighters)
		{
			for (Line& ray : rays)
			{
				// segment of shape: coef of lines intersection 0 < T2 < 1
				float T2 = 2000;
				// ray: T1 > 0
				float T1 = 2000;

				for (Line line : shapes)
				{
					float new_T2 = (ray.dir.x * (line.pos.y - ray.pos.y) + ray.dir.y * (ray.pos.x - line.pos.x)) /
						(line.dir.x * ray.dir.y - line.dir.y * ray.dir.x);

					// clossest srgment
					if (new_T2 >= 0 && new_T2 <= 1)
					{
						T2 = new_T2;
						float new_T1 = (line.pos.x + line.dir.x * T2 - ray.pos.x) / ray.dir.x;
						if (new_T1 < T1 && new_T1 >= 0)
							T1 = new_T1;
					}
				}

				ray.lenght = T1;
			}


			for (auto& rays : lighters)
			{
				std::sort(rays.begin(), rays.end(), [](Line a, Line b)->bool
				{
					float angle_a = pseudoangle(a.dir.x, a.dir.y);
					float angle_b = pseudoangle(b.dir.x, b.dir.y);
					return angle_a < angle_b;
				});
			}
		}



#elif SCREEN_MODE == BIG_SCREEN

		if (rays.size() < shapes.size())
			rays.resize(3 * shapes.size());
		
		for (Line& ray : rays)
		{
			ray.pos.x = mouse.x, ray.pos.y = mouse.y;
		}


		int j = 0;
		for (int i = 0; i < shapes.size(); i++)
		{
			// ray to corner
			rays[j].dir.x = shapes[i].pos.x - rays[i].pos.x;
			rays[j++].dir.y = shapes[i].pos.y - rays[i].pos.y;


			// two rays +- small angle
			// - A where A < PI / 100
			rays[j++].dir = Vec2f(shapes[i].pos.x - rays[i].pos.x - 0.1, shapes[i].pos.y - rays[i].pos.y - 0.1).normalize();

			//  + A
			rays[j++].dir = Vec2f(shapes[i].pos.x - rays[i].pos.x + 0.1, shapes[i].pos.y - rays[i].pos.y + 0.1).normalize();

		}

		std::sort(rays.begin(), rays.end(), [](Line a, Line b)->bool
		{
			float angle_a = pseudoangle(a.dir.x, a.dir.y);
			float angle_b = pseudoangle(b.dir.x, b.dir.y);

			return angle_a < angle_b;
		});

		for (Line& ray : rays)
		{
			// segment of shape: coef of lines intersection 0 < T2 < 1
			float T2 = 2000;
			// ray: T1 > 0
			float T1 = 2000;

			for (Line line : shapes)
			{
				float new_T2 = (ray.dir.x * (line.pos.y - ray.pos.y) + ray.dir.y * (ray.pos.x - line.pos.x)) /
					(line.dir.x * ray.dir.y - line.dir.y * ray.dir.x);

				// clossest srgment
				if (new_T2 >= 0 && new_T2 <= 1)
				{
					T2 = new_T2;
					float new_T1 = (line.pos.x + line.dir.x * T2 - ray.pos.x) / ray.dir.x;
					if (new_T1 < T1 && new_T1 >= 0)
						T1 = new_T1;
				}
			}

			ray.lenght = T1;
		}
#endif


		// Draw ---------------------------------------------------------------

		// clear screen
		draw_filled_rect(0, 0, surface.width, surface.height, Color(0, 0, 0));

		// draw shapes
		for (Line line : shapes)
			line.draw(Color(255, 255, 255));



#if SCREEN_MODE == SMALL_SCREEN
#if DRAW_MODE == LINES
		// draw ray
		for (auto rays : lighters)
		{
			for (Line ray : rays)
			{
				ray.draw();
				// intersection point
				draw_filled_circle(ray.pos.x + ray.dir.x * ray.lenght, ray.pos.y + ray.dir.y * ray.lenght, 5, Color(255, 0, 0));
			}
		}
#elif DRAW_MODE == TRIANGLES
		// draw triangle
		for (auto rays : lighters)
		{
			for (int i = 0; i < rays.size() - 1; i++)
			{
				Vec3f pts[]{ rays[i].pos,
							 Vec2f(rays[i].pos.x + rays[i].dir.x * rays[i].lenght, rays[i].pos.y + rays[i].dir.y * rays[i].lenght),
							 Vec2f(rays[i + 1].pos.x + rays[i + 1].dir.x * rays[i + 1].lenght, rays[i + 1].pos.y + rays[i + 1].dir.y * rays[i + 1].lenght) };

				draw_triangle(pts, Color(88, 88, 88), ADDITION);

			}

			// for end and begin triangle
			{
				Vec3f pts[]{ rays[0].pos,
						 Vec2f(rays[0].pos.x + rays[0].dir.x * rays[0].lenght, rays[0].pos.y + rays[0].dir.y * rays[0].lenght),
						 Vec2f(rays.back().pos.x + rays.back().dir.x * rays.back().lenght, rays.back().pos.y + rays.back().dir.y * rays.back().lenght) };

				draw_triangle(pts, Color(88, 88, 88), ADDITION);

			}
		}

		// draw origin of lighter
		for (auto lighter : lighters)
			draw_filled_circle(lighter[0].pos.x, lighter[0].pos.y, 2, Color(255, 0, 0));
#endif
#elif SCREEN_MODE == BIG_SCREEN
#if DRAW_MODE == LINES
		// draw ray
		for (Line ray : rays)
		{
			ray.draw();
			// intersection point
			draw_filled_circle(ray.pos.x + ray.dir.x * ray.lenght, ray.pos.y + ray.dir.y * ray.lenght, 5, Color(255, 0, 0));
		}
#elif DRAW_MODE == TRIANGLES
		// draw triangle
		for (int i = 0; i < rays.size() - 1; i++)
		{
			Vec3f pts[]{ rays[i].pos,
						 Vec2f(rays[i].pos.x + rays[i].dir.x * rays[i].lenght, rays[i].pos.y + rays[i].dir.y * rays[i].lenght),
						 Vec2f(rays[i + 1].pos.x + rays[i + 1].dir.x * rays[i + 1].lenght, rays[i + 1].pos.y + rays[i + 1].dir.y * rays[i + 1].lenght) };

			draw_triangle(pts, Color(255, 255, 255));

			//rays[i].draw(Color(255, 0, 0));
		}

		// for end and begin triangle
		{
			Vec3f pts[]{ rays[0].pos,
					 Vec2f(rays[0].pos.x + rays[0].dir.x * rays[0].lenght, rays[0].pos.y + rays[0].dir.y * rays[0].lenght),
					 Vec2f(rays.back().pos.x + rays.back().dir.x * rays.back().lenght, rays.back().pos.y + rays.back().dir.y * rays.back().lenght) };

			draw_triangle(pts, Color(255, 255, 255));

			//rays.back().draw(Color(255, 0, 0));
		}
#endif
#endif

		// timer
		timer.update();

		// Render
		StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		// log
		char log[20];
		sprintf_s(log, "%d  %.4f\n", timer.FPS, timer.elapsed);
		OutputDebugString(log);

	}

	return 0;
}