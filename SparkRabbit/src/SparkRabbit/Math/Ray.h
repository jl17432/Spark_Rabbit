#pragma once
#include "BoundingBox.h"

namespace SparkRabbit{

	struct  Ray
	{
		glm::vec3 Origin, Direction;
		bool IntersectsBoundingBox(const BoundingBox& box, float& t) const
		{

			//Axis-Aligned Bounding Box
			float tmin = (box.Min.x - Origin.x) / Direction.x;
			float tmax = (box.Max.x - Origin.x) / Direction.x;
			if (tmin > tmax) std::swap(tmin, tmax);
			float tymin = (box.Min.y - Origin.y) / Direction.y;
			float tymax = (box.Max.y - Origin.y) / Direction.y;
			if (tymin > tymax) std::swap(tymin, tymax);
			if ((tmin > tymax) || (tymin > tmax))
				return false;
			if (tymin > tmin)
				tmin = tymin;
			if (tymax < tmax)
				tmax = tymax;
			float tzmin = (box.Min.z - Origin.z) / Direction.z;
			float tzmax = (box.Max.z - Origin.z) / Direction.z;
			if (tzmin > tzmax) std::swap(tzmin, tzmax);
			if ((tmin > tzmax) || (tzmin > tmax))
				return false;
			if (tzmin > tmin)
				tmin = tzmin;
			if (tzmax < tmax)
				tmax = tzmax;
			t = tmin;
			return true;
		}

		bool IntersectsTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& t) const
		{
			glm::vec3 e1 = v1 - v0;
			glm::vec3 e2 = v2 - v0;
			glm::vec3 p = glm::cross(Direction, e2);
			float a = glm::dot(e1, p);
			if (a > -0.00001f && a < 0.00001f)
				return false;
			float f = 1.0f / a;
			glm::vec3 s = Origin - v0;
			float u = f * glm::dot(s, p);
			if (u < 0.0f || u > 1.0f)
				return false;
			glm::vec3 q = glm::cross(s, e1);
			float v = f * glm::dot(Direction, q);
			if (v < 0.0f || u + v > 1.0f)
				return false;
			t = f * glm::dot(e2, q);
			return true;
		}

	};


}

