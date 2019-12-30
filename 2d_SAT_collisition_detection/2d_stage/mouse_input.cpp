enum Mouse_buttons
{
	LBUTTON,
	RBUTTON,

	MOUSE_BUTTONS_COUNT
};


struct Buttons
{
	bool is_dawn{false};
	bool change{false};
	bool pressed{false};
};


struct Mouse_Input
{
	vec2f pos;
	Buttons buttons[MOUSE_BUTTONS_COUNT];
};