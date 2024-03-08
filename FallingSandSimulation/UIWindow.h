#pragma once
#include <SFML/Graphics.hpp>
#include "ParticleGrid.h"
#include "helpers.h"
#include "ParticleTypeIterator.h"
#include "InputToolIterator.h"
#include "InputHandler.h"

class UIWindow
{
public:
	UIWindow(sf::RenderWindow& window, ParticleGrid& particleGrid, ParticleSimulator& particleSimulator, InputHandler& inputHandler, ParticleTypeIterator& particleTypeIterator, InputToolIterator& inputToolIterator);

	void render();

private:
	sf::RenderWindow& window;
	sf::Clock deltaClock;
	ParticleGrid& particleGrid;
	ParticleSimulator& particleSimulator;
	InputHandler& inputHandler;

	ParticleTypeIterator& particleTypeIterator;
	InputToolIterator& inputToolIterator;

	int screenWidth{};

	void renderGUIWindow();
	void renderClearButton() const;
	void renderSimulationCheckbox() const;
	void renderParticleButtons() const;
	void renderColoredButton(int id, const std::string& name, sf::Vector3f hsvColor) const;
	void renderToolButtons() const;
	void renderImageButton(int id, const std::string& name, unsigned textureHandle, const std::string& tooltip) const;
	void renderDebugRectanglesCheckbox() const;
};
