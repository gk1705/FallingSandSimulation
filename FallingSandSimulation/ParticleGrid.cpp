#include "ParticleGrid.h"

ParticleGrid::ParticleGrid(WindowSettings windowSettings): rowCount(windowSettings.screenHeight / windowSettings.particleSize), columnCount(windowSettings.screenWidth / windowSettings.particleSize), particleSize(windowSettings.particleSize)
{
	particleCount = rowCount * columnCount;
	particles = new Particle[particleCount];
	initialClear();

	// greatest common divisor between columnCount and rowCount for chunk size
	const int chunkSize = gcd(windowSettings.screenWidth / particleSize, windowSettings.screenHeight / particleSize);
	for (int i = 0; i < rowCount; i += chunkSize)
	{
		for (int j = 0; j < columnCount; j += chunkSize)
		{
			chunks.emplace_back(sf::Vector2i(j, i), sf::Vector2i(j + chunkSize - 1, i + chunkSize - 1));
		}
	}

	sf::Vector2i up = sf::Vector2i(0, 1);
	sf::Vector2i down = sf::Vector2i(0, -1);
	sf::Vector2i left = sf::Vector2i(-1, 0);
	sf::Vector2i right = sf::Vector2i(1, 0);
	sf::Vector2i upLeft = sf::Vector2i(-1, 1);
	sf::Vector2i upRight = sf::Vector2i(1, 1);
	sf::Vector2i downLeft = sf::Vector2i(-1, -1);
	sf::Vector2i downRight = sf::Vector2i(1, -1);
	cardinalDirections = { up, down, left, right, upLeft, upRight, downLeft, downRight };
}

ParticleGrid::~ParticleGrid()
{
	delete[] particles;
}

Particle& ParticleGrid::operator[](int index) const
{
	return particles[index];
}

Particle& ParticleGrid::at(sf::Vector2i point) const
{
	return particles[point.y * columnCount + point.x];
}

void ParticleGrid::setParticle(sf::Vector2i point, ParticleType type)
{
	if (point.x < 0 || point.x >= columnCount || point.y < 0 || point.y >= rowCount)
	{
		return;
	}

	const int index = point.y * columnCount + point.x;
	const Particle particle = particleFactory.getParticle(type);

	if (type == ParticleType::Empty)
	{
		unregisterInChunk(point);
	}
	else
	{
		registerInChunk(point);
		expandDirtyRect(point);
	}
	setParticle(index, particle);
}

void ParticleGrid::setParticle(sf::Vector2i point, const Particle& particle)
{
	if (point.x < 0 || point.x >= columnCount || point.y < 0 || point.y >= rowCount)
	{
		return;
	}

	const int index = point.y * columnCount + point.x;

	if (particle.type == ParticleType::Empty)
	{
		unregisterInChunk(point);
	}
	else
	{
		registerInChunk(point);
		expandDirtyRect(point);
	}
	setParticle(index, particle);
}

void ParticleGrid::setParticle(int index, const Particle& particle) const
{
	if (index < 0 || index >= particleCount)
	{
		return;
	}
	if (particles[index].type == particle.type)
	{
		return;
	}

	particles[index] = particle;
}

void ParticleGrid::setEmptyParticle(sf::Vector2i point)
{
	wakeUpDormantNeighbors(point, ParticleType::Empty);
	setParticle(point, particleFactory.getParticle(ParticleType::Empty));
}

void ParticleGrid::swapParticles(sf::Vector2i point1, sf::Vector2i point2)
{
	wakeupDormantWayPoints(point1, point2);
	const Particle temp = at(point1);
	setParticle(point1, at(point2));
	setParticle(point2, temp);
}

sf::Vector2i ParticleGrid::getParticleCoordinates(int index) const
{
	return { index % columnCount, index / columnCount };
}

int ParticleGrid::getParticleIndex(sf::Vector2i point) const
{
	return point.y * columnCount + point.x;
}

