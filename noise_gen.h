#pragma once

#include "FastNoiseLite.h"

#include <vector>

class CombinedNoise
{
	std::vector<std::tuple<FastNoiseLite*, float, float>> noises;
	int seed;
public:
	void add_noise(float frequency, float scale)
	{
		auto tmp_noise = new FastNoiseLite(seed++);
		tmp_noise->SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
		noises.push_back(std::make_tuple(tmp_noise, frequency, scale));
	}

	float get_point(float x, float y)
	{
		float out_val = 0.0;
		//FastNoiseLite* n;
		//float f, s;
		for (auto& tup : noises)
		{
			auto& [n, f, s] = tup;
			out_val += s * (n->GetNoise(x * f, y * f)+1)/2.0;
		}
		return out_val;
	}
};