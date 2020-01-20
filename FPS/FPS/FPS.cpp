#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS

#include <cstdio>
#include <Windows.h>
#include <string>
#include <vector>
#include <algorithm>
#include "Timer.h"

using namespace my::Timer;

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 4.0f;
float fPlayerY = 4.0f;
float fPlayerA = 0.0f;

int nMapHeight = 16;
int nMapWidth = 16;

float fFOV = 3.14159 / 4.0; 
float fDepth = 16.0f;


int main(void)
{
	// create console screen
	char* pScreen = new char[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// set font size
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 0;
	cfi.dwFontSize.Y = 16;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font
	SetCurrentConsoleFontEx(hConsole, false, &cfi);

	// set window size
	SMALL_RECT windowSize = { 0, 0, nScreenHeight, nScreenWidth };
	SetConsoleWindowInfo(hConsole, TRUE, &windowSize);


	std::string map;
	map += "XXXXXXXXXXXXXXXX";
	map += "X..............X";
	map += "X..............X";
	map += "X..............X";
	map += "X..............X";
	map += "X..............X";
	map += "X..............X";
	map += "X..........XXXXX";
	map += "X..............X";
	map += "X..............X";
	map += "X..............X";
	map += "X.....XXXX.....X";
	map += "X..............X";
	map += "X..............X";
	map += "X..............X";
	map += "XXXXXXXXXXXXXXXX";


	timer_start();

	while (1)
	{

		// key input
		if (GetAsyncKeyState('A') & 0x0001)
			fPlayerA -= 0.00001f * nFrameTime;

		if (GetAsyncKeyState('D') & 0x0001)
			fPlayerA += 0.00001f * nFrameTime;

		if (GetAsyncKeyState('W') & 0x0001)
		{
			fPlayerX += 0.00003f * sinf(fPlayerA) * nFrameTime;
			fPlayerY += 0.00003f * cosf(fPlayerA) * nFrameTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == 'X')
			{
				fPlayerX -= 0.00003f * sinf(fPlayerA) * nFrameTime;
				fPlayerY -= 0.000030f * cosf(fPlayerA) * nFrameTime;
			}
		}

		if (GetAsyncKeyState('S') & 0x0001)
		{
			fPlayerX -= 0.00003f * sinf(fPlayerA) * nFrameTime;
			fPlayerY -= 0.00003f * cosf(fPlayerA) * nFrameTime;

			if (map[(int)fPlayerY * nMapWidth + (int)fPlayerX] == 'X')
			{
				fPlayerX += 0.000031f * sinf(fPlayerA) * nFrameTime;
				fPlayerY += 0.000031f * cosf(fPlayerA) * nFrameTime;
			}
		}


		for (int x = 0; x < nScreenWidth; x++)
		{
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			float fDistanceToWall = 0;
			bool  bHitWall = false;
			bool bBoundary = false;

			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

				
			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += 0.5f;

				int nTestX = (int)(fPlayerX + fDistanceToWall * fEyeX);
				int nTestY = (int)(fPlayerY + fDistanceToWall * fEyeY);

				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else
				{
					if (map[nTestY * nMapWidth + nTestX] == 'X')
					{
						bHitWall = true;

						std::vector<std::pair<float, float>> p;

						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								// Angle of corner to eye
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(std::make_pair(d, dot));
							}

						std::sort(p.begin(), p.end(),
							[](const std::pair<float, float>& left, const std::pair<float, float>& right) {return left.first < right.first; });

						float fBound = 0.007f;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}

			int nCeiling = (float)(nScreenHeight / 2.0f) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			for (int y = 0; y < nScreenHeight; y++)
			{
				int nShade = ' ';

				if (fDistanceToWall <= fDepth / 4.0f)        nShade = 219;
				else if (fDistanceToWall <= fDepth / 3.0f)   nShade = 178;
				else if (fDistanceToWall <= fDepth / 2.0f)   nShade = 177;
				else if (fDistanceToWall <= fDepth / 1.5f)   nShade = 176;
				else										 nShade = ' ';

				if (bBoundary)
					nShade = ' ';

				if (y < nCeiling)
					pScreen[y * nScreenWidth + x] = ' ';
				else if (y > nCeiling && y <= nFloor)
					pScreen[y * nScreenWidth + x] = nShade;
				else
				{
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)		nShade = '#';
					else if (b < 0.5)	nShade = 'x';
					else if (b < 0.75)	nShade = '.';
					else if (b < 0.9)	nShade = '-';
					else				nShade = ' ';
					pScreen[y * nScreenWidth + x] = nShade;
				}
			}
		}

		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
				pScreen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];

		pScreen[((int)fPlayerX + 1) * nScreenWidth + (int)fPlayerY] = 'P'; // player on a map


		sprintf(pScreen, "  Alpha %f  PosX %f  PosY %f    FPS %d  FrameTime %0.1f  LOCK: %s",
				fPlayerA, fPlayerX , fPlayerY, nFPS, fFrameTimeAvg, bFrameLock ? "YES" : "NO");
		WriteConsoleOutputCharacterA(hConsole, pScreen, nScreenHeight * nScreenWidth, {0, 0}, &dwBytesWritten);

		// Time
		timer_update();

	}

	return 0;
}