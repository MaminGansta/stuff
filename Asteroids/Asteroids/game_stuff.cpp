

struct Rect
{
	int x0, y0, x1, y1;
};


struct Ship
{
	std::vector<Vec2f> pts{Vec2f(0, 0.02), Vec2f(-0.01, -0.01), Vec2f(0.01, -0.01)};
	float speed_x{0};
	float speed_y{0};
};

struct Asteroid
{
	std::vector<Vec2f> pts;
	float speed_x{0};
	float speed_y{0};
};