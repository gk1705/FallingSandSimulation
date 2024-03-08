#include "ParticleFactory.h"
#include "EngineConstants.h"

ParticleFactory::ParticleFactory()
{
	const sf::Color black(0x000000);
	const sf::Color white(0xffffffff);
	offsetColors.push_back(interpolateColors(black, white, 0.75f));
	offsetColors.push_back(interpolateColors(black, white, 0.50f));
	offsetColors.push_back(interpolateColors(black, white, 0.25f));

	particles[ParticleType::Empty] = Particle(ParticleType::Empty, ParticleState::Dormant, ParticleLocomotionType::Static, 0, 0, sf::Vector2i(0, 0), resolveParticleColor(ParticleType::Empty));
	particles[ParticleType::Concrete] = Particle(ParticleType::Concrete, ParticleState::Dormant, ParticleLocomotionType::Static, 0, 3, sf::Vector2i(0, 0), resolveParticleColor(ParticleType::Concrete));
	particles[ParticleType::Sand] = Particle(ParticleType::Sand, ParticleState::Active, ParticleLocomotionType::Dynamic, 0, 3, sf::Vector2i(0, EngineConstants::GRAVITY), resolveParticleColor(ParticleType::Sand));
	particles[ParticleType::Water] = Particle(ParticleType::Water, ParticleState::Active, ParticleLocomotionType::Dynamic, 0, 2, sf::Vector2i(0, EngineConstants::GRAVITY), resolveParticleColor(ParticleType::Water));
	particles[ParticleType::Smoke] = Particle(ParticleType::Smoke, ParticleState::Active, ParticleLocomotionType::Dynamic, 0, 1, sf::Vector2i(0, -EngineConstants::AIRSUSPENSIONRATE), resolveParticleColor(ParticleType::Smoke));
	particles[ParticleType::Fire] = Particle(ParticleType::Fire, ParticleState::Active, ParticleLocomotionType::Static, EngineConstants::BURNINGFIRELIFETIME, 3, sf::Vector2i(0, 0), resolveParticleColor(ParticleType::Fire));
	particles[ParticleType::Wood] = Particle(ParticleType::Wood, ParticleState::Active, ParticleLocomotionType::Static, 0, 3, sf::Vector2i(0, 0), resolveParticleColor(ParticleType::Wood));
}

Particle ParticleFactory::getParticle(ParticleType type)
{
	Particle particle = particles[type];
	if (type != ParticleType::Empty && type != ParticleType::Water && type != ParticleType::Smoke)
	{
		particle.color = getOffsetColor(particle.color);
	}

	return particle;
}

sf::Color ParticleFactory::getOffsetColor(sf::Color color) const
{
	static bool add = true;
	static int index = 0;

	if (add)
	{
		index++;
		if (index == offsetColors.size() - 1)
		{
			add = false;
		}
		return color + offsetColors[index];
	}
	else
	{
		index--;
		if (index == 0)
		{
			add = true;
		}
		return color * offsetColors[index];
	}
}

sf::Color ParticleFactory::resolveParticleColor(ParticleType type) const
{
	static const sf::Color black(0x000000);
	static const sf::Color white(0xffffffff);
	static const sf::Color concrete(0x808080ff);
	static const sf::Color sand(243, 224, 59);
	static const sf::Color water(sf::Color(0, 0, 200, 200));
	static const sf::Color pink(255, 192, 203);
	static const sf::Color smoke(105, 105, 105, 100);
	static const sf::Color wood(139, 69, 19);
	static const sf::Color fire(255, 69, 0);

	switch (type)
	{
	case ParticleType::Empty:
		return black;
	case ParticleType::Concrete:
		return getOffsetColor(concrete);
	case ParticleType::Sand:
		return getOffsetColor(sand);
	case ParticleType::Water:
		return water;
	case ParticleType::Smoke:
		return smoke;
	case ParticleType::Wood:
		return wood;
	case ParticleType::Fire:
		return fire;
	default:
		return pink;
	}
}
