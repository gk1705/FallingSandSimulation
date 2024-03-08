#pragma once
#include <SFML/Graphics.hpp>

#include "helpers.h"
#include "Particle.h"
#include "ParticleFactory.h"
#include "WindowSettings.h"
#include "ParticleChunk.h"

class ParticleGrid
{
public:
	int rowCount;
	int columnCount;
	int particleSize;
	std::vector<ParticleChunk> chunks;
	bool drawDebugRectangles = true;

	ParticleGrid(WindowSettings windowSettings);
	~ParticleGrid();
	Particle& operator[](int index) const;
	Particle& at(sf::Vector2i point) const;

	int convertCoordinatesToIndex(int x, int y) const;
	sf::Vector2i convertIndexToCoordinates(int index) const;

	bool isInBoundsAndEmpty(sf::Vector2i point) const;
	bool isInBoundsAndNotEmpty(sf::Vector2i point) const;
	bool isValidMove(sf::Vector2i from, sf::Vector2i to) const;
	static bool isPointInsideRect(sf::Vector2i start, sf::Vector2i end, sf::Vector2i point);

	sf::Vector2i getParticleCoordinates(int index) const;
	int getParticleIndex(sf::Vector2i point) const;
	sf::Vector2i getValidMove(sf::Vector2i from, sf::Vector2i to) const;
	std::vector<sf::Vector2i> getWayPoints(sf::Vector2i point1, sf::Vector2i point2) const;
	sf::Vector2i getWayPointByIndex(sf::Vector2i point1, sf::Vector2i point2, int index) const;
	sf::Vector2i findPointBeforeCollision(sf::Vector2i point1, sf::Vector2i point2) const;
	std::vector<sf::Vector2i> getMidpointsInRectangle(sf::Vector2i start, sf::Vector2i end) const;
	std::vector<Particle> getParticlesFromPoints(const std::vector<sf::Vector2i>& points) const;
	sf::Vector2i getCellMiddlePoint(sf::Vector2i point) const;
	int getCardinalDirectionsAreOfTypeCount(sf::Vector2i position, ParticleType type) const;

	void setParticle(sf::Vector2i point, ParticleType type);
	void setParticle(sf::Vector2i point, const Particle& particle);
	void setEmptyParticle(sf::Vector2i point);
	void swapParticles(sf::Vector2i point1, sf::Vector2i point2);
	void setParticlesFromPoints(const std::vector<sf::Vector2i>& points, const std::vector<Particle>& copiedParticles);
	void performMove(sf::Vector2i from, sf::Vector2i to, const Particle& particle);

	void drawLine(sf::Vector2i point1, sf::Vector2i point2, const Particle& particle);
	void drawHollowCircle(sf::Vector2i center, int radius, const Particle& particle);
	void drawFilledCircle(sf::Vector2i center, int radius, ParticleType type);
	void fillRectangle(sf::Vector2i start, sf::Vector2i end, ParticleType type);
	void debugDraw(sf::RenderWindow& window) const;

	void initialClear();
	void clear();
	void eraseCircle(sf::Vector2i center, int radius);
	void clearRectangle(sf::Vector2i start, sf::Vector2i end);
	void clearParticlesFromPoints(const std::vector<sf::Vector2i>& points);

	void checkWayPointsForFreeDownPositionAndOccupy(sf::Vector2i gridPosition, sf::Vector2i& finalPosition) const;
	void checkWayPointsForFreeUpPositionAndOccupy(sf::Vector2i gridPosition, sf::Vector2i& finalPosition) const;

	void wakeupDormantWayPoints(sf::Vector2i from, sf::Vector2i to);
	void wakeUpDormantNeighborsForce(sf::Vector2i point);
	void wakeUpDormantNeighbors(sf::Vector2i point, ParticleType selfType);
	void wakeUp(sf::Vector2i gridPosition, ParticleType selfType);

	void swapDirtyRects();
	void expandDirtyRect(sf::Vector2i point);

	void transferMomentum(sf::Vector2i gridPosition) const;
	static void applyFriction(Particle& particle);

private:
	Particle* particles;
	int particleCount;

	ParticleFactory particleFactory;
	std::vector<sf::Vector2i> cardinalDirections;

	void setParticle(int index, const Particle& particle) const;
	void registerInChunk(sf::Vector2i point);
	void unregisterInChunk(sf::Vector2i point);

	void drawDebugDirtyRectangle(sf::RenderWindow& window, const ParticleChunk& chunk) const;
};