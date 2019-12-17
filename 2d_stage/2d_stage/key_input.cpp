enum Buttons
{
	P1_UP,
	P1_DOWN,
	P2_UP,
	P2_DOWN,

	BUTTONS_COUNT
};

struct Input
{
	bool is_down[BUTTONS_COUNT];
	bool change[BUTTONS_COUNT];
	bool is_pressed[BUTTONS_COUNT];

	Input()
	{
		for (int i = 0; i < BUTTONS_COUNT; i++)
		{
			is_down[i] = false;
			change[i] = false;
			is_pressed[i] = false;
		}
	}
};