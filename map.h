#ifndef _MAP_H_
#define _MAP_H_

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
            if (y != 0) { contacts.push_Back({x-1, y-1, 0, 0, 1.414}); }
            if (y != _size_y - 1) { contacts.push_Back({x-1, y+1, 0, 0, 1.414}); }
        }
        if (x != 0)
        {
            contacts.push_back({x-1, y, 0,0,1});
            if (y != 0) { contacts.push_Back({x+1, y-1, 0, 0, 1.414}); }
            if (y != _size_y - 1) { contacts.push_Back({x+1, y+1, 0, 0, 1.414}); }
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
    }
    
};

} // namespace equilibrium

#endif  // _MAP_H_
