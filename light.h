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
