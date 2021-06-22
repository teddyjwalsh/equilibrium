#ifndef _MAP_H_
#define _MAP_H_

#include <random>
#include <vector>
#include <algorithm>

#include "noise.h"

namespace equilibrium
{

class Map
{
    std::vector<std::vector<float>> _height_map;  // Landscape/terrain height
    std::vector<std::vector<float>> _lifted_matter_map;  // Eroded matter "floating" in buckets
    std::vector<std::vector<int>> _bucket_map;  // Current state of flowing liquid
    std::vector<std::vector<int>> _frozen_bucket_map;  // Saved state of flowing liquid
    std::vector<std::vector<int>> _evaporated_map;  // Water evaporated into the air
    
    std::default_random_engine _height_generator;
    std::default_random_engine _rain_generator;
    std::default_random_engine _evaporation_generator;
    std::uniform_real_distribution<double> _height_distribution;
    std::uniform_real_distribution<double> _evaporation_distribution;
    float _blob_height = 0.00001;
    float _evaporation_probability = 0.05;
    int _size_x;
    int _size_y;

    CombinedNoise _terrain_gen;
    
    struct ContactInfo
    {
        int x;
        int y;
        float height_delta;
        float contact_delta;
        float weight;
    };
    
    std::vector<ContactInfo> get_contacts(int x, int y)
    {
        std::vector<ContactInfo> contacts;
        // Only calculate contacts for neighbors that exist! Checking if we're on
        // a boundary basically
        if (x != _size_x - 1)
        {
            contacts.push_back({x+1, y, 0,0,1});
            if (y != 0) { contacts.push_back({x-1, y-1, 0, 0, 1.414}); }
            if (y != _size_y - 1) { contacts.push_back({x-1, y+1, 0, 0, 1.414}); }
        }
        if (x != 0)
        {
            contacts.push_back({x-1, y, 0,0,1});
            if (y != 0) { contacts.push_back({x+1, y-1, 0, 0, 1.414}); }
            if (y != _size_y - 1) { contacts.push_back({x+1, y+1, 0, 0, 1.414}); }
        }
        if (y != _size_y - 1) { contacts.push_back({x, y+1,0,0,2}); }
        if (y != 0) { contacts.push_back({x, y-1, 0,0,1}); }
        float start_height = _height_map[x][y] + _frozen_bucket_map[x][y]*_blob_height;
        for (auto& contact : contacts)
        {
            float rand_height_delta = _height_distribution(_height_generator);
            float contact_height = _height_map[contact.x][contact.y] + _frozen_bucket_map[contact.x][contact.y]*_blob_height;
            contact.height_delta = (start_height - contact_height)/contact.weight + rand_height_delta;
            contact.contact_delta = std::max(0.0f, std::min(_height_map[contact.x][contact.y] + rand_height_delta, start_height) - _height_map[x][y]);
        }
        std::sort(contacts.begin(), contacts.end(), [](const ContactInfo& a, const ContactInfo& b) { return a.height_delta < b.height_delta; });
        return contacts;
    }
    
    void iterate_cell(int x, int y)
    {
        const float primary_erosion_subtraction = 3*0.0005;
        const float contact_erosion_factor = 10*0.001;
        if (_frozen_bucket_map[x][y] > 0)
        {
            float max_transfer;
            auto contacts = get_contacts(x, y);
            float transfer_delta = contacts.back().height_delta;
            if (transfer_delta > 0)
            {
                int transfer_bucket_count = int(std::min(std::max(int(transfer_delta/_blob_height/2.0f), 1), _frozen_bucket_map[x][y]));
                float lifted_soil = transfer_delta*primary_erosion_subtraction;
                float lifted_transfer = _lifted_matter_map[contacts.back().x][contacts.back().y]*transfer_bucket_count*1.0/_bucket_map[x][y];
                _lifted_matter_map[contacts.back().x][contacts.back().y] -= lifted_transfer;
                lifted_soil += lifted_transfer;
                
                _bucket_map[x][y] -= transfer_bucket_count;
                _bucket_map[contacts.back().x][contacts.back().y] += transfer_bucket_count;
                _height_map[x][y] -= transfer_delta*primary_erosion_subtraction;
                for (auto& contact : contacts)
                {
                    _height_map[contact.x][contact.y] -= transfer_delta*contact.contact_delta*contact_erosion_factor;
                    lifted_soil += transfer_delta*contact.contact_delta*contact_erosion_factor;
                }
                _lifted_matter_map[contacts.back().x][contacts.back().y] += lifted_soil;
            }
        }
    }

    void freeze_buckets()
    {
        _frozen_bucket_map = _bucket_map;
    }

public:

    Map(int size_x, int size_y):
        _size_x(size_x),
        _size_y(size_y),
        _height_distribution(-0.000001, 0.000001),
        _bucket_map(size_x, std::vector<int>(size_y)),
        _height_map(size_x, std::vector<float>(size_y)),
        _lifted_matter_map(size_x, std::vector<float>(size_y))
    {
        _terrain_gen.add_noise(50, 1);
        _terrain_gen.add_noise(300, 1);
        for (int i = 0; i < _size_x; ++i)
        {
            for (int j = 0; j < _size_y; ++j)
            {
                _height_map[i][j] = _terrain_gen.get_point(i, j);
            }
        }
    }

    void iterate()
    {
        freeze_buckets();
        for (int i = 0; i < _size_x; ++i)
        {
            for (int j = 0; j < _size_y; ++j)
            {
                iterate_cell(i, j);
            }
        }
        freeze_buckets();
        for (int i = _size_x - 1; i >= 0; --i)
        {
            for (int j = _size_y - 1; j >= 0; --j)
            {
                iterate_cell(i, j);
            }
        }
        float drop_fraction = 0.001;
        for (int i = 0; i < _size_x; ++i)
        {
            for (int j = 0; j < _size_y; ++j)
            {
                float dropped_soil = drop_fraction*_lifted_matter_map[i][j];
                _lifted_matter_map[i][j] -= dropped_soil;
                _height_map[i][j] += dropped_soil;
            }
        }
    }
};

} // namespace equilibrium

#endif  // _MAP_H_
