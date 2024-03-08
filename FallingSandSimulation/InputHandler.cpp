#include "InputHandler.h"

InputHandler::InputHandler(ParticleGrid& particleGrid, ParticleSimulator& particleSimulator,
	ParticleTypeIterator& particleTypeIterator, InputToolIterator& inputToolIterator, StampTool& stampTool,
	RectangleTool& rectangleTool): particleGrid(particleGrid), particleSimulator(particleSimulator), particleTypeIterator(particleTypeIterator), inputToolIterator(inputToolIterator)
{
	inputTools["Stamp"] = &stampTool;
	inputTools["Rectangle"] = &rectangleTool;
	currentTool = inputTools["Stamp"];
}

void InputHandler::handleInput(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space)
	{
		particleSimulator.toggleSimulation();
	}
	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D)
	{
		particleTypeIterator.iterateParticleTypeUp();
	}
	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A)
	{
		particleTypeIterator.iterateParticleTypeDown();
	}
	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R)
	{
		particleGrid.clear();
	}
	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q)
	{
		setTool("Rectangle");
	}
	else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::S)
	{
		setTool("Stamp");
	}
	else if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::P)
	{
		particleSimulator.simulateParticles();
	}
	else
	{
		currentTool->handleInput(event);
	}
}

void InputHandler::render() const
{
	currentTool->render();
}

void InputHandler::setTool(const std::string& toolName)
{
	currentTool = inputTools[toolName];
	inputToolIterator.toolTypeString = "Selected Tool: " + toolName;
}
