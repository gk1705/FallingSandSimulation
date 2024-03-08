#include "ParticleSimulator.h"
#include "FireParticleSystem.h"
#include "SmokeParticleSystem.h"
#include "WaterParticleSystem.h"
#include "WoodParticleSystem.h"

ParticleSimulator::ParticleSimulator(ParticleGrid& particleGrid): particleGrid(particleGrid), threadPool(std::thread::hardware_concurrency())
{
	particleSystems[ParticleType::Sand] = new SandParticleSystem(particleGrid);
	particleSystems[ParticleType::Water] = new WaterParticleSystem(particleGrid);
	particleSystems[ParticleType::Smoke] = new SmokeParticleSystem(particleGrid);
	particleSystems[ParticleType::Wood] = new WoodParticleSystem(particleGrid);
	particleSystems[ParticleType::Fire] = new FireParticleSystem(particleGrid);
}

ParticleSimulator::~ParticleSimulator()
{
	for (const auto& pair : particleSystems)
	{
		delete pair.second;
	}
}

void ParticleSimulator::toggleSimulation()
{
	simulate = !simulate;
}

void ParticleSimulator::simulateParticles()
{
	if (!simulate)
	{
		return;
	}

	gatherPossibleMoves();
	sortPossibleMovesByDestination();
	const std::vector<std::pair<int, int>> ranges = getArrayThreadRanges();
	processMovesWithRandomContender(ranges);
	processParticles();
	computeDensitySwap();

	particleGrid.swapDirtyRects();
	possibleMoves.resetCurrentIndex();
}

bool ParticleSimulator::isValidParticle(sf::Vector2i gridPosition) const
{
	const Particle particle = particleGrid.at(gridPosition);
	if (particle.type == ParticleType::Empty) return false;
	if (!particleSystems.contains(particle.type)) return false;
	return true;
}

int ParticleSimulator::getRandomNumber(int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());

	std::uniform_int_distribution<> distribution(0, max);

	return distribution(gen);
}

void ParticleSimulator::gatherPossibleMoves()
{
	std::vector<std::future<void>> futures;
	for (const auto& chunk : particleGrid.chunks)
	{
		if (chunk.getCount() == 0) continue;
		const DirtyRect dirtyRect = chunk.dirtyRect;
		if (dirtyRect.getDimensions() == sf::Vector2i(0, 0)) continue;
		auto future = threadPool.enqueue([this, dirtyRect]
		{
			for (int i = dirtyRect.minPosition.y; i <= dirtyRect.maxPosition.y - 1; i++)
			{
				for (int j = dirtyRect.minPosition.x; j <= dirtyRect.maxPosition.x - 1; j++)
				{
					if (!isValidParticle(sf::Vector2i(j, i))) continue;
					sf::Vector2i move = particleSystems[particleGrid.at(sf::Vector2i(j, i)).type]->gatherMove(sf::Vector2i(j, i));
					if (move != sf::Vector2i(j, i))
					{
						particleGrid.expandDirtyRect(sf::Vector2i(j, i));
						possibleMoves.add(std::make_pair(move, sf::Vector2i(j, i)));
					}
				}
			}
		});
		futures.push_back(std::move(future));
	}

	for (auto& future : futures)
	{
		future.get();
	}
}

void ParticleSimulator::sortPossibleMovesByDestination()
{
	possibleMoves.sort([](const std::pair<sf::Vector2i, sf::Vector2i>& a, const std::pair<sf::Vector2i, sf::Vector2i>& b)
	{
		if (a.first.x == b.first.x)
		{
			return a.first.y < b.first.y;
		}
		return a.first.x < b.first.x;
	});
}

std::vector<std::pair<int, int>> ParticleSimulator::getArrayThreadRanges()
{
	std::vector<std::pair<int, int>> ranges;
	constexpr int minRangeSize = 12;

	const int currentIndex = possibleMoves.getCurrentIndex();
	const int rangeSize = currentIndex / threadPool.numThreads < minRangeSize ? minRangeSize : currentIndex / threadPool.numThreads;

	int start = 0;
	int end = rangeSize - 1;
	int currentRangeIndex = 0;
	while (end < currentIndex)
	{
		std::pair<int, int> range;
		range.first = start;

		while (start > 0 && possibleMoves[start].first == possibleMoves[start - 1].first)
		{
			start--;
			ranges.at(currentIndex - 1).second--;
		}
		while (end < currentIndex && possibleMoves[end].first == possibleMoves[end + 1].first)
		{
			end++;
		}

		range.second = end;
		ranges.emplace_back(range);
		currentRangeIndex++;

		start = end + 1;
		end = start + rangeSize - 1;
	}
	if (start < currentIndex - 1)
	{
		ranges.emplace_back(start, currentIndex - 1);
	}

	return ranges;
}

