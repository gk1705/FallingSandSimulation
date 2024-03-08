#pragma once

#include <SFML/Graphics.hpp>

class IInputTool
{
public:
	virtual void handleInput(const sf::Event& event) = 0;
	virtual void render() = 0;
	virtual ~IInputTool() = default;
};