void ParticleGrid::drawLine(sf::Vector2i point1, sf::Vector2i point2, const Particle& particle)
{
	int x0 = point1.x, y0 = point1.y;
	const int x1 = point2.x;
	const int y1 = point2.y;

	const int dx = std::abs(x1 - x0);
	const int sx = x0 < x1 ? 1 : -1;
	const int dy = -std::abs(y1 - y0);
	const int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;

	while (true)
	{
		setParticle(sf::Vector2i(x0, y0), particle);
		if (x0 == x1 && y0 == y1) break;
		const int e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

std::vector<sf::Vector2i> ParticleGrid::getWayPoints(sf::Vector2i point1, sf::Vector2i point2) const
{
	int x0 = point1.x, y0 = point1.y;
	int x1 = point2.x, y1 = point2.y;

	if (x0 < 0 || x0 >= columnCount || y0 < 0 || y0 >= rowCount)
	{
		x0 = std::max(0, std::min(x0, columnCount - 1));
		y0 = std::max(0, std::min(y0, rowCount - 1));
	}
	if (x1 < 0 || x1 >= columnCount || y1 < 0 || y1 >= rowCount)
	{
		x1 = std::max(0, std::min(x1, columnCount - 1));
		y1 = std::max(0, std::min(y1, rowCount - 1));
	}
	if (x0 == x1 && y0 == y1)
	{
		return std::vector<sf::Vector2i>{};
	}

	const int dx = std::abs(x1 - x0);
	const int sx = x0 < x1 ? 1 : -1;
	const int dy = -std::abs(y1 - y0);
	const int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;

	std::vector<sf::Vector2i> wayPoints;
	while (true)
	{
		wayPoints.emplace_back(x0, y0);
		if (x0 == x1 && y0 == y1) break;
		const int e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}

	return wayPoints;
}

sf::Vector2i ParticleGrid::findPointBeforeCollision(sf::Vector2i point1, sf::Vector2i point2) const
{
	int x0 = point1.x, y0 = point1.y;
	int x1 = point2.x, y1 = point2.y;

	if (x0 < 0 || x0 >= columnCount || y0 < 0 || y0 >= rowCount)
	{
		x0 = std::max(0, std::min(x0, columnCount - 1));
		y0 = std::max(0, std::min(y0, rowCount - 1));
	}
	if (x1 < 0 || x1 >= columnCount || y1 < 0 || y1 >= rowCount)
	{
		x1 = std::max(0, std::min(x1, columnCount - 1));
		y1 = std::max(0, std::min(y1, rowCount - 1));
	}
	if (x0 == x1 && y0 == y1)
	{
		return { x0, y0 };
	}

	const int dx = std::abs(x1 - x0);
	const int sx = x0 < x1 ? 1 : -1;
	const int dy = -std::abs(y1 - y0);
	const int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;

	while (true)
	{
		int previousX = x0;
		int previousY = y0;

		const int e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }

		if (at(sf::Vector2i(x0, y0)).type != ParticleType::Empty) return { previousX, previousY };
		else if (x0 == x1 && y0 == y1) return { x0, y0 };
	}
}

sf::Vector2i ParticleGrid::getWayPointByIndex(sf::Vector2i point1, sf::Vector2i point2, int index) const
{
	int x0 = point1.x, y0 = point1.y;
	int x1 = point2.x, y1 = point2.y;

	x0 = std::max(0, std::min(x0, columnCount - 1));
	y0 = std::max(0, std::min(y0, rowCount - 1));
	x1 = std::max(0, std::min(x1, columnCount - 1));
	y1 = std::max(0, std::min(y1, rowCount - 1));

	if ((x0 == x1 && y0 == y1) || index < 0)
	{
		return { -1, -1 };
	}

	const int dx = std::abs(x1 - x0);
	const int sx = x0 < x1 ? 1 : -1;
	const int dy = -std::abs(y1 - y0);
	const int sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;
	int waypointIndex = 0;

	while (true)
	{
		if (waypointIndex == index)
		{
			return { x0, y0 };
		}
		if (x0 == x1 && y0 == y1)
		{
			break; // reached the end without finding the index
		}
		const int e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
		++waypointIndex;
	}

	return { -1, -1 }; // index out of bounds
}

void ParticleGrid::drawHollowCircle(sf::Vector2i center, int radius, const Particle& particle)
{
	int x = radius, y = 0;
	int radiusError = 1 - x;

	while (x >= y)
	{
		setParticle(sf::Vector2i(center.x + x, center.y + y), particle);
		setParticle(sf::Vector2i(center.x + y, center.y + x), particle);
		setParticle(sf::Vector2i(center.x - y, center.y + x), particle);
		setParticle(sf::Vector2i(center.x - x, center.y + y), particle);
		setParticle(sf::Vector2i(center.x - x, center.y - y), particle);
		setParticle(sf::Vector2i(center.x - y, center.y - x), particle);
		setParticle(sf::Vector2i(center.x + y, center.y - x), particle);
		setParticle(sf::Vector2i(center.x + x, center.y - y), particle);

		y++;
		if (radiusError < 0)
		{
			radiusError += 2 * y + 1;
		}
		else
		{
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

void ParticleGrid::drawFilledCircle(sf::Vector2i center, int radius, ParticleType type)
{
	int x = radius, y = 0;
	int radiusError = 1 - x;

	if (radius == 0)
	{
		setParticle(center, particleFactory.getParticle(type));
		return;
	}

	while (x >= y)
	{
		for (int i = center.x - x; i <= center.x + x; i++)
		{
			setParticle(sf::Vector2i(i, center.y + y), particleFactory.getParticle(type));
			setParticle(sf::Vector2i(i, center.y - y), particleFactory.getParticle(type));
		}

		for (int i = center.x - y; i <= center.x + y; i++)
		{
			setParticle(sf::Vector2i(i, center.y + x), particleFactory.getParticle(type));
			setParticle(sf::Vector2i(i, center.y - x), particleFactory.getParticle(type));
		}

		y++;
		if (radiusError < 0)
		{
			radiusError += 2 * y + 1;
		}
		else
		{
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

void ParticleGrid::eraseCircle(sf::Vector2i center, int radius)
{
	int x = radius, y = 0;
	int radiusError = 1 - x;

	if (radius == 0)
	{
		setEmptyParticle(center);
		return;
	}

	while (x >= y)
	{
		for (int i = center.x - x; i <= center.x + x; i++)
		{
			setEmptyParticle(sf::Vector2i(i, center.y + y));
			setEmptyParticle(sf::Vector2i(i, center.y - y));
		}

		for (int i = center.x - y; i <= center.x + y; i++)
		{
			setEmptyParticle(sf::Vector2i(i, center.y + x));
			setEmptyParticle(sf::Vector2i(i, center.y - x));
		}

		y++;
		if (radiusError < 0)
		{
			radiusError += 2 * y + 1;
		}
		else
		{
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

void ParticleGrid::initialClear()
{
	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < columnCount; j++)
		{
			particles[i * columnCount + j] = particleFactory.getParticle(ParticleType::Empty);
		}
	}
}

void ParticleGrid::clear()
{
	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < columnCount; j++)
		{
			setEmptyParticle(sf::Vector2i(j, i));
		}
	}
}

int ParticleGrid::convertCoordinatesToIndex(int x, int y) const
{
	return y * columnCount + x;
}

sf::Vector2i ParticleGrid::convertIndexToCoordinates(int index) const
{
	return { index % columnCount, index / columnCount };
}

void ParticleGrid::fillRectangle(sf::Vector2i start, sf::Vector2i end, ParticleType type)
{
	const int xStart = std::min(start.x, end.x);
	const int xEnd = std::max(start.x, end.x) + 1;
	const int yStart = std::min(start.y, end.y);
	const int yEnd = std::max(start.y, end.y) + 1;

	const int horizontalCount = (xEnd - xStart);
	const int verticalCount = (yEnd - yStart);

	for (int i = 0; i < horizontalCount; i++)
	{
		for (int j = 0; j < verticalCount; j++)
		{
			setParticle(sf::Vector2i(xStart + i, yStart + j), particleFactory.getParticle(type));
		}
	}
}

void ParticleGrid::clearRectangle(sf::Vector2i start, sf::Vector2i end)
{
	const int xStart = std::min(start.x, end.x);
	const int xEnd = std::max(start.x, end.x) + 1;
	const int yStart = std::min(start.y, end.y);
	const int yEnd = std::max(start.y, end.y) + 1;

	const int horizontalCount = (xEnd - xStart);
	const int verticalCount = (yEnd - yStart);

	for (int i = 0; i < horizontalCount; i++)
	{
		for (int j = 0; j < verticalCount; j++)
		{
			setEmptyParticle(sf::Vector2i(xStart + i, yStart + j));
		}
	}
}

std::vector<sf::Vector2i> ParticleGrid::getMidpointsInRectangle(sf::Vector2i start, sf::Vector2i end) const
{
	const int xStart = std::min(start.x, end.x);
	const int xEnd = std::max(start.x, end.x);
	const int yStart = std::min(start.y, end.y);
	const int yEnd = std::max(start.y, end.y);

	start = sf::Vector2i(xStart, yStart);
	end = sf::Vector2i(xEnd, yEnd);

	std::vector<sf::Vector2i> points;
	for (int i = 0; i < rowCount; i++)
	{
		for (int j = 0; j < columnCount; j++)
		{
			if (isPointInsideRect(start, end, getCellMiddlePoint(sf::Vector2i(j, i))))
			{
				points.emplace_back(j, i);
			}
		}
	}
	return points;
}

std::vector<Particle> ParticleGrid::getParticlesFromPoints(const std::vector<sf::Vector2i>& points) const
{
	std::vector<Particle> copiedParticles;
	for (const sf::Vector2i point : points)
	{
		copiedParticles.push_back(at(point));
	}
	return copiedParticles;
}

void ParticleGrid::setParticlesFromPoints(const std::vector<sf::Vector2i>& points,
	const std::vector<Particle>& copiedParticles)
{
	if (points.size() != copiedParticles.size())
	{
		return;
	}
	for (int i = 0; i < points.size(); i++)
	{
		setParticle(points[i], copiedParticles[i]);
	}
}

void ParticleGrid::clearParticlesFromPoints(const std::vector<sf::Vector2i>& points)
{
	for (const sf::Vector2i point : points)
	{
		setParticle(point, particleFactory.getParticle(ParticleType::Empty));
	}
}

bool ParticleGrid::isPointInsideRect(sf::Vector2i start, sf::Vector2i end, sf::Vector2i point)
{
	return point.x >= start.x && point.x <= end.x && point.y >= start.y && point.y <= end.y;
}

sf::Vector2i ParticleGrid::getCellMiddlePoint(sf::Vector2i point) const
{
	return { point.x * particleSize + particleSize / 2, point.y * particleSize + particleSize / 2 };
}

void ParticleGrid::checkWayPointsForFreeDownPositionAndOccupy(sf::Vector2i gridPosition, sf::Vector2i& finalPosition) const
{
	for (int index = 1; ; ++index)
	{
		sf::Vector2i point = getWayPointByIndex(gridPosition, finalPosition, index);
		if (point == sf::Vector2i(-1, -1))
		{
			break;
		}

		const sf::Vector2i down = sf::Vector2i(point.x, point.y + 1);
		if (isInBoundsAndEmpty(down))
		{
			finalPosition = down;
			break;
		}
	}
}

void ParticleGrid::checkWayPointsForFreeUpPositionAndOccupy(sf::Vector2i gridPosition, sf::Vector2i& finalPosition) const
{
	for (int index = 1; ; ++index)
	{
		sf::Vector2i point = getWayPointByIndex(gridPosition, finalPosition, index);
		if (point == sf::Vector2i(-1, -1))
		{
			break;
		}

		const sf::Vector2i up = sf::Vector2i(point.x, point.y - 1);
		if (isInBoundsAndEmpty(up))
		{
			finalPosition = up;
			break;
		}
	}
}

bool ParticleGrid::isInBoundsAndEmpty(sf::Vector2i point) const
{
	if (point.x < 0 || point.x >= columnCount || point.y < 0 || point.y >= rowCount)
	{
		return false;
	}

	return at(point).type == ParticleType::Empty;
}

bool ParticleGrid::isInBoundsAndNotEmpty(sf::Vector2i point) const
{
	if (point.x < 0 || point.x >= columnCount || point.y < 0 || point.y >= rowCount)
	{
		return false;
	}

	return at(point).type != ParticleType::Empty;
}

sf::Vector2i ParticleGrid::getValidMove(sf::Vector2i from, sf::Vector2i to) const
{
	return findPointBeforeCollision(from, to);
}

bool ParticleGrid::isValidMove(sf::Vector2i from, sf::Vector2i to) const
{
	const sf::Vector2i endpoint = findPointBeforeCollision(from, to);
	return endpoint != from;
}

void ParticleGrid::performMove(sf::Vector2i from, sf::Vector2i to, const Particle& particle)
{
	// TODO: refactor in systems, used to have different logic
	swapParticles(from, to);
}

void ParticleGrid::wakeupDormantWayPoints(sf::Vector2i from, sf::Vector2i to)
{
	const Particle& fromParticle = at(from);
	for (int index = 0; ; ++index)
	{
		sf::Vector2i point = getWayPointByIndex(from, to, index);
		if (point == sf::Vector2i(-1, -1))
		{
			break;
		}

		wakeUpDormantNeighbors(point, fromParticle.type);
	}
}

void ParticleGrid::wakeUpDormantNeighborsForce(sf::Vector2i point)
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0)
			{
				continue;
			}
			const sf::Vector2i neighbor = sf::Vector2i(point.x + i, point.y + j);
			if (neighbor.x < 0 || neighbor.x >= columnCount || neighbor.y < 0 || neighbor.y >= rowCount)
			{
				continue;
			}

			wakeUp(neighbor, ParticleType::Empty);
		}
	}
}

void ParticleGrid::wakeUpDormantNeighbors(sf::Vector2i point, ParticleType selfType)
{
	for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			if (i == 0 && j == 0)
			{
				continue;
			}
			const sf::Vector2i neighbor = sf::Vector2i(point.x + i, point.y + j);
			if (neighbor.x < 0 || neighbor.x >= columnCount || neighbor.y < 0 || neighbor.y >= rowCount)
			{
				continue;
			}

			wakeUp(neighbor, selfType);
		}
	}
}

