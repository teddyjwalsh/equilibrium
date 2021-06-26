#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE

#include <iostream>

#include "spdlog/spdlog.h"

#include "physics_component.h"
#include "physics_system.h"
#include "time_system.h"
#include "component_manager.h"
#include "pickup_system.h"
#include "input_system.h"
#include "time_component.h"
#include "client_component.h"

#include "context.h"
#include "camera.h"
#include "renderable_mesh.h"
#include "map.h"
#include "compute_shader.h"
#include "camera_component.h"
#include "graphics_system.h"
#include "ray_camera_component.h"
#include "eq_input_system.h"

int main()
{
    auto cm = std::make_shared<ComponentManager>();
    cm->add_array<CompRayCamera>();
    cm->add_array<CompGraphics>();
    cm->add_array<CompRenderableMesh>();
    cm->add_array<CompPosition>();
    cm->add_array<CompPickupper>();
    cm->add_array<CompKeyState>();
    auto graphics_system = std::make_shared<GraphicsSystem>();
    cm->add_system(graphics_system);
    cm->add_component<CompGraphics>();
    auto is = std::make_shared<SysInputEq>();
    EntityId player = cm->add_entity({
               uint32_t(type_id<CompPosition>),
               uint32_t(type_id<CompPickupper>),
               uint32_t(type_id<CompRayCamera>),
               uint32_t(type_id<CompKeyState>)});
    cm->add_system(is);
    cm->init_update();
    while(1)
    {
        cm->update();
    }
    
}
