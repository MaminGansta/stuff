
inline void drawPixel(int x, int y, Color color) {
	// cheker
	x = MIN(x, surface.width - 1);
	x = MAX(x, 0);
	y = MIN(y, surface.height - 1);
	y = MAX(y, 0);

	surface.memory[y * surface.width + x] = color.whole;
}

inline Color getPixel(int x, int y) {
	// cheker
	x = MIN(x, surface.width - 1);
	x = MAX(x, 0);
	y = MIN(y, surface.height - 1);
	y = MAX(y, 0);

	return *(Color*)&surface.memory[y * surface.width + x];
}


inline void drawLine(int x, int y, int x2, int y2, Color color) {
	bool yLonger = false;
	int shortLen = y2 - y;
	int longLen = x2 - x;
	if (abs(shortLen) > abs(longLen)) {
		int swap = shortLen;
		shortLen = longLen;
		longLen = swap;
		yLonger = true;
	}
	int decInc;
	if (longLen == 0) decInc = 0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen > 0) {
			longLen += y;
			for (int j = 0x8000 + (x << 16); y <= longLen; ++y) {
				drawPixel(j >> 16, y, color);
				j += decInc;
			}
			return;
		}
		longLen += y;
		for (int j = 0x8000 + (x << 16); y >= longLen; --y) {
			drawPixel(j >> 16, y, color);
			j -= decInc;
		}
		return;
	}

	if (longLen > 0) {
		longLen += x;
		for (int j = 0x8000 + (y << 16); x <= longLen; ++x) {
			drawPixel(x, j >> 16, color);
			j += decInc;
		}
		return;
	}
	longLen += x;
	for (int j = 0x8000 + (y << 16); x >= longLen; --x) {
		drawPixel(x, j >> 16, color);
		j -= decInc;
	}
}

inline void drawSquare(int x, int y, int x2, int y2, Color color) {
	drawLine(x, y, x2, y2, color);
	drawLine(x2, y2, x2 + (y - y2), y2 + (x2 - x), color);
	drawLine(x, y, x + (y - y2), y + (x2 - x), color);
	drawLine(x + (y - y2), y + (x2 - x), x2 + (y - y2), y2 + (x2 - x), color);
}

inline void drawRect(int x, int y, int x2, int y2, Color color) {
	drawLine(x, y, x2, y, color);
	drawLine(x2, y, x2, y2, color);
	drawLine(x2, y2, x, y2, color);
	drawLine(x, y2, x, y, color);
}

inline void draw_filled_rect(int x0, int y0, int x1, int y1, Color color)
{
	for (int y = y0; y < y1; y++)
		for (int x = x0; x < x1; x++)
			drawPixel(x, y, color);

}

inline void draw_filled_circle(int X, int Y, int radius, Color color)
{
	//int min_y = MAX(Y - radius, 0);
	//int max_y = MIN(Y + radius, surface.height);
	//int min_x = MAX(X - radius, 0);
	//int max_x = MIN(X + radius, surface.width);

	for (int y = -radius; y <= radius; y++)
	{
		for (int x = -radius; x <= radius; x++)
		{
			// end of user area
			if (Y + y < 0 || Y + y > surface.height - 1) continue;
			if (X + x < 0 || X + x > surface.width - 1) continue;

			// + 1 make it more slightly
			if (x * x + y * y <= radius * radius + 1)
				drawPixel(X + x, Y + y, color);
		}
	}
}


inline void draw_triangle(Vec3f* pts, Color color, uint8_t mode = FULL)
{
	if (pts[0].y == pts[1].y && pts[0].y == pts[2].y) return; // i don't care about degenerate triangles
	if (pts[0].y > pts[1].y) { std::swap(pts[0], pts[1]); }
	if (pts[0].y > pts[2].y) { std::swap(pts[0], pts[2]); }
	if (pts[1].y > pts[2].y) { std::swap(pts[1], pts[2]); }

	Vert2f bot_left(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	Vert2f top_right(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bot_left[j] = MAX(0.0f, MIN(bot_left[j], pts[i][j]));
			top_right[j] = MIN(surface.width, MAX(top_right[j], pts[i][j]));
		}
	}
	Vert3i P;
	for (P.x = bot_left.x; P.x < top_right.x; P.x += 1.0f)
	{
		for (P.y = bot_left.y; P.y < top_right.y; P.y += 1.0f)
		{
			Vec3f bar;
			if (barycentric(pts[0], pts[1], pts[2], P, &bar))
			{
				if (mode == FULL)
				{
					drawPixel(P.x, P.y, color);
				}
				else if (mode == ADDITION)
				{
					Color out;
					Color screen_pixel = getPixel(P.x, P.y);
				
					out.r = MIN(255, ((uint16_t)color.r + screen_pixel.r));
					out.g = MIN(255, ((uint16_t)color.g + screen_pixel.g));
					out.b = MIN(255, ((uint16_t)color.b + screen_pixel.b));
					
					drawPixel(P.x, P.y, out);
				}
			}
		}
	}
}
