// life.cpp : Defines the entry point for the console application.
#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <chrono>
#include <Windows.h>

void iferr(BOOL val)
{
	if (!val)
	{
		throw std::runtime_error("Error: " + std::to_string(GetLastError()));
	}
}

class Window
{
public:
	Window() : hStdOut(nullptr), hStdIn(nullptr)
	{
	}

	~Window()
	{
		if (hStdIn) SetConsoleMode(hStdIn, oldMode);
		if (hStdOut) SetConsoleCursorInfo(hStdOut, &oldCursor);
	}

	void init()
	{
		hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
		hStdIn = GetStdHandle(STD_INPUT_HANDLE);

		iferr(GetConsoleMode(hStdIn, &oldMode));
		iferr(GetConsoleCursorInfo(hStdOut, &oldCursor));

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		iferr(GetConsoleScreenBufferInfo(hStdOut, &csbi));
		width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		heigth = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		coord.X = csbi.srWindow.Left;
		coord.Y = csbi.srWindow.Top;

		iferr(SetConsoleMode(hStdIn, ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT));
		CONSOLE_CURSOR_INFO newCursor;
		newCursor.bVisible = false;
		newCursor.dwSize = 10;
		iferr(SetConsoleCursorInfo(hStdOut, &newCursor));
		b1 = std::vector<TCHAR>(width*heigth, ' ');

		for (int r = 0; r < heigth; ++r)
		{
			for (int c = 0; c < width; ++c)
			{
				if (rand() % 3 == 0) b1[r * width + c] = 'O';
			}
		}
		b2 = b1;
	}

	void run()
	{
		running = true;
		DWORD numRead;
		INPUT_RECORD buff[128];
		last = std::chrono::high_resolution_clock::now();
		while(running)
		{
			iferr(GetNumberOfConsoleInputEvents(hStdIn, &numRead));
			if (numRead > 0) {
				iferr(ReadConsoleInput(hStdIn, &buff[0], sizeof(buff) / sizeof(INPUT_RECORD), &numRead));

				for (unsigned long i = 0; i < numRead; ++i)
				{
					if (!running) break;
					handleEvent(buff[i]);
				}
			}
			b2 = b1;
			for (int r = 0; r < heigth; ++r)
			{
				for (int c = 0; c < width; ++c)
				{
					int alive = 0;
					for (int dr = r - 1; dr < r + 2; ++dr)
					{
						for (int dc = c - 1; dc < c + 2; ++dc)
						{
							if (!(dr == r && dc == c) && dr >= 0 && dr < heigth && dc >= 0 && dc < width)
							{
								if (b1[dr * width + dc] == 'O') ++alive;
							}
						}
					}
					if (b1[r * width + c] == 'O' && (alive < 2 || alive > 3)) b2[r * width + c] = ' ';
					if (b1[r * width + c] != 'O' && alive == 3) b2[r * width + c] = 'O';
				}
			}
			b2.swap(b1);
			DWORD ret;
			WriteConsoleOutputCharacter(hStdOut, &b1[0], heigth * width, coord, &ret);
			auto curr = std::chrono::high_resolution_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(curr - last).count();
			last = curr;
			std::basic_string<TCHAR> str = "delta: " + std::to_string(diff);
			WriteConsoleOutputCharacter(hStdOut, &str[0], str.size(), coord, &ret);

			Sleep(1000.0/60);
		}
	}

	void handleEvent(INPUT_RECORD& rec)
	{
		switch (rec.EventType)
		{
		case KEY_EVENT:
			if (rec.Event.KeyEvent.bKeyDown && rec.Event.KeyEvent.wVirtualKeyCode == VK_ESCAPE)
			{
				running = false;
				return;
			}
		default:
			break;
		}
	}

private:
	HANDLE hStdOut;
	HANDLE hStdIn;
	DWORD oldMode;
	CONSOLE_CURSOR_INFO oldCursor;
	short width, heigth;
	bool running;
	std::vector<TCHAR> b1, b2;
	COORD coord;
	std::chrono::time_point<std::chrono::high_resolution_clock> last;
};

int main()
{
	Window window;
	try
	{
		window.init();
		window.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	
    return 0;
}