void ParticleGrid::wakeUp(sf::Vector2i gridPosition, ParticleType selfType)
{
	Particle& particle = at(gridPosition);
	expandDirtyRect(gridPosition);
	if (selfType == ParticleType::Smoke) return;
	if (particle.type != ParticleType::Empty && particle.state == ParticleState::Dormant)
	{
		if (getPseudoRandomDouble() < EngineConstants::INERTIALRESISTANCE)
		{
			particle.state = ParticleState::Suspended;
		}
	}
}

void ParticleGrid::transferMomentum(sf::Vector2i gridPosition) const
{
	Particle& particle = at(gridPosition);
	if (particle.velocity.y <= EngineConstants::GRAVITY)
	{
		return;
	}

	const bool canMoveLeft = isInBoundsAndEmpty(sf::Vector2i(gridPosition.x - 1, gridPosition.y));
	const bool canMoveRight = isInBoundsAndEmpty(sf::Vector2i(gridPosition.x + 1, gridPosition.y));

	const float gravitationalAcceleration = particle.velocity.y - EngineConstants::GRAVITY;
	const int horizontalTransfer = gravitationalAcceleration * EngineConstants::CONVERSIONFACTOR;

	if (canMoveLeft && canMoveRight)
	{
		if (bool randomLeft = getPseudoRandomInteger() % 2 == 0)
		{
			particle.velocity.x -= horizontalTransfer;
		}
		else
		{
			particle.velocity.x += horizontalTransfer;
		}
	}
	else if (canMoveLeft)
	{
		particle.velocity.x -= horizontalTransfer;
	}
	else if (canMoveRight)
	{
		particle.velocity.x += horizontalTransfer;
	}
}

