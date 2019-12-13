

struct IShader
{
	virtual ~IShader() {};
	virtual void vertex(Vec3f* pts) = 0;
	virtual Color fragment() = 0;
};

// globals (uniform)
// life time of current partical
float uni_life_time = 0.1;
Color uni_color(255, 0, 0);


struct goOutShader : public IShader
{
	virtual void vertex(Vec3f* pts) override
	{
		for (int i = 0; i < 3; i++)
		{
			pts[i].x = pts[i].x * surface.height + surface.width / 2;
			pts[i].y = pts[i].y * surface.height + surface.height / 2;
		}
	}

	virtual Color fragment() override
	{
		return uni_color * uni_life_time;
	}
};