
// mouse

enum Mouse_buttons
{
	LBUTTON,
	RBUTTON,

	MOUSE_BUTTONS_COUNT
};


struct Buttons
{
	bool is_down{ false };
	bool change{ false };
	bool pressed{ false };
};


struct Mouse_Input
{
	vec2f pos;
	Buttons buttons[MOUSE_BUTTONS_COUNT];
};



// keyboard

enum Key_Buttons
{
	P1_UP,
	P1_DOWN,
	P2_UP,
	P2_DOWN,

	BUTTONS_COUNT
};

struct Key_Input
{
	Buttons buttons[MOUSE_BUTTONS_COUNT];

	Key_Input()
	{
		for (int i = 0; i < BUTTONS_COUNT; i++)
		{
			buttons[i].is_down = false;
			buttons[i].change= false;
		}
	}

	void discard()
	{
		for (int i = 0; i < BUTTONS_COUNT; i++)
		{
			buttons[i].is_down = false;
			buttons[i].change = false;
		}
	}
};


