#include "graphics_system.h"

std::string light_compute_shader = R"V0G0N(

#version 460

struct QuadTreeNode
{
    uint children[4];
    vec3 loc;
	float max_height;
	float size;
    int is_object;
};

layout(local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(rgba32f, binding=1) uniform image2D tex_out;
layout(binding = 2, std430) buffer Nodes
{
	QuadTreeNode nodes[ ];
};

uniform vec3 camera_loc;
uniform vec3 camera_look;
uniform float camera_width;
uniform float camera_height;
uniform vec3 camera_right;
uniform vec3 camera_up;
const float camera_f = 0.5;

ivec4 offset_to_sorted_children(vec3 offset)
{
    if (offset.x < 0 && offset.y < 0)
    {
        return ivec4(0,1,2,3);
    }
    if (offset.x > 0 && offset.y < 0)
    {
        return ivec4(1,3,0,2);
    }
    if (offset.x > 0 && offset.y > 0)
    {
        return ivec4(3,1,2,0);
    }
    if (offset.x < 0 && offset.y > 0)
    {
        return ivec4(2,0,3,1);
    }
    return ivec4(0,1,2,3);
}

bool ray_intersect_aabb(vec3 origin, vec3 dir, vec3 bmin, vec3 bmax, inout float tmin, inout float tmax) 
{
    
    float tx1 = (bmin.x - origin.x)/dir.x;
    float tx2 = (bmax.x - origin.x)/dir.x;
    
    tmin = min(tx1, tx2);
    tmax = max(tx1, tx2);
    
    float ty1 = (bmin.y - origin.y)/dir.y;
    float ty2 = (bmax.y - origin.y)/dir.y;

    tmin = max(tmin, min(ty1, ty2));
    tmax = min(tmax, max(ty1, ty2));
    
    float tz1 = (bmin.z - origin.z)/dir.z;
    float tz2 = (bmax.z - origin.z)/dir.z;

    tmin = max(tmin, min(tz1, tz2));
    tmax = min(tmax, max(tz1, tz2));

    //bool hit = tmax >= tmin;
    return tmax >= tmin && tmin > 0;
}

bool ray_into_height_map_quadtree(vec3 origin, vec3 dir, uint root_node, inout float t)
{
  uint node_queue[50];
  vec3 ix_queue[50];
  int node_queue_front_pointer = 0;
  int node_queue_back_pointer = 0;
  node_queue[node_queue_back_pointer] = root_node;
  //ix_queue[node_queue_back_pointer] = origin;
  node_queue_back_pointer += 1;
  bool hit = false;
  float tmin = 0; 
  float tmax = 0;
  int noder = 1;
  //hit = ray_intersect_aabb(origin, dir, nodes[noder].loc, nodes[noder].loc + vec3(nodes[noder].size, nodes[noder].size, nodes[noder].max_height), tmin, tmax);
  //return hit;
  
  while (node_queue_front_pointer < node_queue_back_pointer && node_queue_back_pointer < 50)
  {
    root_node = node_queue[node_queue_front_pointer];
/*
    vec3 bmin = nodes[root_node].loc;
    vec3 bmax = nodes[root_node].loc + vec3(nodes[root_node].size, nodes[root_node].size, nodes[root_node].max_height);
    float tmin = 0; 
    float tmax = 0;
    hit = ray_intersect_aabb(origin, dir, bmin, bmax, tmin, tmax);
*/

    //if (hit)
    {
      //t = tmin;
      if (nodes[root_node].is_object > 0)
      {
        return true;
      }
      else
      {
        ivec4 sorted = offset_to_sorted_children(vec3(origin - (nodes[root_node].loc + nodes[root_node].size/2.0)));
        for (int i = 0; i < 4; ++i)
        {
          uint child_node = nodes[root_node].children[sorted[i]];
          if (child_node > 0)
          {
            vec3 bmin = (nodes[child_node].loc);
            vec3 bmax = (nodes[child_node].loc) + vec3(nodes[child_node].size, nodes[child_node].size, nodes[child_node].max_height);
            hit = ray_intersect_aabb(origin, dir, bmin, bmax, tmin, tmax);
            if (hit)
            {
                //ix_queue[node_queue_back_pointer] = ix_queue[node_queue_front_pointer] + dir*tmin*0.99;
                node_queue[node_queue_back_pointer] = child_node;
                node_queue_back_pointer += 1;
            }
          }
        }
      }
    }
    node_queue_front_pointer += 1;
  }
  return false;
}

vec3 gen_ray(float x, float y)
{
	vec3 forward = camera_look*camera_f;
	vec3 bottom_left = forward - camera_right*camera_width/2.0 - camera_up*camera_height/2.0;
	vec3 out_dir = bottom_left + x*camera_right*camera_width + y*camera_up*camera_height;
	return out_dir;
}

void main()
{
    
    vec3 light_dir = normalize(vec3(0,-1,-1));
	const ivec2 dims = imageSize(tex_out);
	const ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	vec3 ray = gen_ray(pixel_coords.x*1.0/dims.x, pixel_coords.y*1.0/dims.y);
    float t; 
    float tmax;
    bool hit = false;
    hit = ray_into_height_map_quadtree(camera_loc, ray, 0, t);
    float height_map_val = 0;
    if (hit)
    {
        vec3 intersect = camera_loc + ray*t*0.99;
        //intersect.z += 0.1;
        //hit = ray_into_height_map_quadtree(intersect, -light_dir, 0, t);
        //if (!hit)
        {
            height_map_val = 1.0;
        }
    }

	//float height_map_val = 0.5;//height_map[pixel_coords.y*dims.x + pixel_coords.x];
	imageStore(tex_out, pixel_coords, height_map_val*vec4(1));
}

)V0G0N";