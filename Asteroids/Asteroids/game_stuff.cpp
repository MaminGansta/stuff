

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

	std::uniform_int_distribution<> r_shape(0, 4);
	buffer.buffer[buffer.actives].shape = r_shape(gen);
	buffer.buffer[buffer.actives].scale = scale;
	std::uniform_real_distribution<> r(0, 2*PI);
	float angle = r(gen);
	float x = cosf(angle) * 0.8;
	float y = sinf(angle) * 0.8;
	buffer.buffer[buffer.actives].pos = vec2f(x, y);

	std::uniform_real_distribution<> div(0, diviation);
	std::uniform_real_distribution<> speed(0.1, 0.3);

	buffer.buffer[buffer.actives].speed_x = -cosf(angle + div(gen)) * speed(gen);
	buffer.buffer[buffer.actives].speed_y = -sinf(angle + div(gen)) * speed(gen);
	buffer.actives++;
} 


	std::vector<vec2f> asteroids_shape[] = {
	std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.02, 0.04), vec2f(0, 0.02), vec2f(-0.05, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.01), vec2f(0.05, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.02, 0.05), vec2f(0, 0.01), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
	std::vector<vec2f> { vec2f(0.01, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04),vec2f(-0.04, 0.05), vec2f(-0.03, -0.01), vec2f(-0.1, 0.02) , vec2f(-0.05, -0.02), vec2f(0.01, -0.07), vec2f(0.05, -0.02) , vec2f(0.1, 0), vec2f(0.02, 0.01) }
	};