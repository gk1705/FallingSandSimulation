#pragma once

#include <random>
#include <cmath>
#include <array>

#include <SFML/Graphics.hpp>

inline bool isNearlyZero(double value, double epsilon = 1e-9)
{
	return std::fabs(value) < epsilon;
}

inline double generateRandomDouble() // normalized, name should reflect that
{
	// static is used here so the engine and distribution are only initialized once.
	static std::random_device rd;
	static std::mt19937 eng(rd());
	static std::uniform_real_distribution<> distr(0, 1); // range

	return distr(eng);
}

inline int generateRandomInt()
{
	// static is used here so the engine and distribution are only initialized once.
	static std::random_device rd;
	static std::mt19937 eng(rd());
	static std::uniform_int_distribution<> distr(0, RAND_MAX); // range

	return distr(eng);
}

inline constexpr size_t randomDoubleArraySize = 10000;
inline std::array<double, randomDoubleArraySize> randomDoubles{};

inline void initializeRandomDoubles()
{
	for (int i = 0; i < randomDoubleArraySize; i++)
	{
		randomDoubles[i] = generateRandomDouble();
	}
}

inline size_t randomDoubleIndex = 0;

inline double getPseudoRandomDouble()
{
	const double randomNumber = randomDoubles[randomDoubleIndex];
	randomDoubleIndex = ++randomDoubleIndex % (randomDoubleArraySize - 100); // hack, so a race condition doesnt cause an out of bounds access
	return randomNumber;
}

inline constexpr size_t randomIntegersArraySize = 10000;
inline std::array<int, randomIntegersArraySize> randomIntegers{};

inline void initializeRandomIntegers()
{
	for (int i = 0; i < randomIntegersArraySize; i++)
	{
		randomIntegers[i] = generateRandomInt();
	}
}

inline size_t randomIntegerIndex = 0;

inline int getPseudoRandomInteger()
{
	const int randomNumber = randomIntegers[randomIntegerIndex];
	randomIntegerIndex = ++randomIntegerIndex % (randomIntegersArraySize - 100); // hack, so a race condition doesnt cause an out of bounds access
	return randomNumber;
}

inline float getHueFromRGBA(sf::Color color)
{
	const float r = color.r / 255.0f;
	const float g = color.g / 255.0f;
	const float b = color.b / 255.0f;

	const float max = std::max(r, std::max(g, b));
	const float min = std::min(r, std::min(g, b));

	float hue = 0;
	if (isNearlyZero(max - r))
	{
		hue = (g - b) / (max - min);
	}
	else if (isNearlyZero(max - g))
	{
		hue = 2.0f + (b - r) / (max - min);
	}
	else
	{
		hue = 4.0f + (r - g) / (max - min);
	}
	hue *= 60.0f;
	if (hue < 0) hue += 360.0f;

	return hue / 360.0f;
}

inline sf::Vector3f rgbaToHsv(sf::Color rgbaColor)
{
	float r = rgbaColor.r / 255.0f;
	float g = rgbaColor.g / 255.0f;
	float b = rgbaColor.b / 255.0f;

	const float max = std::max({ r, g, b });
	const float min = std::min({ r, g, b });
	const float delta = max - min;

	float h = 0, s = 0, v = max;

	if (!isNearlyZero(delta))
	{
		if (isNearlyZero(max - r))
		{
			h = 60 * fmod(((g - b) / delta), 6);
		}
		else if (isNearlyZero(max - g))
		{
			h = 60 * (((b - r) / delta) + 2);
		}
		else if (isNearlyZero(max - b))
		{
			h = 60 * (((r - g) / delta) + 4);
		}

		s = isNearlyZero(max) ? 0 : delta / max;
	}

	if (h < 0)
	{
		h += 360;
	}

	sf::Vector3f hsvColor(h, s, v);
	if (hsvColor.x > 1.0f) hsvColor.x /= 360.0f;
	if (hsvColor.y > 1.0f) hsvColor.y /= 100.0f;
	if (hsvColor.z > 1.0f) hsvColor.z /= 100.0f;
	return hsvColor;
}

inline sf::Color interpolateColors(sf::Color color1, sf::Color color2, float t)
{
	const float r = color1.r + (color2.r - color1.r) * t;
	const float g = color1.g + (color2.g - color1.g) * t;
	const float b = color1.b + (color2.b - color1.b) * t;
	const float a = color1.a + (color2.a - color1.a) * t;

	return sf::Color(r, g, b, a);
}

inline int gcd(int a, int b)
{
	while (b != 0)
	{
		const int temp = b;
		b = a % b;
		a = temp;
	}
	return a;
}