#ifndef _NOISE_H_
#define _NOISE_H_

#include <vector>
#include <tuple>

#include "FastNoiseLite.h"

namespace equilibrium
{

class CombinedNoise 
{
    std::vector<std::tuple<FastNoiseLite*, float, float>> _noises;
public:
    void add_noise(float frequency, float scale)
    {
        FastNoiseLite* temp_noise_pointer = new FastNoiseLite(FastNoiseLite::NoiseType_OpenSimplex2);
        _noises.push_back(std::make_tuple(temp_noise_pointer, frequency, scale));
    }
    
    float get_point(float x, float y)
    {
        float out_val = 0;
        for (auto& [noise, f, s] : _noises)
        {
            out_val += (noise->GetNoise(x*f, y*f)+1)*s/2.0;
        }
        return out_val;
    }
};

} // namespace equilibrium

#endif  // _NOISE_H_
