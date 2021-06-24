
Movement system:
  terrain_component = get_terrain_component()
  moveable_components = get_moveable_components()
  for moveable in moveable_components:
    pos_component = get_sibling<pos>(moveable)
    gradients = sample_gradient_across_request(moveable.request)
    terrain_types = sample_terrain_type_across_request(moveable.request)
    for grad, ter_type in gradients, terrain_types
      if grad > moveable.grad_threshold or ter_type not in moveable.traversable_terrain_types:
        break
      else
        pos_component.last_location = pos_component.location
        pos_component.location = grad.location
        
Oxygen use system:
  oxygen_use_components = get_oxygen_tank_components()
  for ouc in oxygen_use_components
    pos_component = get_sibling<pos>(ouc)
    if ouc.use_with_movement and pos_component:
      ouc.oxygen_level -= get_oxygen_use_from_location_change(pos_component.last_location, pos_component.loction)
      // What are consequences for oxygen depletion?
      // - Pass out, respawn at last used oxygen station
      // - Die? What would this mean?
      // - Movement becomes very slow. Seems annoying.
        
 
Inputs: ActionComponent, PositionComponent, InventoryComponent
Outputs: ActionComponent, ObjectPlacementMapComponent, InventoryComponent
Action system:
  action_components = get_action_components()
  for action_comp in action_components
    current_action = action_comp.queue.front()
    pos_comp = get_sibling<pos>(action_comp)
    switch current_action.type
      case MOVEMENT
        moveable_comp = get_sibling<moveable>(action_comp)
        moveable_comp.request = current_action.location
      case PLACE_OBJECT
        if abs(pos_comp.location - current_action.location) < action_comp.radii[PLACE_OBJECT]
          inventory = get_sibling<inventory>(action_comp)
          if current_action.object.type in inventory:
            object_placement_map = get_component<object_placement_comp>()
            object_placement_map.place(inventory, current_action.object.type)
          action_comp.queue.pop_front()
        else
          moveable_comp = get_sibling<moveable>(action_comp)
          moveable_comp.request = current_action.location
      case TERRAIN_MOD
      case USE_ENVIRONMENT_OBJECT
      case USE_INVENTORY_OBJECT
      case DIG
        
      
      
Input system:
  keystate_comp = get_keystate_component()
  camera_component = get_camera_component()
  action_component = get_sibling<action_component>(keystate_comp)
  if keystate_comp.right_mouse_press:
    object_placement_map = get_component<object_placement_map>()
    object = object_placement_map.get_object_at_coordinate(camera.screen_to_world_coords(keystate_comp.mouse_loc))
    if (object)
      new_action.type = USE_ENVIRONMENT_OBJECT
      new_action.object = object
      action_component.queue_action(new_action)
    else
      new_action.type = MOVEMENT
      new_action.location = camera.screen_to_world_coords(keystate_comp.mouse_loc)
  elif keystate_comp.left_mouse_press:
    if action_component.action_waiting_on_mouse:
      // switch based on action here
  
Use system:
  user_components = get_components<UserComp>()
  for user  in user_components
    if user.use_event:
      // Questionable due to usee component having functionality
      user.use_event.usee.use(user)
    
        
