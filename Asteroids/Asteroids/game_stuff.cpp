

struct Ship : public object
{

	std::vector<vec2f> local{vec2f(0.03, 0), vec2f(-0.02, 0.02), vec2f(-0.02, -0.02)};
	std::vector<vec2f> global{ vec2f(0.03, 0), vec2f(-0.02, 0.02), vec2f(-0.02, -0.02) };
	std::vector<vec3i> faces{ vec3i(0, 1 , 2) };

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

		for (int i = 0; i < local.size(); i++)
			global[i] = pos + (rotate * scale * local[i]);
	}

};




std::vector<vec2f> asteroids_shape[] = {
std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
std::vector<vec2f> { vec2f(0.02, 0.04), vec2f(0, 0.02), vec2f(-0.05, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.01), vec2f(0.05, -0.02), vec2f(0.05, 0) },
std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.01, 0.04), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
std::vector<vec2f> { vec2f(0.03, 0.04), vec2f(0, 0.05), vec2f(-0.02, 0.05), vec2f(0, 0.01), vec2f(-0.03, -0.01), vec2f(-0.005, -0.03), vec2f(0.04, -0.02), vec2f(0.05, 0) },
std::vector<vec2f> { vec2f(0.01, 0.04), vec2f(0, 0.02), vec2f(-0.01, 0.04),vec2f(-0.04, 0.05), vec2f(-0.03, -0.01), vec2f(-0.1, 0.02) , vec2f(-0.05, -0.02), vec2f(0.01, -0.07), vec2f(0.05, -0.02) , vec2f(0.1, 0), vec2f(0.02, 0.01) }
};

struct Asteroid : public object
{
	std::vector<vec2f> local;
	std::vector<vec2f> global;
	std::vector<vec3i> faces; // vec3i means 3 verts of triangle

	int shape;
	float speed_x{0};
	float speed_y{0};

	//Asteroid() = default;
	Asteroid(std::vector<vec2f> pts): local(pts), global(pts) {}
};

struct Asteroid_buffer
{
	size_t size;
	size_t actives{0};
	std::vector<Asteroid> buffer;

	Asteroid_buffer(size_t size) : size(size)
	{
		buffer.reserve(size);
		for (int i = 0; i < size; i++)
		{
			buffer.push_back(Asteroid(asteroids_shape[rand() % 5]));
			buffer.back().faces = triangulate(buffer.back().local);
		}
	}

	void calculate()
	{
		for (Asteroid& astr : buffer)
		{
			astr.pos.x += astr.speed_x * elapsed;
			astr.pos.y += astr.speed_y * elapsed;

			for (int i = 0; i < astr.local.size(); i++)
				astr.global[i] = astr.pos + (astr.rotate * astr.scale * astr.local[i]);
		}
	}

	void delete_asteroid(int i)
	{
		std::swap(buffer[i], buffer[--actives]);
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

struct Bullet
{
	vec2f pos;
	vec2f dir;
	float speed = 1;
};

template <int size>
struct bullet_buffer
{
	int max_size = size;
	Bullet buffer[size];
	int  actives = 0;
	
	bullet_buffer() = default;

	void add_bullet(vec2f pos, float dir)
	{
		if (actives == max_size) return;
		buffer[actives++] = Bullet{ pos, vec2f(cosf(dir), sinf(dir)) };
	}

	void calculate()
	{
		for (int i = 0; i < actives; i++)
		{
			buffer[i].pos += buffer[i].dir * buffer[i].speed * elapsed;

			if (fabs(buffer[i].pos.x) > 0.7 || fabs(buffer[i].pos.y) > 0.7)
				std::swap(buffer[i], buffer[--actives]);
		}
	}

	void delete_bullet(int i)
	{
		std::swap(buffer[i], buffer[--actives]);
	}

	Bullet& operator [](int i)
	{
		return buffer[i];
	}
};