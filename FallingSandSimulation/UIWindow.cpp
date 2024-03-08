#include "UIWindow.h"

#include "imgui/imgui-SFML.h"
#include "imgui/imgui.h"

UIWindow::UIWindow(sf::RenderWindow& window, ParticleGrid& particleGrid, ParticleSimulator& particleSimulator,
                   InputHandler& inputHandler, ParticleTypeIterator& particleTypeIterator, InputToolIterator& inputToolIterator): window(window), particleGrid(particleGrid), particleSimulator(particleSimulator), inputHandler(inputHandler), particleTypeIterator(particleTypeIterator), inputToolIterator(inputToolIterator)
{
	screenWidth = window.getSize().x;
}

void UIWindow::render()
{
	renderGUIWindow();
	renderClearButton();
	renderSimulationCheckbox();
	renderParticleButtons();
	renderToolButtons();
	renderDebugRectanglesCheckbox();

	ImGui::End();
	ImGui::SFML::Render(window);
}

void UIWindow::renderGUIWindow()
{
	ImGui::SFML::Update(window, deltaClock.restart());
	ImGui::Begin("Toolkit Window");
	ImGui::SetWindowSize(ImVec2(280, 300), ImGuiCond_::ImGuiCond_Once);
}

void UIWindow::renderClearButton() const
{
	ImGui::SetWindowPos(ImVec2(screenWidth - 280, 0), ImGuiCond_::ImGuiCond_Once);
	if (ImGui::Button("Clear"))
	{
		particleGrid.clear();
	}
	ImGui::SetItemTooltip("Alternatively: Press 'R'"
		"\n\nHint: Mouse and keyboard inputs are not captured while hovering the toolkit window");
}

void UIWindow::renderSimulationCheckbox() const
{
	ImGui::SameLine();
	static bool simulate = true;
	if (ImGui::Checkbox("Simulate", &simulate))
	{
		particleSimulator.simulate = simulate;
	}
	ImGui::SetItemTooltip("Press space-bar to toggle the simulation"
		"\n\nHint: Mouse and keyboard inputs are not captured while hovering the toolkit window");
}

void UIWindow::renderParticleButtons() const
{
	for (auto& type : particleTypeIterator.particleTypes)
	{
		renderColoredButton((int)type, particleTypeIterator.particleNames[type], rgbaToHsv(particleTypeIterator.particleColors[type]));
		ImGui::SetItemTooltip("You can use 'A' and 'D' to iterate through the particle types"
			"\n\nHint: Mouse and keyboard inputs are not captured while hovering the toolkit window");
	}
	ImGui::Text(particleTypeIterator.particleTypeString.c_str());
}

void UIWindow::renderColoredButton(int id, const std::string& name, sf::Vector3f hsvColor) const
{
	const float buttonHue = hsvColor.x;
	const float buttonSaturation = hsvColor.y;
	const float buttonValue = hsvColor.z;

	if (id != 1 && id % 5 != 0)
	{
		ImGui::SameLine();
	}

	ImGui::PushID(id);
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(buttonHue, buttonSaturation - 0.1f, buttonValue - 0.1f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(buttonHue, buttonSaturation, buttonValue));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(buttonHue, buttonSaturation + 0.1f, buttonValue + 0.1f));
	if (ImGui::Button(name.c_str()))
	{
		particleTypeIterator.currentType = (ParticleType)id;
		particleTypeIterator.particleTypeString = "Selected Type: " + particleTypeIterator.particleNames[particleTypeIterator.currentType];
	}
	ImGui::PopStyleColor(3);
	ImGui::PopID();
}

void UIWindow::renderToolButtons() const
{
	renderImageButton(1, "Stamp", inputToolIterator.stampTextureHandle, "Left Mouse Button: Spawn Particles\nRight Mouse Button: Delete Particles\n"
	                  "Mouse Wheel Up or Down: Increase or Decrease radius"
	                  "\n\nHint: Mouse and keyboard inputs are not captured while hovering the toolkit window");
	renderImageButton(2, "Rectangle", inputToolIterator.rectangleTextureHandle, "Hold Left Mouse Button: Draw Rectangle\nF Key: Fill with Particles\n"
	                  "C Key: Delete Particles\nMiddle Mouse Button: Drag Rectangle (and the contents inside of it)"
	                  "\n\nHint: Mouse and keyboard inputs are not captured while hovering the toolkit window");
	ImGui::Text(inputToolIterator.toolTypeString.c_str());
}

void UIWindow::renderImageButton(int id, const std::string& name, unsigned textureHandle,
	const std::string& tooltip) const
{
	if (id != 1)
	{
		ImGui::SameLine();
	}

	if (ImGui::ImageButton((void*)(intptr_t)textureHandle, ImVec2(30, 30)))
	{
		inputHandler.setTool(name);
	}
	ImGui::SetItemTooltip(tooltip.c_str());
}

void UIWindow::renderDebugRectanglesCheckbox() const
{
	static bool drawDebugRectangles = true;
	if (ImGui::Checkbox("Active chunks and dirty areas", &drawDebugRectangles))
	{
		particleGrid.drawDebugRectangles = drawDebugRectangles;
	}
}

