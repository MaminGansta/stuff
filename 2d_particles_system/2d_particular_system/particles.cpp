

// shapes initialization
std::vector<std::vector<vec2f>> shapes
{ 
	{vec2f(0, 0.1), vec2f(-0.1, -0.1), vec2f(0.1, -0.1)},
	{vec2f(0.1, 0.1), vec2f(0.1, -0.1), vec2f(-0.1, -0.1), vec2f(-0.1, 0.1)}
};


struct object
{
	vec2f pos;
	Matrix22f rotate;
	Matrix22f scale;

	object(vec2f pos = vec2f()) : pos(pos), rotate(identity), scale(identity) {}
};



struct particle : public object
{
	bool active{ false };
	int shape;

	vec2f speed_dir;
	float whole_speed; // start speed
	float speed;      // current speed

	float whole_life;
	float life_time; // in secconds
	float rotation_angle{0};
	float scale_indx{ 1 };

	particle(int shape, vec2f pos = vec2f()) : object(pos), shape(shape) {}
	particle() = default;
};



// global
float elapsed = 0;

// for second mod
vec2f mouse_pos;


void stnd_behavior(std::vector<particle>& buffer, size_t& actives)
{
	for (int i = 0; i < actives; i++)
	{
		
		if (buffer[i].life_time < elapsed)
		{
			buffer[i].active = false;
			std::swap(buffer[i--], buffer[--actives]);
			continue;
		}

		buffer[i].life_time -= elapsed;
		buffer[i].speed = buffer[i].whole_speed * (buffer[i].life_time / buffer[i].whole_life);
		buffer[i].pos +=  buffer[i].speed_dir * buffer[i].speed * elapsed;
		buffer[i].rotate = Matrix22f(cosf(buffer[i].rotation_angle), -sinf(buffer[i].rotation_angle),
									 sinf(buffer[i].rotation_angle),  cosf(buffer[i].rotation_angle));
		buffer[i].scale[0][0] = buffer[i].scale_indx;
		buffer[i].scale[1][1] = buffer[i].scale_indx;
	}
}

struct particles_buffer
{
	std::vector<particle> buffer;
	size_t actives{0};
	size_t size;
	void(*behavior)(std::vector<particle>& buffer, size_t& actives);

	particles_buffer(size_t max_size, void(*behavior)(std::vector<particle>& buffer, size_t& actives) = stnd_behavior) : size(max_size), behavior(behavior)
	{
		buffer.reserve(size);
		for (int i = 0; i < size; i++)
			buffer.push_back(particle(rand() % shapes.size()));
	}

	void calculate() { behavior(buffer, actives); }
	particle& operator [] (int i) { return buffer[i]; }
};

void add_particles(particles_buffer& buffer, int amount, vec2f pos, float scale, vec2f dir = vec2f(1, 0), float range = PI / 6, float speed = 0, float life_time = 999)
{
	int max = MIN(buffer.size - 1, buffer.actives + amount);

	for (int i = buffer.actives; i < max; i++)
	{
		buffer.buffer[i].active = true;
		buffer.buffer[i].pos = pos;
		buffer.buffer[i].scale_indx = scale;
		std::random_device rd;  //Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(0, range);
		float diviation = dis(gen) - range/2;
		buffer.buffer[i].speed_dir = Matrix22f(cosf(diviation), -sinf(diviation), sinf(diviation), cosf(diviation)) * dir.normalize();
		buffer.buffer[i].speed = speed;
		buffer.buffer[i].whole_speed = speed;
		buffer.buffer[i].life_time = life_time;
		buffer.buffer[i].whole_life = life_time;
	}

	buffer.actives += max - buffer.actives;
}

