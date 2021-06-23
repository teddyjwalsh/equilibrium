std::tuple<bool, fvec3> ray_into_height_map_quadtree(const glm::vec3& origin, const glm::vec3& direction, quadtree::QuadTreeNode<HMTreeData>* root)
{
  auto cur_box = get_box_defined_by_node_and_height(root);
  auto& [hit, tmin, tmax] = ray_intersect_aabb(cur_box, origin, direction);
  if (hit)
  {
    fvec3 intersect = (origin + direction*tmin);
    if (root->data.is_object)
    {
      return std::make_tuple(hit, intersect);
    }
    else
    {
      auto sorted_children = sort_children_by_proximity(cur_Box, origin);
      for (auto& [dist, child_index] : sorted_children)
      {
        if (root->children[child_index])
        {
          [hit, intersect] = ray_into_height_map_quadtree(origin, direction, root->children[child_index]);
          if (hit)
          {
            return std::make_tuple(hit, intersect);
          }
        }
      }
    }
  }
  return std::make_tuple(false, fvec3(0));
}

bool ray_into_height_map_quadtree(vec3 origin, vec3 dir, int in_node, out float t)
{
  int node_queue[100];
  int node_queue_back_pointer = 0;
  int node_queue_front_pointer = 0;
  node_queue[node_queue_back_pointer] = root_node;
  node_queue_pointer += 1;
  while (node_queue_front_pointer != node_queue_back_pointer && node_queue_back_pointer < 100)
  {
    int root_node = nodes[node_queue_front_pointer];
    bmin = nodes[root_node].loc;
    bmax = nodes[root_node].loc + vec3(nodes[root_node].size, nodes[root_node].size, nodes[root_node].data.max_height);
    float tmin, tmax;
    bool hit = ray_intersect_abb(origin, dir, bmin, bmax, tmin, tmax);
    if (hit)
    {
      t = tmin;
      if (nodes[root_node].data.is_object)
      {
        return true;
      }
      else
      {
        for (int i = 0; i < 4; ++i)
        {
          int child_node = nodes[root_node].children[i];
          if (child_node > 0)
          {
            node_queue[node_queue_back_pointer] = child_node;
            node_queue_back_pointer += 1;
          }
        }
      }
    }
    node_queue_front_pointer += 1;
  }
}
