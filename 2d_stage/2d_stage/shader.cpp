

struct IShader
{
	virtual ~IShader() {};
	virtual void vertex(vec2f* pts, vec3f* screen_pts, object& obj) = 0;
	virtual Color fragment() = 0;
};

// globals (uniform)
// life time of current partical
float uni_life_time = 1;
Color uni_color(255, 0, 0);

// mod == 1
Color uni_color2(255, 255, 255);
float uni_dist = 1;


struct goOutShader : public IShader
{
	virtual void vertex(vec2f* pts, vec3f* screen_pts, object& obj) override
	{
		for (int i = 0; i < 3; i++)
		{
			screen_pts[i] = vec3f(obj.pos + (obj.scale * obj.rotate * pts[i]));
			screen_pts[i].x = screen_pts[i].x * surface.height + surface.width / 2;
			screen_pts[i].y = screen_pts[i].y * surface.height + surface.height / 2;
		}
	}

	virtual Color fragment() override
	{
#if mod == 0
		return uni_color * uni_life_time;
#elif mod == 1
		return uni_color * uni_dist;
#elif mod == 2
		return Color(uni_color2.r, uni_color2.g * uni_dist, uni_color2.b * (1 - uni_dist));
#endif
	}
};