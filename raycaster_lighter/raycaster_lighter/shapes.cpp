
struct Line
{
	Vert2f pos;
	Vec2f dir;
	int lenght{0};

	inline Line(Vert2f pos, Vec2f dir, int lenght) : pos(pos), dir(dir), lenght(lenght) {}

	void draw(Color color = Color(255, 255, 255))
	{
		
		Vec2f second_point = pos + dir * lenght;
		drawLine(pos.x, pos.y, second_point.x, second_point.y, color);
	}
};


// shapes
// bound all dots with lines
inline void make_shape(Vec2f* p, int size, std::vector<Line>& lines)
{
	for (int i = 0; i < size - 1; i++)
	{
		Vec2f dir(p[i + 1].x - p[i].x, p[i + 1].y - p[i].y);
		lines.push_back(Line(p[i], dir.normalize(), dir.norm() > 0? dir.norm() + 1: dir.norm() - 1));
	}

	// bound end wiht start dot
	Vec2f dir(p[0].x - p[size - 1].x, p[0].y - p[size - 1].y);
	lines.push_back(Line(p[size - 1], dir.normalize(), dir.norm() > 0 ? dir.norm() + 1 : dir.norm() - 1));
}


// add some shapes
inline void add_some_shapes(std::vector<Line>& lines)
{
	// window borders
	{
		Vec2f vert[] = { Vec2f(0, 0), Vec2f(surface.width - 1, 0), Vec2f(surface.width - 1, surface.height - 1), Vec2f(0, surface.height - 1) };
		make_shape(vert, 4, lines);
	}

	// shapes
	{
		Vec2f vert[] = { Vec2f(150, 100), Vec2f(250, 200), Vec2f(200, 400) };
		make_shape(vert, 3, lines);
	}

	{
		Vec2f vert[] = { Vec2f(400, 200), Vec2f(450, 200), Vec2f(350, 300) };
		make_shape(vert, 3, lines);
	}

	{
		Vec2f vert[] = { Vec2f(1000, 200), Vec2f(1150, 100), Vec2f(1050, 400) };
		make_shape(vert, 3, lines);
	}

	{
		Vec2f vert[] = { Vec2f(980, 600), Vec2f(1020, 640), Vec2f(1010, 610) };
		make_shape(vert, 3, lines);
	}

	{
		Vec2f vert[] = { Vec2f(350, 50), Vec2f(550, 50), Vec2f(630, 150), Vec2f(430, 150) };
		make_shape(vert, 4, lines);
	}


	{
		Vec2f vert[] = { Vec2f(980, 500), Vec2f(1060, 550), Vec2f(1100, 430) };
		make_shape(vert, 3, lines);
	}

	{
		Vec2f vert[] = { Vec2f(200, 500), Vec2f(350, 400), Vec2f(400, 530), Vec2f(200, 600) };
		make_shape(vert, 4, lines);
	}
}