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
      
      