void ParticleSimulator::processMovesWithRandomContender(const std::vector<std::pair<int, int>>& ranges)
{
	std::vector<std::future<void>> futures;
	for (auto range : ranges)
	{
		auto future = threadPool.enqueue([this, range]
		{
			for (int i = range.first; i <= range.second; i++)
			{
				int contenders = 1;
				sf::Vector2i currentDestination = possibleMoves[i].first;
				while (i + contenders < possibleMoves.getCurrentIndex() && currentDestination == possibleMoves[i + contenders].first)
				{
					contenders++;
				}

				const int chosenContender = getPseudoRandomInteger() % contenders;
				//const int chosenContender = getRandomNumber(contenders - 1); optionally
				const sf::Vector2i currentSource = possibleMoves[i + chosenContender].second;
				if (!isValidParticle(currentSource)) continue; // TODO: debug, there still is a rare case where an empty particle gets processed
				particleSystems[particleGrid.at(currentSource).type]->computeMove(currentSource, currentDestination);
				i += contenders - 1;
			}
		});
		futures.push_back(std::move(future));
	}

	for (auto& future : futures)
	{
		future.get();
	}
}

void ParticleSimulator::processMovesWithRandomContender()
{
	for (int i = 0; i < possibleMoves.getCurrentIndex(); i++)
	{
		int contenders = 1;
		sf::Vector2i currentDestination = possibleMoves[i].first;
		while (i + contenders < possibleMoves.getCurrentIndex() && currentDestination == possibleMoves[i + contenders].first)
		{
			contenders++;
		}

		const int chosenContender = getPseudoRandomInteger() % contenders;
		//const int chosenContender = getRandomNumber(contenders - 1); optionally
		const sf::Vector2i currentSource = possibleMoves[i + chosenContender].second;
		particleSystems[particleGrid.at(currentSource).type]->computeMove(currentSource, currentDestination);
		i += contenders - 1;
	}
}

void ParticleSimulator::computeDensitySwap() const
{
	for (int i = particleGrid.chunks.size() - 1; i >= 0; i--)
	{
		const auto& chunk = particleGrid.chunks[i];
		if (chunk.getCount() == 0) continue;
		const DirtyRect& dirtyRect = chunk.dirtyRect;
		if (dirtyRect.getDimensions() == sf::Vector2i(0, 0)) continue;
		for (int i = dirtyRect.maxPosition.y - 1; i >= dirtyRect.minPosition.y; i--)
		{
			if (i == particleGrid.rowCount - 1) continue;
			for (int j = dirtyRect.minPosition.x; j <= dirtyRect.maxPosition.x - 1; j++)
			{
				const Particle particle = particleGrid.at(sf::Vector2i(j, i));
				const Particle belowParticle = particleGrid.at(sf::Vector2i(j, i + 1));
				if (particle.type == ParticleType::Empty || particle.locomotionType == ParticleLocomotionType::Static || belowParticle.locomotionType == ParticleLocomotionType::Static) continue;
				if (particle.density > belowParticle.density)
				{
					particleGrid.swapParticles(sf::Vector2i(j, i), sf::Vector2i(j, i + 1));
				}
			}
		}
	}
}

void ParticleSimulator::processParticles()
{
	std::vector<std::future<void>> futures;
	for (const auto& chunk : particleGrid.chunks)
	{
		if (chunk.getCount() == 0) continue;
		const DirtyRect& dirtyRect = chunk.dirtyRect;
		if (dirtyRect.getDimensions() == sf::Vector2i(0, 0)) continue;
		auto future = threadPool.enqueue([this, dirtyRect]
		{
			for (int i = dirtyRect.minPosition.y; i <= dirtyRect.maxPosition.y - 1; i++)
			{
				for (int j = dirtyRect.minPosition.x; j <= dirtyRect.maxPosition.x - 1; j++)
				{
					if (!isValidParticle(sf::Vector2i(j, i))) continue;
					const Particle& particle = particleGrid.at(sf::Vector2i(j, i));
					bool isEmpty = particle.type == ParticleType::Empty;
					if (particleSystems[particleGrid.at(sf::Vector2i(j, i)).type]->simulate(sf::Vector2i(j, i)))
					{
						particleGrid.expandDirtyRect(sf::Vector2i(j, i));
					}
				}
			}
		});
		futures.push_back(std::move(future));
	}

	for (auto& future : futures)
	{
		future.get();
	}
}
