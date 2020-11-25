#include "Scene.h"
#include <glm/glm.hpp>
#include <vector>

class AABB
{
public:
    AABB() { reset(); }
    ~AABB() {}

    void reset()
    {
        // reset min to the largest value and
        min = glm::vec3(INFINITY);
        // reset max to the smallest possible value
        max = -glm::vec3(INFINITY);
        // since we want to find the minimum and maximum using fit() the array
        // 
    }

    void fit(const std::vector<glm::vec3>& points)
    {
        reset();
        for (auto& p : points)
        {
            min = glm::min(p, min);
            max = glm::max(p, max);
        }
    }

    glm::vec3 min, max;
};

class SphereBounds
{
public:
    SphereBounds() {}

    glm::vec3 center;
    float radius;
};