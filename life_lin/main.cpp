#include <ncurses.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <chrono>

class App
{
public:
	App()
	{
	}
    
    ~App()
	{
		endwin();
	}
    
	void init()
	{
		srand(time(nullptr));
		initscr();
		cbreak(); // or raw
		curs_set(0);
		start_color();
		init_pair(1,COLOR_RED,COLOR_BLUE);
		b1 = std::vector<char>(COLS * LINES, ' ');
		
		for (auto& b : b1)
		{
			if (rand()%3==0) b = 'O';
		}
		
	}
	
	void run()
	{
		last = std::chrono::high_resolution_clock::now();
		struct timespec ts {0, 16666666};
		
		while (true)
		{
			b2 = b1;
			for (int r = 0; r < LINES; ++r)
			{
				for (int c = 0; c < COLS; ++c)
				{
					int alive = 0;
					for (int nr = r-1; nr < (r+2); ++nr)
					{
						if (nr >= 0 && nr < LINES)
						{
							for (int nc = c-1; nc < (c+2); ++nc)
							{
								if (nc >= 0 && nc < COLS && !(nr == r && nc == c) && b1[nr * COLS + nc] == 'O') ++alive; 
							}
						}
					}
					if (b1[r * COLS + c] == 'O')
					{
						if (alive < 2 || alive > 3) b2[r * COLS + c] = ' ';
					}
					else
					{
						if (alive == 3) b2[r * COLS + c] = 'O';
					}
				}
			}
			b1.swap(b2);
			//clear();
			for (int r = 0; r < LINES; ++r)
			{
				// mvaddchnstr(r, 0, (const unsigned int
				//				   *)&b2[r * COLS], COLS);
				for (int c = 0; c < COLS; ++c)
				{
					mvaddch(r, c, b2[r * COLS + c]);
				}
			}
			auto curr = std::chrono::high_resolution_clock::now();
			auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(curr - last).count();
			last = curr;
			mvprintw( 0, 0, "delta: %ld", diff );
			refresh();
			nanosleep(&ts, nullptr);
			//getch();
		}
	}
	
private:
	std::vector<char> b1;
	std::vector<char> b2;
	std::chrono::time_point<std::chrono::high_resolution_clock> last;

};

int main()
{
	App app;
	app.init();
	app.run();
	
	return 0;
}
