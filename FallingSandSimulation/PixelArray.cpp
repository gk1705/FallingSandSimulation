#include "PixelArray.h"

PixelArray::PixelArray(ParticleGrid& particleGrid, WindowSettings windowSettings): particleGrid(particleGrid), screenWidth(windowSettings.screenWidth), screenHeight(windowSettings.screenHeight), particleSize(windowSettings.particleSize), rowCount(screenHeight / windowSettings.particleSize), columnCount(screenWidth / windowSettings.particleSize)
{
	pixels = new sf::Uint8[screenWidth * screenHeight * 4];
}

PixelArray::~PixelArray()
{
	delete[] pixels;
}

void PixelArray::updatePixelArray() const
{
	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < columnCount; j++)
		{
			const int index = i * columnCount + j;
			const sf::Color color = particleGrid[index].color;
			const int pixelIndex = (i * particleSize * screenWidth * 4) + (j * particleSize * 4);
			for (int k = 0; k < particleSize; k++)
			{
				for (int l = 0; l < particleSize; l++)
				{
					pixels[pixelIndex + (k * screenWidth * 4) + (l * 4)] = color.r;
					pixels[pixelIndex + (k * screenWidth * 4) + (l * 4) + 1] = color.g;
					pixels[pixelIndex + (k * screenWidth * 4) + (l * 4) + 2] = color.b;
					pixels[pixelIndex + (k * screenWidth * 4) + (l * 4) + 3] = color.a;
				}
			}
		}
	}
}

