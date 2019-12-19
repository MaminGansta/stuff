

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
	int shape;
	float speed_x{0};
	float speed_y{0};
};

struct Asteroid_buffer
{
	size_t size;
	size_t actives{0};
	std::vector<Asteroid> buffer;

	Asteroid_buffer(size_t size) : size(size), buffer(size) {}

	void calculate()
	{
		for (Asteroid& astr : buffer)
		{
			astr.pos.x += astr.speed_x * elapsed;
			astr.pos.y += astr.speed_y * elapsed;
		}
	}

	Asteroid& operator [] (int i)
	{
		return buffer[i];
	}
};

void add_asteroid(Asteroid_buffer& buffer, float scale, float diviation)
{
	// pos on the circle
	// diviation of the center

	if (buffer.actives == buffer.size)
		return;

	buffer.buffer[buffer.actives].shape = rand() % 5;
	buffer.buffer[buffer.actives].scale = scale;
	std::uniform_real_distribution<> r(0.5, 0.1);
	int sgn1 = sgn(r(gen));
	int sgn2 = sgn(r(gen));
	float x = r(gen) * sgn1;
	float y = r(gen) * sgn2;
	buffer.buffer[buffer.actives].pos = vec2f(x, y);

	buffer.buffer[buffer.actives].speed_x = -x / 2;
	buffer.buffer[buffer.actives].speed_y = -y / 2;
	buffer.actives++;
} 


	std::vector<vec2f> asteroids_shape[] = {
	std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.02, 0.04), vec2f(0, 0.02), vec2f(-0.05, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.01), vec2f(0.05, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.02, 0.05), vec2f(0, 0.01), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.01, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04),vec2f(-0.04, 0.05), vec2f(-0.03, -0.01), vec2f(-0.1, 0.02) , vec2f(-0.05, -0.02), vec2f(0.01, -0.07), vec2f(0.05, -0.02) , vec2f(0.1, 0), vec2f(0.02, 0.01) }
	};