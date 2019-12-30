
void triangle(vec3f* pts, IShader* shader);

inline void drawPixel(int x, int y, IShader* shader)
{
	if (x > surface.width - 1 || x < 0 || y > surface.height - 1 || y < 0)
		return;

	surface.memory[y * surface.width + x] = shader->fragment().whole;
}

inline void drawPixel(int x, int y, Color color)
{
	if (x > surface.width - 1 || x < 0 || y > surface.height - 1|| y < 0)
		return;
	
	surface.memory[y * surface.width + x] = color.whole;
}


inline void drawLine(int x, int y, int x2, int y2, IShader* shader) {
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
				drawPixel(j >> 16, y, shader);
				j += decInc;
			}
			return;
		}
		longLen += y;
		for (int j = 0x8000 + (x << 16); y >= longLen; --y) {
			drawPixel(j >> 16, y, shader);
			j -= decInc;
		}
		return;
	}

	if (longLen > 0) {
		longLen += x;
		for (int j = 0x8000 + (y << 16); x <= longLen; ++x) {
			drawPixel(x, j >> 16, shader);
			j += decInc;
		}
		return;
	}
	longLen += x;
	for (int j = 0x8000 + (y << 16); x >= longLen; --x) {
		drawPixel(x, j >> 16, shader);
		j -= decInc;
	}
}

inline void draw_rect(int x0, int y0, int x1, int y1, Color color)
{
	for (int y = y0; y < y1; y++)
		drawPixel(x0, y, color);

	for (int y = y0; y < y1; y++)
		drawPixel(x1, y, color);

	for (int x = x0; x < x1; x++)
		drawPixel(x, y0, color);

	for (int x = x0; x < x1; x++)
		drawPixel(x, y1, color);
}


inline void draw_filled_rect(int x0, int y0, int x1, int y1, Color color)
{
	for (int y = y0; y < y1; y++)
		for (int x = x0; x < x1; x++)
			drawPixel(x, y, color);
}

inline void draw_filled_circle(vec2f center, float radius, Color color)
{
	int x = center.x * surface.height + surface.width / 2;
	int y = center.y * surface.height + surface.height / 2;
	radius *= surface.width / 2;

	for (int i = -radius; i < radius; i++)
		for (int j = -radius; j < radius; j++)
			if (i*i + j*j < radius * radius)
				drawPixel(x + i, y + j, color);

}


inline void draw_shape(std::vector<vec2f>& pts, object& obj, IShader* shader)
{
	for (int i = 0; i < pts.size() - 1; i++)
	{
		vec2f local_pts[2] = { pts[i], pts[i+1]};
		vec3f screen_pts[2];
		shader->vertex(local_pts, screen_pts, obj);
		drawLine(screen_pts[0].x, screen_pts[0].y, screen_pts[1].x, screen_pts[1].y, shader);
	}

	vec2f local_pts[2] = { pts.back(), pts.front() };
	vec3f screen_pts[2];
	shader->vertex(local_pts, screen_pts, obj);
	drawLine(screen_pts[0].x, screen_pts[0].y, screen_pts[1].x, screen_pts[1].y, shader);
}


inline void draw_filled_shape(std::vector<vec2f>& pts, object& obj, IShader* shader)
{
	// triangle 
	if (pts.size() == 3)
	{
		vec2f local_pts[3] = { pts[0], pts[1], pts[2] };
		vec3f screen_pts[3];
		shader->vertex(local_pts, screen_pts, obj);
		triangle(screen_pts, shader);
	}
	// quad shape (2 triangles)
	else if (pts.size() == 4)
	{
		vec2f local_pts[3] = { pts[0], pts[1], pts[2] };
		vec3f screen_pts[3];
		shader->vertex(local_pts, screen_pts, obj);
		triangle(screen_pts, shader);

		local_pts[0] = pts[0];
		local_pts[1] = pts[2];
		local_pts[2] = pts[3];
		shader->vertex(local_pts, screen_pts, obj);
		triangle(screen_pts, shader);
	}
	else
	{
		for (int i = 0; i < pts.size() - 1; i++)
		{
			vec3f screen_pts[3];
			vec2f local_pts[3] = { obj.pos, pts[i], pts[i + 1] };
			shader->vertex(local_pts, screen_pts, obj);
			triangle(screen_pts, shader);
		}
		vec3f screen_pts[3];
		vec2f local_pts[3] = { obj.pos, pts.back(), pts.front() };
		shader->vertex(local_pts, screen_pts, obj);
		triangle(screen_pts, shader);
	}
}


void triangle(vec3f* pts, IShader* shader)
{
	if (pts[0].y == pts[1].y && pts[0].y == pts[2].y) return; // i don't care about degenerate triangles
	
	vec2f bot_left((std::numeric_limits<float>::max)(), (std::numeric_limits<float>::max)());
	vec2f top_right((std::numeric_limits<float>::min)(), (std::numeric_limits<float>::min)());
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			bot_left[j] = MAX(0.0f, MIN(bot_left[j], pts[i][j]));
			if (j == 1)
				top_right[j] = MIN(surface.height, MAX(top_right[j], pts[i][j]));
			else
				top_right[j] = MIN(surface.width, MAX(top_right[j], pts[i][j]));
		}
	}

	vec3i P;
	for (P.x = bot_left.x; P.x < top_right.x; P.x += 1.0f)
	{
		for (P.y = bot_left.y; P.y < top_right.y; P.y += 1.0f)
		{
			vec3f bar;
			if (barycentric(pts[0], pts[1], pts[2], P, &bar))
				surface.memory[P.y * surface.width + P.x] = shader->fragment().whole;	
		}
	}
}