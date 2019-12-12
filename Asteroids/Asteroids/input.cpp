

struct Button_state
{
	bool is_down{false};
	bool changed{false};
};


enum Mouse_buttons
{
	M_LBUTTON,
	M_RBUTTON,

	MBUTTONS_COUNT
};

struct Mouse_input
{
	uint16_t x, y;
	Button_state buttons[MBUTTONS_COUNT];
};


enum Keyboard_buttons
{
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_LEFT,
	BUTTON_RIGHT,
	BUTTON_SHOOT,

	BUTTON_COUNT
};

struct Key_input
{
	Button_state buttons[BUTTON_COUNT];
};
