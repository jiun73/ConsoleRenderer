#pragma once
#include <stdlib.h>
#define _WINSOCKAPI_
#include <Windows.h>
#include <iostream>
#include <map>
#include "Vector2D.h"
#include "Rect.h"
#include "Threads.h"

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

class ConsoleApp
{
private:
	HANDLE console;
	HANDLE input;
	INPUT_RECORD InputRecord[128];
	DWORD Events;
	V2d_i mouse;

	ThreadPool thread;

	std::map<V2d_i, ConsolePixel> screenBuffer;

	ConsolePixel pencil;
	V2d_i cursor = { 0,0 };
	V2d_i oldSize = 0;

	void setConsoleToCursor();
	void clearConsole() { system("cls"); }

	void setConsoleSettings(const ConsolePixel& pixel);
	void setConsoleColor(const fgConsoleColors& fg, const bgConsoleColors& bg) { SetConsoleTextAttribute(console, fg | bg); }
	void setDrawPencil(const ConsolePixel& pixel) { pencil = pixel; }
	void setCursor(const V2d_i& pos);
	void hideCursor();

	std::array<bool, 255> last;
	std::array<bool, 255> keys;

	bool keyState(int code)
	{
		return (GetKeyState(code) & 0x8000);
	}

	void getKeysStates()
	{
		last = keys;

		for (int i = 1; i < keys.size(); i++)
			keys.at(i) = keyState(i);
	}

public:
	ConsoleApp()
	{
		console = GetStdHandle(STD_OUTPUT_HANDLE);
		input = GetStdHandle(STD_INPUT_HANDLE);
		hideCursor();

		SetConsoleMode(input, ENABLE_EXTENDED_FLAGS);
		std::cout << SetConsoleMode(input, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);

		thread.start(1);
		thread.queueJob([&](int)
			{
				while(true)
					update();
			});
	}
	~ConsoleApp() {}

	void setDrawPencil	(const fgConsoleColors& fgcolor, const bgConsoleColors& bgcolor, const char& glyph) { pencil = { bgcolor, fgcolor, glyph}; }
	void setDrawColor	(const fgConsoleColors& fg, const bgConsoleColors& bg) { setConsoleColor(fg, bg); pencil.fg = fg; pencil.bg = bg; }
	void setDrawGlyph	(const char& _glyph) { pencil.glyph = _glyph; }

	void drawLineVertical	(const V2d_i& pos, const int& length);
	void drawLineHorizontal	(const V2d_i& pos, const int& length);
	void drawRect			(const Rect& dest);
	void drawPixel			(const V2d_i& pos) { if (!isWithinScreen(pos)) return; screenBuffer[pos] = pencil; }
	void drawText			(const std::string& text, V2d_i pos);

	void drawRect	(const int& x, const int& y, const int& w, const int& h) { return drawRect({ { x,y },{ w,h } }); }
	void drawPixel	(const int& x, const int& y) { return drawPixel({ x,y }); }
	void drawText	(const std::string& text, const int& x, const int& y) { drawText(text, { x,y }); }

	void clear() { clearConsole(); screenBuffer.clear(); }

	bool isWithinScreen(const V2d_i& pos);
	V2d_i getConsoleSize();

	void present();

	bool held_code(int code)
	{
		return keys.at(code);
	}

	bool held(char character)
	{
		return keyState((int)character);
	}

	bool pressed_code(int code)
	{
		return (!last.at(code) && keys.at(code));
	}

	bool pressed(char character)
	{
		return (!last.at((int)character) && keys.at((int)character));
	}

	V2d_i getMousePos()
	{
		return mouse;
	}

	bool didMouseClickOn(V2d_i pos)
	{
		return (pressed_code(VK_LBUTTON) && mouse == pos);
	}

	void update()
	{
		getKeysStates();

		ReadConsoleInput(input, InputRecord, 128, &Events);

		for (size_t i = 0; i < Events; i++)
		{


			switch (InputRecord[i].EventType)
			{
			case KEY_EVENT:
			{
				//printf("Key event: ");

				//if (InputRecord[i].Event.KeyEvent.bKeyDown)
				////	printf("key pressed\n");
				//else printf("key released\n");
			}
			break;
			case MOUSE_EVENT:
			{
				std::cout << "mouse event";

				mouse.x = InputRecord[i].Event.MouseEvent.dwMousePosition.X;
				mouse.y = InputRecord[i].Event.MouseEvent.dwMousePosition.Y;
			}

			break;
			default:
				break;
			}
		}
		FlushConsoleInputBuffer(input);
	}

};