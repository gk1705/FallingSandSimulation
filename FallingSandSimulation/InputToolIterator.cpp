#include "InputToolIterator.h"

InputToolIterator::InputToolIterator()
{
	stampTexture.loadFromFile("stamp.png");
	stampTextureHandle = stampTexture.getNativeHandle();
	rectangleTexture.loadFromFile("rectangle.jpg");
	rectangleTextureHandle = rectangleTexture.getNativeHandle();
}