void ParticleGrid::applyFriction(Particle& particle)
{
	if (particle.velocity.x != 0)
	{
		const int frictionDirection = particle.velocity.x > 0 ? -1 : 1;
		particle.velocity.x += frictionDirection * EngineConstants::FRICTION;
		if (particle.velocity.x < 0 == frictionDirection < 0) // if friction has reversed the direction, stop the particle.
			particle.velocity.x = 0;
	}
}

int ParticleGrid::getCardinalDirectionsAreOfTypeCount(sf::Vector2i position, ParticleType type) const
{
	int count = 0;
	for (sf::Vector2i direction : cardinalDirections)
	{
		const sf::Vector2i neighbor = position + direction;
		if (neighbor.x < 0 || neighbor.x >= columnCount || neighbor.y < 0 || neighbor.y >= rowCount)
		{
			continue;
		}

		if (at(neighbor).type == type)
		{
			count++;
		}
	}
	return count;
}

void ParticleGrid::debugDraw(sf::RenderWindow& window) const
{
	if (!drawDebugRectangles)
	{
		return;
	}

	for (const auto& chunk : chunks)
	{
		if (chunk.getCount() > 0)
		{
			sf::RectangleShape rectangle;
			rectangle.setSize(sf::Vector2f((chunk.end.x - chunk.start.x + 1) * particleSize, (chunk.end.y - chunk.start.y + 1) * particleSize));
			rectangle.setPosition(sf::Vector2f(chunk.start.x * particleSize, chunk.start.y * particleSize));
			rectangle.setFillColor(sf::Color::Transparent);
			rectangle.setOutlineColor(sf::Color::Red);
			rectangle.setOutlineThickness(1);
			window.draw(rectangle);
			drawDebugDirtyRectangle(window, chunk);
		}
	}
}

