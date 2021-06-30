#include "quadtree.h"

glm::ivec4 offset_to_sorted_children(const glm::vec3& offset)
{
	if (offset.x < 0 && offset.y < 0)
	{
		return glm::ivec4(0, 1, 2, 3);
	}
	if (offset.x > 0 && offset.y < 0)
	{
		return glm::ivec4(1, 3, 0, 2);
	}
	if (offset.x > 0 && offset.y > 0)
	{
		return glm::ivec4(3, 1, 2, 0);
	}
	if (offset.x < 0 && offset.y > 0)
	{
		return glm::ivec4(2, 0, 3, 1);
	}
	return glm::ivec4(0, 1, 2, 3);
}

bool ray_intersect_aabb(const glm::vec3& origin, const glm::vec3& dir, const glm::vec3& bmin, const glm::vec3& bmax, float& tmin, float& tmax)
{
	static float tx1, tx2, ty1, ty2, tz1, tz2;
	tx1 = (bmin.x - origin.x) / dir.x;
	tx2 = (bmax.x - origin.x) / dir.x;

	tmin = std::min(tx1, tx2);
	tmax = std::max(tx1, tx2);

	ty1 = (bmin.y - origin.y) / dir.y;
	ty2 = (bmax.y - origin.y) / dir.y;

	tmin = std::max(tmin, std::min(ty1, ty2));
	tmax = std::min(tmax, std::max(ty1, ty2));

	tz1 = (bmin.z - origin.z) / dir.z;
	tz2 = (bmax.z - origin.z) / dir.z;

	tmin = std::max(tmin, std::min(tz1, tz2));
	tmax = std::min(tmax, std::max(tz1, tz2));

	//bool hit = tmax >= tmin;
	return tmax >= tmin;
}