#pragma once
#include <random>

namespace rnd
{
	inline int rand_num(int min, int max)
	{
		std::random_device seeder;
		std::mt19937 engine(seeder());
		std::uniform_int_distribution<int> dist(min, max);

		return dist(engine);
	}

	inline double rand_dec(double min, double max)
	{
		min *= 10000;
		max *= 10000;

		int i{ rand_num(min, max) };
		double r = static_cast<double>(i) / 10000;

		return r;
	}
}