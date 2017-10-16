#include <time.h>
#include <stdlib.h>
#include <iostream>
#include <SFML/Graphics.hpp>

class App
{
public:
	App()
	{
	}
    
    ~App()
	{
	}
    
	void init()
	{
		srand(time(nullptr));
		window.create(sf::VideoMode(width*scale, height*scale), "Life", sf::Style::Titlebar | sf::Style::Close);
		window.setFramerateLimit(30);
		view.reset(sf::FloatRect(0.0f, 0.0f, width, height));
		window.setView(view);
		img1.create(width, height);
		img2.create(width, height);

		for (int r = 0; r < height; ++r)
		{
			for (int c = 0; c < width; ++c)
			{
				if (rand()%3==0) img1.setPixel(c, r, sf::Color::White);
			}
		}
		img2.copy(img1, 0, 0);
		tex.loadFromImage(img1);
		sprite.setTexture(tex);
		sprite.setPosition(0.0f, 0.0f);
	}
	
	void run()
	{
		sf::Clock clock;
		float lastTime = clock.restart().asSeconds();
		unsigned int frame = 0;

		while(window.isOpen()) {
			sf::Event e;
			while(window.pollEvent(e)) {
				switch(e.type) {
					case sf::Event::Closed:
						window.close();
						break;
					case sf::Event::KeyReleased:
						if (e.key.code == sf::Keyboard::Escape) window.close();
						if (e.key.code == sf::Keyboard::Space) paused = !paused;
						if (e.key.code == sf::Keyboard::C) img1.create(width, height);
						if (e.key.code == sf::Keyboard::X) fillPattern1(img1);
						if (e.key.code == sf::Keyboard::S) fillSpaceShip(img1, window.mapPixelToCoords(sf::Mouse::getPosition(window)));
						break;
					default:
						break;
				}
			}

			if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
			{
				sf::Vector2f p = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				img1.setPixel(p.x, p.y, sf::Color::White);
			}
			else if (sf::Mouse::isButtonPressed(sf::Mouse::Right))
			{
				sf::Vector2f p = window.mapPixelToCoords(sf::Mouse::getPosition(window));
				img1.setPixel(p.x, p.y, sf::Color::Black);
			}

			window.clear();
			
			img2.copy(img1, 0, 0);
			const sf::Uint8* src = img1.getPixelsPtr();
			sf::Image* dst = &img2;


			if (!paused)
			{
				for (int r = 0; r < height; ++r)
				{
					for (int c = 0; c < width; ++c)
					{
						int alive = 0;
						for (int nr = r-1; nr < (r+2); ++nr)
						{
							if (nr >= 0 && nr < height)
							{
								for (int nc = c-1; nc < (c+2); ++nc)
								{
									if (nc >= 0 && nc < width && !(nr == r && nc == c) && src[(nr * width + nc)*4] == 255) ++alive; 
								}
							}
						}
						if (src[(r * width + c)*4] == 255)
						{
							if (alive < 2 || alive > 3) dst->setPixel(c, r, sf::Color::Black);
						}
						else
						{
							if (alive == 3) dst->setPixel(c, r, sf::Color::White);
						}
					}
				}
				img1.copy(img2, 0, 0);
			}
			tex.loadFromImage(img1);
			window.draw(sprite);
			
			window.display();
			if(++frame >= 60)
			{
				float currentTime = clock.restart().asSeconds();
				std::cout << currentTime << '\n';
				lastTime = currentTime;
				frame = 0;
			}
		}
	}
	
private:
	void fillPattern1(sf::Image& img)
	{
		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				if (((x+y)%2)==0) img.setPixel(x, y, sf::Color::White);
				else img.setPixel(x, y, sf::Color::Black);
			}
		}
	}

	void fillSpaceShip(sf::Image& img, const sf::Vector2f &p)
	{
		// TODO create library of patterns and use image copy
		sf::Vector2u pos(p);
		if (pos.x < 3 || pos.x >= width-3 || pos.y < 3 || pos.y >= height-3) return;
		img.setPixel(p.x  , p.y  , sf::Color::Black);
		img.setPixel(p.x+1, p.y  , sf::Color::White);
		img.setPixel(p.x+2, p.y  , sf::Color::Black);
		img.setPixel(p.x  , p.y+1, sf::Color::Black);
		img.setPixel(p.x+1, p.y+1, sf::Color::Black);
		img.setPixel(p.x+2, p.y+1, sf::Color::White);
		img.setPixel(p.x  , p.y+2, sf::Color::White);
		img.setPixel(p.x+1, p.y+2, sf::Color::White);
		img.setPixel(p.x+2, p.y+2, sf::Color::White);
	}

	sf::RenderWindow window;
	sf::Image img1, img2;
	sf::Texture tex;
	sf::Sprite sprite;
	sf::View view;
	// TODO avoid 2 copies per frame
	bool readFromFirst = true;
	bool paused = false;

	const unsigned int width = 100;
	const unsigned int height = 75;
	const unsigned int scale = 10;

};

int main()
{
	App app;
	app.init();
	app.run();
	
	return 0;
}