void ParticleGrid::drawDebugDirtyRectangle(sf::RenderWindow& window, const ParticleChunk& chunk) const
{
	const sf::Vector2i dimensions = chunk.dirtyRect.getDimensions();
	if (dimensions == sf::Vector2i(0, 0))
	{
		return;
	}

	sf::RectangleShape rectangle;
	rectangle.setSize(sf::Vector2f(dimensions.x * particleSize, dimensions.y * particleSize));
	const sf::Vector2i position = sf::Vector2i(chunk.dirtyRect.minPosition.x * particleSize, chunk.dirtyRect.minPosition.y * particleSize);
	rectangle.setPosition(sf::Vector2f(position));
	rectangle.setFillColor(sf::Color::Transparent);
	rectangle.setOutlineColor(sf::Color::Green);
	rectangle.setOutlineThickness(1);
	window.draw(rectangle);
}

void ParticleGrid::swapDirtyRects()
{
	for (auto& chunk : chunks)
	{
		chunk.swapDirtyRects();
	}
}

void ParticleGrid::expandDirtyRect(sf::Vector2i point)
{
	for (auto& chunk : chunks)
	{
		if (point.x >= chunk.start.x && point.x <= chunk.end.x && point.y >= chunk.start.y && point.y <= chunk.end.y)
		{
			chunk.expandDirtyRect(point);
			break;
		}
	}
}

void ParticleGrid::registerInChunk(sf::Vector2i point)
{
	if (at(point).type != ParticleType::Empty)
	{
		return;
	}

	for (auto& chunk : chunks)
	{
		if (point.x >= chunk.start.x && point.x <= chunk.end.x && point.y >= chunk.start.y && point.y <= chunk.end.y)
		{
			chunk.incrementCount();
			break;
		}
	}
}

void ParticleGrid::unregisterInChunk(sf::Vector2i point)
{
	if (at(point).type == ParticleType::Empty)
	{
		return;
	}

	for (auto& chunk : chunks)
	{
		if (point.x >= chunk.start.x && point.x <= chunk.end.x && point.y >= chunk.start.y && point.y <= chunk.end.y)
		{
			chunk.decrementCount();
			break;
		}
	}
}