#ifndef KEYMAP_H
#define KEYMAP_H

#define KEY_RELEASED 0x80
#define KEYMAP_SIZE (sizeof(keymap) / sizeof(keymap[0][0]))

#define ESC 0x1B

#define KEY_F1  0x80
#define KEY_F2  (KEY_F1 + 1)
#define KEY_F3  (KEY_F2 + 1)
#define KEY_F4  (KEY_F3 + 1)
#define KEY_F5  (KEY_F4 + 1)
#define KEY_F6  (KEY_F5 + 1)
#define KEY_F7  (KEY_F6 + 1)
#define KEY_F8  (KEY_F7 + 1)
#define KEY_F9  (KEY_F8 + 1)
#define KEY_F10 (KEY_F9 + 1)
#define KEY_F11 (KEY_F10 + 1)
#define KEY_F12 (KEY_F11 + 1)

#define LSHIFT 	0x2A
#define RSHIFT 	0x36
#define CTRL   	0x1D
#define ALT    	0x38
#define DEL    	0x53

#define KEY_INS   0x90
#define KEY_DEL   (KEY_INS + 1) 
#define KEY_HOME  (KEY_DEL + 1)
#define KEY_END   (KEY_HOME + 1)
#define KEY_PGUP  (KEY_END + 1)
#define KEY_PGDN  (KEY_PGUP + 1)
#define KEY_LEFT  (KEY_PGDN + 1)
#define KEY_UP    (KEY_LEFT + 1)
#define KEY_DOWN  (KEY_UP + 1)
#define KEY_RIGHT (KEY_DOWN + 1)

#define SYSRQ    0x70
#define PSCREEN  (SYSRQ + 1)
#define KEY_WIN  (PSCREEN + 1)
#define KEY_MENU (KEY_WIN + 1)

#define	KEY_NUM_LOCK		0x45
#define	KEY_SCROLL_LOCK		0x46
#define	KEY_CAPS_LOCK		0x3A


int keymap[][2] = {
	/* 0 */		{0, 0},      {ESC, 0},
	/* 2 */		{'1', '!'},  {'2', '@'},
	/* 4 */		{'3', '#'},  {'4', '$'},
	/* 6 */		{'5', '%'},  {'6', '^'},
	/* 8 */		{'7', '&'},  {'8', '*'},
	/* 10 */	{'9', '('},  {'0', ')'},
	/* 12 */	{'-', '_'},  {'=', '+'},
	/* 14 */	{'\b', '\b'},{'\t', 0},
	/* 16 */	{'q', 'Q'},  {'w', 'W'},
	/* 18 */	{'e', 'E'},  {'r', 'R'},
	/* 20 */	{'t', 'T'},  {'y', 'Y'},
	/* 22 */	{'u', 'U'},  {'i', 'I'},
	/* 24 */	{'o', 'O'},  {'p', 'P'},
	/* 26 */	{'´', '`'},  {'[', '{'},
	/* 28 */	{'\n', 0},   {'/', '?'},
	/* 30 */	{'a', 'A'},  {'s', 'S'},
	/* 32 */	{'d', 'D'},  {'f', 'F'},
	/* 34 */	{'g', 'G'},  {'h', 'H'},
	/* 36 */	{'j', 'J'},  {'k', 'K'},
	/* 38 */	{'l', 'L'},  {'ç', 'Ç'},
	/* 40 */	{'~', '^'},  {'\'', '"'},
	/* 42 */	{LSHIFT, 0}, {']', '}'},
	/* 44 */	{'z', 'Z'},  {'x', 'X'},
	/* 46 */	{'c', 'C'},  {'v', 'V'},
	/* 48 */	{'b', 'B'},  {'n', 'N'},
	/* 50 */	{'m', 'M'},  {',', '<'},
	/* 52 */	{'.', '>'},  {';', ':'},
	/* 54 */	{RSHIFT, 0}, {0, 0},
	/* 56 */	{ALT, 0},    {' ', ' '},
	/* 58 */	{0, 0},      {KEY_F1, KEY_F1},
	/* 60 */	{KEY_F2, KEY_F2},   {KEY_F3, KEY_F3},
	/* 62 */	{KEY_F4, KEY_F4},   {KEY_F5, KEY_F5},
	/* 64 */	{KEY_F6, KEY_F6},   {KEY_F7, KEY_F7},
	/* 66 */	{KEY_F8, KEY_F8},   {KEY_F9, KEY_F9},
	/* 68 */	{KEY_F10, KEY_F10}, {0, 0},
	/* 70 */	{0, 0},             {KEY_HOME, '7'},
	/* 72 */	{KEY_UP, '8'},      {KEY_PGUP, '9'},
	/* 74 */	{'-', 0},           {KEY_LEFT, '4'},
	/* 76 */	{'5', 0},           {KEY_RIGHT, '6'},
	/* 78 */	{'+', 0},           {KEY_END, '1'},
	/* 80 */	{KEY_DOWN, '2'},    {KEY_PGDN, '3'},
	/* 82 */	{KEY_INS, '0'},     {KEY_DEL, KEY_DEL},
	/* 84 */	{KEY_F11, KEY_F11}, {KEY_F12, KEY_F12},
	/* 86 */	{'\\', '|'},        {'\n', '\n'},
	/* 88 */	{CTRL, 0},          {'/', 0},
	/* 90 */	{SYSRQ, PSCREEN},   {ALT, 0},
	/* 92 */	{KEY_HOME, 0},      {KEY_UP, 0},
	/* 94 */	{KEY_PGUP, 0},      {KEY_LEFT, 0},
	/* 96 */	{KEY_RIGHT, 0},     {KEY_END, 0},
	/* 98 */	{KEY_DOWN, 0},      {KEY_PGDN, 0},
	/* 100 */	{KEY_INS, 0},       {KEY_DEL, 0},
	/* 102 */	{KEY_WIN, 0},       {KEY_WIN, 0},
	/* 104 */	{KEY_MENU, 0}
};

#endif
