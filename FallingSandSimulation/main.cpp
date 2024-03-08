#include <chrono>
#include <thread>
#include <iostream>

#include <SFML/Graphics.hpp>
#include "ParticleGrid.h"
#include "ParticleSimulator.h"
#include "PixelArray.h"
#include "InputHandler.h"
#include "UIWindow.h"

#include "helpers.h"

#include "imgui/imgui-SFML.h"
#include "WindowSettings.h"
#include "imgui/imgui.h"

void printFrameTime(auto elapsedTime)
{
	std::cout << "Elapsed time: " << elapsedTime.count() << std::endl;
}

void printFPS(auto currentTime, auto previousTime)
{
	float fps = (float)1e9 / (float) std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - previousTime).count();
	std::cout << "FPS: " << fps << std::endl;
}

int main()
{
	WindowSettings windowSettings;
	initializeRandomDoubles();
	initializeRandomIntegers();

	ParticleGrid particleGrid(windowSettings);
	ParticleSimulator particleSimulator(particleGrid);

	PixelArray pixelArray(particleGrid, windowSettings);
	pixelArray.updatePixelArray();

	sf::Texture screenTexture;
	screenTexture.create(windowSettings.screenWidth, windowSettings.screenHeight);
	screenTexture.update(pixelArray.pixels);
	sf::Sprite screenSprite = sf::Sprite(screenTexture);

	sf::RenderWindow window(sf::VideoMode(windowSettings.screenWidth, windowSettings.screenHeight), "Grid Particle Simulation", sf::Style::Close);

	ParticleTypeIterator particleTypeIterator;
	InputToolIterator inputToolIterator;

	StampTool stampTool(window, particleGrid, particleTypeIterator, 25);
	RectangleTool rectangleTool(window, particleGrid, particleTypeIterator);

	InputHandler inputHandler(particleGrid, particleSimulator, particleTypeIterator, inputToolIterator, stampTool, rectangleTool);
	UIWindow uiWindow(window, particleGrid, particleSimulator, inputHandler, particleTypeIterator, inputToolIterator);

	auto frameDuration = std::chrono::milliseconds(16);
	auto previousTime = std::chrono::high_resolution_clock::now();
	auto lag = std::chrono::milliseconds(0);

	ImGui::SFML::Init(window);
	ImGui::CaptureMouseFromApp(true);
	ImGui::CaptureKeyboardFromApp(true);

	while (window.isOpen())
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - previousTime);

		previousTime = currentTime;
		lag += elapsedTime;

		sf::Event event{};
		while (window.pollEvent(event))
		{
			ImGui::SFML::ProcessEvent(event);
			auto& io = ImGui::GetIO();
			if (io.WantCaptureMouse || io.WantCaptureKeyboard) continue;

			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			inputHandler.handleInput(event);
		}

		// limit simulation updates to a fixed time step
		while (lag >= frameDuration)
		{
			lag -= frameDuration;
			particleSimulator.simulateParticles();
		}

		pixelArray.updatePixelArray();
		screenTexture.update(pixelArray.pixels);
		window.clear();
		window.draw(screenSprite);
		particleGrid.debugDraw(window);
		inputHandler.render();
		uiWindow.render();
		window.display();
	}

	ImGui::SFML::Shutdown();
	return 0;
}