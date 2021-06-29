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
#include "terrain_system.h"
#include "gl_graphics_system.h"
#include "time_component.h"
#include "height_map_component.h"


int main()
{
    auto cm = std::make_shared<ComponentManager>();
    cm->add_array<CompCamera>();
    cm->add_array<CompGraphics>();
    cm->add_array<CompRenderableMesh>();
    cm->add_array<CompPosition>();
    cm->add_array<CompPickupper>();
    cm->add_array<CompKeyState>();
    cm->add_array<CompTime>();
    cm->add_array<CompHeightMap>();
    auto time_system = std::make_shared<SysTime>();
    auto terrain_system = std::make_shared<SysTerrain>();
    auto graphics_system = std::make_shared<SysGLGraphics>();
    cm->add_system(graphics_system);
    cm->add_system(time_system);
    cm->add_system(terrain_system);
    cm->add_component<CompGraphics>();
    cm->add_component<CompTime>();

    auto is = std::make_shared<SysInputEq>();
    EntityId player = cm->add_entity({
               uint32_t(type_id<CompPosition>),
               uint32_t(type_id<CompPickupper>),
               uint32_t(type_id<CompCamera>),
               uint32_t(type_id<CompKeyState>)
        });

    EntityId envioronment = cm->add_entity({
           uint32_t(type_id<CompHeightMap>),
           uint32_t(type_id<CompRenderableMesh>),
        });

    cm->add_system(is);
    cm->add_system(time_system);
    cm->init_update();
    while(1)
    {
        cm->update();
    }
    
}
