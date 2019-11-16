#include <windows.h>

bool running = true;

#include "render_state.cpp"
#include "geometry.cpp"
#include "draw.cpp"
#include "timer.cpp"
#include "mouse_input.cpp"


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
	window_class.lpszClassName = "Game";
	window_class.lpfnWndProc = win_callback;

	// reg window
	RegisterClass(&window_class);

	// create window
	HWND window = CreateWindow(window_class.lpszClassName, "Game!!!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInst, 0);
	HDC hdc = GetDC(window);

	// mouse input
	Mouse_Input mouse_input;


	Timer timer;
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
					mouse_input.x = int(LOWORD(msg.lParam));
					mouse_input.y = surface.height - int(HIWORD(msg.lParam));
				}break;
				case WM_LBUTTONDOWN:
				{
					OutputDebugString("mouse down\n");
					mouse_input.buttons[LBUTTON].change = !mouse_input.buttons[LBUTTON].is_dawn;
					mouse_input.buttons[LBUTTON].is_dawn = true;
				}break;
				case WM_LBUTTONUP:
				{
					OutputDebugString("mouse relised\n");
					mouse_input.buttons[LBUTTON].change = mouse_input.buttons[LBUTTON].is_dawn;
					mouse_input.buttons[LBUTTON].is_dawn = false;
				}break;
				default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

		// Simulate
		if (mouse_input.buttons[LBUTTON].is_dawn)
			draw_filled_circle(mouse_input.x, mouse_input.y, 30,Color(255, 0, 0));



		// timer
		timer.update();

		// Render
		StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

	}

	return 0;
}