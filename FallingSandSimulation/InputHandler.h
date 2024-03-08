#pragma once

#include <SFML/Graphics.hpp>
#include "ParticleGrid.h"
#include "ParticleSimulator.h"
#include "RectangleTool.h"
#include "StampTool.h"
#include "InputToolIterator.h"

class InputHandler
{
public:
    InputHandler(ParticleGrid& particleGrid, ParticleSimulator& particleSimulator, ParticleTypeIterator& particleTypeIterator, InputToolIterator& inputToolIterator, StampTool& stampTool, RectangleTool& rectangleTool);
    void handleInput(const sf::Event& event);
    void render() const;
    void setTool(const std::string& toolName);

private:
    ParticleGrid& particleGrid;
    ParticleSimulator& particleSimulator;
    ParticleTypeIterator& particleTypeIterator;
    InputToolIterator& inputToolIterator;

    std::unordered_map<std::string, IInputTool*> inputTools;
    IInputTool* currentTool;
};