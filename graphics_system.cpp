#include "graphics_system.h" 
std::string light_compute_shader = R"V0G0N( #version 460

struct QuadTreeNode
{
    uint children[4];
    vec3 loc;
	float max_height;
	float size;
    int is_object;
};


struct Ray
{
	vec3 loc;
	vec3 dir;
	float travel;
	vec3 color;
};

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;
layout(rgba32f, binding=1) uniform image2D tex_out;
layout(binding = 2, std430) buffer Nodes
{
	QuadTreeNode nodes[ ];
};

layout(binding = 3, std430) buffer HeightMap
{
	float height_map[ ];
};

layout(binding = 5, std430) buffer ColorMap
{
	vec4 color_map[ ];
};

uniform vec3 camera_loc;
uniform vec3 camera_look;
uniform float camera_width;
uniform float camera_height;
uniform vec3 camera_right;
uniform vec3 camera_up;
uniform int height_map_width;
uniform int height_map_height;
uniform float max_height;
uniform float time;
const float camera_f = 0.5;
const int queue_length = 50;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

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
    return tmax >= tmin;
}

float height_map_lookup(vec2 r)
{
    if (int(floor(r.x)) > height_map_width || int(floor(r.x)) < 0 || int(floor(r.y)) > height_map_height || int(floor(r.y)) < 0)
    {
        return -1.0;
    }
    return height_map[int(floor(r.y))*height_map_width + int(floor(r.x))];
}

vec3 color_map_lookup(vec2 r)
{
    if (int(floor(r.x)) > height_map_width || int(floor(r.x)) < 0 || int(floor(r.y)) > height_map_height || int(floor(r.y)) < 0)
    {
        return vec3(0);
    }
    return color_map[int(floor(r.y))*height_map_width + int(floor(r.x))].xyz;
}

vec3 height_map_lookup_normal(vec2 r)
{
    if (int(floor(r.x)) > height_map_width-1 || int(floor(r.x)) < 1 || int(floor(r.y)) > height_map_height-1 || int(floor(r.y)) < 1)
    {
        return vec3(0,0,1);
    }
    float h1 = height_map[int(floor(r.y))*height_map_width + int(floor(r.x-1))];
    float h2 = height_map[int(floor(r.y))*height_map_width + int(floor(r.x+1))];
    float h3 = height_map[int(floor(r.y-1))*height_map_width + int(floor(r.x))];
    float h4 = height_map[int(floor(r.y+1))*height_map_width + int(floor(r.x))];
    vec3 v1 = normalize(vec3(r.x+1,r.y,h2) - vec3(r.x-1,r.y,h1));
    vec3 v2 = normalize(vec3(r.x,r.y+1,h4) - vec3(r.x,r.y-1,h3));
    return normalize(cross(v1, v2));
}

bool ray_into_height_map_quadtree(vec3 origin, vec3 dir, uint root_node, inout float t, uint node_queue[queue_length], out vec3 norm, out vec3 intersect)
{

  //vec3 ix_queue[50];
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
  
  while (node_queue_front_pointer != node_queue_back_pointer)// && node_queue_back_pointer < 4)
  {
    root_node = node_queue[node_queue_front_pointer];
/*
    vec3 bmin = nodes[root_node].loc;
    vec3 bmax = nodes[root_node].loc + vec3(nodes[root_node].size, nodes[root_node].size, nodes[root_node].max_height);
    float tmin = 0; 
    float tmax = 0;
    t = 100000;
    hit = ray_intersect_aabb(origin, dir, bmin, bmax, tmin, tmax);
*/

    //if (hit)
    {
      //t = tmin;
      if (nodes[root_node].is_object > 0)
      {
        vec3 bmin = (nodes[root_node].loc);
        vec3 bmax = (nodes[root_node].loc) + vec3(nodes[root_node].size, nodes[root_node].size, nodes[root_node].max_height);
      	hit = ray_intersect_aabb(origin, dir, bmin, bmax, tmin, tmax);
        if (tmin > 0)
        {
	    t = tmin;
        
        intersect = origin + dir*t;
        norm = height_map_lookup_normal(intersect.xy);
        return true;
        }
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
                node_queue_back_pointer = (node_queue_back_pointer + 1) % queue_length;
            }
          }
        }
      }
    }
    node_queue_front_pointer = (node_queue_front_pointer + 1) % queue_length;
  }
  return false;
}

bool ray_into_height_map(vec3 origin, vec3 dir, inout float t, uint node_queue[queue_length], inout vec3 norm, out vec3 intersect)
{
    float tmin, tmax;
    bool hit = ray_intersect_aabb(origin, dir, vec3(0), vec3(height_map_width, height_map_height, max_height), tmin, tmax);
    if (!hit) { return false; }
    float inc = 3.0;
    t = 0;
    int count = 0;
    const float max_inc = 500;
    vec3 cur_loc = origin + dir*tmin;
    float end_t = tmax - tmin;
	float start_t = tmin;
    if (tmin < 0) { cur_loc = origin; end_t = tmax; start_t = t;}
    while (t < end_t)
    {
        if (count % 200 == 199) { inc += 1; }
        float cur_height = height_map_lookup(cur_loc.xy);
        if (cur_loc.z < cur_height)
        {
			
			//ray_intersect_aabb(cur_loc, dir, vec3(floor(cur_loc.xy), 0), vec3(ceil(cur_loc.xy), cur_height), tmin, tmax);
            norm = height_map_lookup_normal(cur_loc.xy);
            intersect = cur_loc;
            intersect.z = cur_height + 0.001;
            return true;
        }
        t += inc;
        cur_loc += dir*inc;
        count += 1;
    }
    return false;
}

