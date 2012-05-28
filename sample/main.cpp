#include "LTBL/LightSystem.h"

#include <assert.h>

#include <SFML/Graphics.hpp>

unsigned int GetFrameRate(const sf::Uint32 time)
{
  static unsigned int frameCounter = 0;
  static unsigned int frameTime = 0;
  static unsigned int fps = 0;

  ++frameCounter;

  frameTime += time;
  if (frameTime >= 1000)
  {
    fps = frameCounter;
    frameCounter = 0;
    frameTime -= 1000;
  }

  return fps;
}

int main(int argc, char* args[])
{ 
	sf::VideoMode vidMode;
	vidMode.width = 800;
	vidMode.height = 600;
	vidMode.bitsPerPixel = 32;
	assert(vidMode.isValid());

	sf::RenderWindow win;
	win.create(vidMode, "Let there be Light - Demo");

	// ---------------------- Background Image ---------------------

	sf::Texture backgroundImage;

	assert(backgroundImage.loadFromFile("data/background.png"));

	// Tiling background
	backgroundImage.setRepeated(true);

	sf::Sprite backgroundSprite(backgroundImage);
	backgroundSprite.setTextureRect(sf::IntRect(0, 0, vidMode.width, vidMode.height));

	// --------------------- Light System Setup ---------------------

	ltbl::LightSystem ls(qdt::AABB(Vec2f(0.0f, 0.0f), Vec2f(static_cast<float>(vidMode.width), static_cast<float>(vidMode.height))), &win);

	// Create a light
	ltbl::Light* testLight = new ltbl::Light();
	testLight->center = Vec2f(200.0f, 200.0f);
	testLight->radius = 500.0f;
	testLight->size = 30.0f;
	testLight->softSpreadAngle = 0.0f;
	testLight->CalculateAABB();

	ls.AddLight(testLight);

	// Create a hull by loading it from a file
	ltbl::ConvexHull* testHull = new ltbl::ConvexHull();

	if(!testHull->LoadShape("data/testShape.txt"))
		abort();

	// Pre-calculate certain aspects
	testHull->CalculateNormals();
	testHull->GenerateAABB();

	testHull->SetWorldCenter(Vec2f(100.0f, 100.0f));

	ls.AddConvexHull(testHull);

	// Create a hull by loading it from a file
	ltbl::ConvexHull* testHull2 = new ltbl::ConvexHull();

	if(!testHull2->LoadShape("data/testShape.txt"))
		abort();

	// Pre-calculate certain aspects
	testHull2->CalculateNormals();
	testHull2->GenerateAABB();

	testHull2->SetWorldCenter(Vec2f(300.0f, 300.0f));

	ls.AddConvexHull(testHull2);

	// ------------------------- Game Loop --------------------------
  
  sf::Text fpsText("0 FPS", sf::Font::getDefaultFont(), 15);
  fpsText.setPosition(10, 10);

  sf::Clock clock;
	sf::Event eventStructure;

	bool quit = false;
	while(!quit)
	{
		while(win.pollEvent(eventStructure))
			if(eventStructure.type == sf::Event::Closed)
			{
				quit = true;
				break;
			}

		sf::Vector2i mousePos = sf::Mouse::getPosition(win);

		// Update light
		testLight->center.x = static_cast<float>(mousePos.x);
		testLight->center.y = static_cast<float>(vidMode.height - mousePos.y);
		testLight->UpdateTreeStatus();

		win.clear();

		// Draw the background
		win.draw(backgroundSprite);

		// Calculate the lights
		ls.RenderLights();

		// Draw the lights
		ls.RenderLightTexture(0.0f);

    win.draw(fpsText);

		win.display();

    const sf::Uint32 elapsedTime = clock.restart().asMilliseconds();
    unsigned int fps = GetFrameRate(elapsedTime);

    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%d FPS", fps);
    fpsText.setString(buffer);
	}

	win.close();
}
