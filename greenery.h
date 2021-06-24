Growth system:
  growth_components = get_components<GrowthComp>();
  atmosphere_comp = get_component<AtmosphereComp>();
  for growth_comp in growth_components
    pos_comp = get_sibling<pos>(growth_comp);
    temp = atmosphere_comp.temp_at_loc(pos_comp.loc)
    temp_factor = calc_temp_factor(temp, growth_comp.ideal_temp)
    light = atmosphere_comp.temp_at_loc(pos_comp.loc)
    light_factor = calc_temp_factor(light, growth_comp.ideal_light)
    co2_factor = 1.0;
    if growth_comp.needs_co2
      inhaler_comp = get_sibling<inhaler>(growth_comp)
      co2_taken = min(growth_factor.max_co2, inhaler.co2)
      inhaler.co2 -= co2_taken
      co2_factor = calc_co2_factor(co2_taken, growth_comp.max_co2)
    growth = light_factor*co2_factor*temp_factor
    growth_comp.size += growth
    
    
Seeding system:
  seeding_components = get_components<SeedingComp>();
  seed_bins = get_component<SeedBinsComponent>();
  for seed_comp in seeding_components
    growth_comp = get_sibling<GrowthComp>(seed_comp)
    if growth_comp.size > seed_comp.min_seeding_size:
      if current_time - seed_comp.last_seed_time > seed_comp.seeding_period
        seed_comp.last_seed_time = current_time
        plant_random_radius(seed_bins, seed_comp)
        
Lung system:
  atmosphere_component = get_component<AtmosphereComp>();
  lung_components = get_components<LungComp>();
  for lung in lung_components:
    if lung.inhales_oxygen:
      lung.oxygen += atmosphere_component.oxygen*lung.oxygen_inhale_rate
    if lung.exhales_oxygen:
      lung.oxygen -= min(lung.oxygen, atmosphere_component.global_oxygen*lung.oxygen_exhale_rate)
    // same for co2
      