vec3 reflect(vec3 d, vec3 n)
{
    return d - 2*(dot(d,n))*n;
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
    uint node_queue[queue_length];
    vec3 light_dir = normalize(vec3(cos(time*0.09), 0, sin(time*0.09)));
	const ivec2 dims = imageSize(tex_out);
	const ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);

	vec3 ray = gen_ray(pixel_coords.x*1.0/dims.x, pixel_coords.y*1.0/dims.y);
    float t; 
    float tmax;
    bool hit = false;
    
    bool result;
    vec3 norm;
    vec3 hit_loc;
    result = ray_into_height_map(camera_loc, ray, t, node_queue, norm, hit_loc);
    //result = ray_into_height_map_quadtree(camera_loc, ray, 0, t, node_queue, norm, hit_loc);
	Ray init_ray;
	init_ray.loc = camera_loc,
	init_ray.dir = ray;
	init_ray.color = vec3(1);
    vec3 init_norm = norm;
    vec3 init_hit = hit_loc;
    int num_bounces = 5;
    vec4 light_sum = vec4(0,0,0,1);
	vec3 reflected = reflect(norm, ray);
	int count_bounces = 0;
    if (result)
    {
        init_ray.travel = 0;//length(hit_loc - init_ray.loc);
        float init_dist = length(hit_loc - init_ray.loc);
        num_bounces = 3;//int(clamp(80.0/init_dist, 2.0,3.0));
        int num_avgs = 3;
        //num_avgs = 2;//int(clamp(50.0/init_dist, 1.0,5.0));
        for (int j = 1; j < num_avgs; ++j)
        {
            Ray new_ray = init_ray;
            norm = init_norm;
            hit_loc = init_hit;
            for (int i = 1; i < num_bounces+1; ++i)
            {
                vec3 hit_2_light = -light_dir;//normalize(light.loc - hit_loc);
                reflected = reflect(norm, new_ray.dir);
                Ray light_ray;
                light_ray.loc = hit_loc;
                light_ray.dir = hit_2_light;
                new_ray.loc = hit_loc;
                //new_ray.dir = normalize((reflected*rand(pixel_coords.yx*100+j*10+ i*100)/3 - norm*rand(pixel_coords + j*100 + i*10000) )/2.0);
				vec3 perp1 = normalize(cross(norm, norm + vec3(1,0,0)));
				vec3 perp2 = normalize(cross(perp1, norm));
				new_ray.dir = normalize(norm*rand(pixel_coords*(i*num_avgs + j)) + perp1*(rand(-pixel_coords.yx*(i*num_avgs + j))*2 - 1) + perp2*(rand(pixel_coords*(j*num_bounces + i))*2 - 1));
                new_ray.dir = normalize(norm + 5*new_ray.dir);
				//if (dot(rand_vec, norm) < 0) { rand_vec = -rand_vec; }
				
                //new_ray.dir = normalize(new_ray.dir + 2.0*rand_vec);
				float dotted = 1*max(0,dot(hit_2_light, norm));
				if (num_bounces > 0)
				{
					new_ray.travel += length(hit_loc - new_ray.loc);
				}
                
				result = ray_into_height_map(new_ray.loc, hit_2_light, t, node_queue, norm, hit_loc);
                //result = ray_into_height_map_quadtree(new_ray.loc, hit_2_light, 0, t, node_queue, norm, hit_loc);
                new_ray.color *= (color_map_lookup(hit_loc.xy));
				float travel_factor = 1/pow(new_ray.travel + 1,2);
				if (!result)
				{
					
					light_sum += vec4(new_ray.color*travel_factor*dotted,0.0);
				}
				
				result = ray_into_height_map(new_ray.loc, new_ray.dir, t, node_queue, norm, hit_loc);
                //result = ray_into_height_map_quadtree(new_ray.loc, new_ray.dir, 0, t, node_queue, norm, hit_loc);
				if (!result)
				{
					light_sum += vec4(new_ray.color,1.0)*vec4(0.1,0.1,0.2,1.0)*travel_factor;
					break;
				}
				
				count_bounces += 1;
            }
        }
        imageStore(tex_out, pixel_coords, (2.0*light_sum/num_avgs));
    }
    else
    {
        imageStore(tex_out, pixel_coords, vec4(clamp(1 - 1.7*pow(pixel_coords.y,2)/500000.0,0.7,0.95)
                                    ,clamp(1 - pow(pixel_coords.y,2)/500000.0,0.8,0.98),
                                    1.0,1));
    }
	//imageStore(tex_out, pixel_coords, vec4(norm, 1.0));
}

)V0G0N";
