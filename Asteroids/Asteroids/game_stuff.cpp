

struct Ship : public object
{
	std::vector<vec2f> pts{vec2f(0.03, 0), vec2f(-0.02, 0.02), vec2f(-0.02, -0.02)};
	float angle{PI / 2};
	float speed_x{0};
	float speed_y{0};
	float speed_dif{0.5};

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

struct Asteroid : public object
{
	std::vector<vec2f> pts;
	float speed_x{0};
	float speed_y{0};
};


std::vector<vec2f> get_asteroid(int i)
{
	std::vector<vec2f> a[] = {
	std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.02, 0.04), vec2f(0, 0.02), vec2f(-0.05, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.01), vec2f(0.05, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.02, 0.05), vec2f(0, 0.01), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.01, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04),vec2f(-0.04, 0.05), vec2f(-0.03, -0.01), vec2f(-0.1, 0.02) , vec2f(-0.05, -0.02), vec2f(0.01, -0.07), vec2f(0.05, -0.02) , vec2f(0.1, 0), vec2f(0.02, 0.01) }
	};

	return a[i];
}

