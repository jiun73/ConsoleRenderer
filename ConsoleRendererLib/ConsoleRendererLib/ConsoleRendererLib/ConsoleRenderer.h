#pragma once
#include <stdlib.h>
#define _WINSOCKAPI_
#include <Windows.h>
#include <iostream>
#include <map>
#include "Vector2D.h"
#include "Rect.h"
#include "Threads.h"
#include "Random.h"

enum fgConsoleColors
{
	BLACK = 0,
	DARK_BLUE = FOREGROUND_BLUE,
	DARK_RED = FOREGROUND_RED,
	DARK_GREEN = FOREGROUND_GREEN,
	DARK_CYAN = DARK_BLUE | DARK_GREEN,
	DARK_PURPLE = DARK_BLUE | DARK_RED,
	DARK_YELLOW = DARK_GREEN | DARK_RED,
	GRAY = DARK_RED | DARK_GREEN | DARK_BLUE,
	DARK_GRAY = FOREGROUND_INTENSITY,
	BLUE = DARK_BLUE | FOREGROUND_INTENSITY,
	RED = DARK_RED | FOREGROUND_INTENSITY,
	GREEN = DARK_GREEN | FOREGROUND_INTENSITY,
	CYAN = DARK_CYAN | FOREGROUND_INTENSITY,
	PURPLE = DARK_PURPLE | FOREGROUND_INTENSITY,
	YELLOW = DARK_YELLOW | FOREGROUND_INTENSITY,
	WHITE = GRAY | FOREGROUND_INTENSITY,
};

enum bgConsoleColors
{
	BG_BLACK = 0,
	BG_DARK_BLUE = BACKGROUND_BLUE,
	BG_DARK_RED = BACKGROUND_RED,
	BG_DARK_GREEN = BACKGROUND_GREEN,
	BG_DARK_CYAN = BG_DARK_BLUE | BG_DARK_GREEN,
	BG_DARK_PURPLE = BG_DARK_BLUE | BG_DARK_RED,
	BG_DARK_YELLOW = BG_DARK_GREEN | BG_DARK_RED,
	BG_GRAY = BG_DARK_RED | BG_DARK_GREEN | BG_DARK_BLUE,
	BG_DARK_GRAY = BACKGROUND_INTENSITY,
	BG_BLUE = BG_DARK_BLUE | BACKGROUND_INTENSITY,
	BG_RED = BG_DARK_RED | BACKGROUND_INTENSITY,
	BG_GREEN = BG_DARK_GREEN | BACKGROUND_INTENSITY,
	BG_CYAN = BG_DARK_CYAN | BACKGROUND_INTENSITY,
	BG_PURPLE = BG_DARK_PURPLE | BACKGROUND_INTENSITY,
	BG_YELLOW = BG_DARK_YELLOW | BACKGROUND_INTENSITY,
	BG_WHITE = BG_GRAY | BACKGROUND_INTENSITY,
};

#include <array>

struct ConsolePixel
{
	bgConsoleColors bg = BG_BLACK;
	fgConsoleColors fg = BLACK;
	char glyph = (char)219;
};

class ConsoleApp;

class RunBool 
{
	ConsoleApp* app = nullptr;
	bool isRunning = false;

public:
	RunBool(ConsoleApp* app, bool isRunning) : app(app), isRunning(isRunning) {}
	~RunBool();

	operator bool() const { return isRunning; };
};

class ConsoleApp
{
private:
	HANDLE console;
	HANDLE input;
	INPUT_RECORD InputRecord[128];
	DWORD Events;
	

	ThreadPool thread;

	std::map<V2d_i, ConsolePixel> screenBuffer;

	ConsolePixel _pencil;
	V2d_i cursor = { 0,0 };
	V2d_i oldSize = 0;

	void setConsoleToCursor();
	void clearConsole() { system("cls"); }

	void setConsoleSettings(const ConsolePixel& pixel);
	void setConsoleColor(const fgConsoleColors& fg, const bgConsoleColors& bg) { SetConsoleTextAttribute(console, fg | bg); }
	void setDrawPencil(const ConsolePixel& pixel) { _pencil = pixel; }
	void setCursor(const V2d_i& pos);
	void hideCursor();

	std::array<bool, 255> last;
	std::array<bool, 255> keys;
	std::array<int, 255> cooldown;
	int base_cooldown = 10;

	bool isRunning = true;

	bool get_key_state(int code)
	{
		return (GetKeyState(code) & 0x8000);
	}

	void get_key_states()
	{
		for (int i = 1; i < keys.size(); i++)
			last.at(i) = keys.at(i);

		for (int i = 1; i < keys.size(); i++)
			keys.at(i) = get_key_state(i);
	}

	void keyboard_update() 
	{ 
		get_key_states();
		for(auto& c : cooldown)
		{
			if (c > 0)
				c--;
		}
	}
	void update();
	void present();

public:
	V2d_i _mouse;
	Random random;

	ConsoleApp();
	~ConsoleApp() {}

	void pencil	(const fgConsoleColors& fgcolor, const bgConsoleColors& bgcolor, const char& glyph);
	void color	(const fgConsoleColors& fg, const bgConsoleColors& bg) { setConsoleColor(fg, bg); _pencil.fg = fg; _pencil.bg = bg; }
	void glyph	(const char& _glyph) { _pencil.glyph = _glyph; }

	void vertical		(const V2d_i& pos, const int& length);
	void horizontal		(const V2d_i& pos, const int& length);
	void rect			(const Rect& dest);
	void pix			(const V2d_i& pos) { if (!pos_in_screen(pos)) return; screenBuffer[pos] = _pencil; }
	void text			(const std::string& text, V2d_i pos);

	void clear() { clearConsole(); screenBuffer.clear(); }

	bool pos_in_screen(const V2d_i& pos);
	V2d_i size();

	

	bool held_code		(int code);
	bool held			(char character);
	bool pressed_code	(int code);
	bool pressed		(char character);

	Rect getMaxWindow(HWND window)
	{
		RECT rectWorkArea;
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		::GetMonitorInfo(::MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST), &mi);
		rectWorkArea = mi.rcWork;
		return { {rectWorkArea.left, rectWorkArea.top},{rectWorkArea.bottom - rectWorkArea.top, rectWorkArea.right - rectWorkArea.left} };
	}

	V2d_i mouse() 
	{ 
		HWND console_wnd = GetConsoleWindow();
		POINT cursor_pos;
		GetCursorPos(&cursor_pos);

		if (console_wnd && ScreenToClient(console_wnd, &cursor_pos));
		{
			CONSOLE_FONT_INFOEX cfi;
			GetCurrentConsoleFontEx(console, FALSE, &cfi);
			COORD sz = GetConsoleFontSize(console, cfi.nFont);
			
			V2d_i max = getMaxWindow(console_wnd).sz;
			V2d_i min = size();

			int fontX = (max.x / (double)min.x) * sz.X;
			int fontY = (max.y / (double)min.y) * sz.Y;

			_mouse.x = ((int)cursor_pos.x / sz.X);
			_mouse.y = ((int)cursor_pos.y / sz.Y);
			return _mouse;
		}
		
		return 0; 
	}

	bool mouse_left_held() { return held_code(VK_LBUTTON); }
	bool mouse_left_press() { return pressed_code(VK_LBUTTON); }
	bool mouse_click_on(V2d_i pos) { return (pressed_code(VK_LBUTTON) && _mouse == pos); }

	const RunBool&	run()	{ return RunBool(this, isRunning); }
	void			close() { isRunning = false; }
	friend RunBool;

};