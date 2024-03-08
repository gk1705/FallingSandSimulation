#pragma once

#include <string>

#include <SFML/Graphics/Texture.hpp>

class InputToolIterator
{
public:
	std::string toolTypeString = "Selected Tool: None";
	sf::Texture stampTexture;
	unsigned stampTextureHandle = 0;
	sf::Texture rectangleTexture;
	unsigned rectangleTextureHandle = 0;
	std::string currentTool = "None";

	int textureHeight = 45;
	int textureWidth = 45;

	InputToolIterator();
};
