#pragma once
#include <random> 

class Random
{
public:
	std::random_device rd;
	std::mt19937 mt;

	Random() : rd(), mt(rd()) {  }
	Random(int seed) : rd(), mt(rd()) { mt.seed(static_cast<std::mt19937::result_type>(seed)); }
	virtual ~Random() {}

	template<typename T>
	T range(const T& min, const T& max)
	{
		static_assert(std::is_arithmetic<T>::value, "type of range must be numeric!");
		std::uniform_int_distribution<T> dist(min, max);
		return dist(mt);
	}

	bool percentage(int comp)
	{
		return (range(0, 100) < comp);
	}

	//weight, value
	template<typename T>
	T biased(std::vector<std::pair<int, T>> vs)
	{
		std::vector<T> bias;
		for (auto v : vs)
			for (int i = 0; i < v.first; i++)
				bias.push_back(v.second);
		return bias[range(0, bias.size() - 1)];
	}
};