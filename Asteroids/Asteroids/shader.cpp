

struct IShader
{
	virtual ~IShader() {};
	virtual void vertex(vec2f* pts, vec3f* screen_pts, object& obj) = 0;
	virtual Color fragment() = 0;
};

// globals (uniform)
// life time of current partical
Color uni_particles_color = Color(255, 255, 255);
float uni_life_time = 1;

// particles shader
struct flame_shader : public IShader
{
	void vertex(vec2f* pts, vec3f* screen_pts, object& obj) override
	{
		for (int i = 0; i < 3; i++)
		{
			screen_pts[i] = vec3f(obj.pos + (obj.scale * obj.rotate * pts[i]));
			screen_pts[i].x = screen_pts[i].x * surface.height + surface.width / 2;
			screen_pts[i].y = screen_pts[i].y * surface.height + surface.height / 2;
		}
	}

	Color fragment() override
	{
		return uni_particles_color * uni_life_time;
	}
};


// stars
float uni_star_life;

bool uni_switch = false;
float strength = 0;

struct Star_shader : public IShader
{
	void vertex(vec2f* pts, vec3f* screen_pts, object& obj) override
	{
		for (int i = 0; i < 3; i++)
		{
			screen_pts[i] = vec3f(obj.pos + (obj.scale * obj.rotate * pts[i]));
			screen_pts[i].x = screen_pts[i].x * surface.height + surface.width / 2;
			screen_pts[i].y = screen_pts[i].y * surface.height + surface.height / 2;
		}
	}

	Color fragment() override
	{
		strength += (fmod(uni_star_life, 10) / 5000) * elapsed;

		if (strength > 0.9)
		{
			uni_switch = !uni_switch;
			strength = 0;
		}

		if (uni_switch)
			return Color(255, 255, 255) * strength;
		else
			return Color(255, 255, 255) * (1 - strength);
	}
};


// ship shader
Color uni_shape_color = Color(255, 255, 255);

struct line_shader : public IShader
{
	void vertex(vec2f* pts, vec3f* screen_pts, object& obj) override
	{
		for (int i = 0; i < 2; i++)
		{
			screen_pts[i] = vec3f(obj.pos + (obj.scale * obj.rotate * pts[i]));
			screen_pts[i].x = screen_pts[i].x * surface.height + surface.width / 2;
			screen_pts[i].y = screen_pts[i].y * surface.height + surface.height / 2;
		}

	}

	Color fragment() override
	{
		return uni_shape_color;
	}
};