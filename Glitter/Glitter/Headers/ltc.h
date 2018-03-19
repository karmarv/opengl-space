
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cstdio>
#include <algorithm> 

const int sizem = 64;

struct mat33
{
    operator glm::mat3() const
    {
        return glm::mat3(m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8]);
    }

    double m[9];
};

// Data for precomputed LTC 
#include "ltc.inc"
#include "ltcg.inc"

#if 1
glm::mat3 M_GGX(const float theta, const float alpha)
{
	int t = std::max<int>(0, std::min<int>(sizem-1, (int)floorf(theta / (0.5f*3.14159f) * sizem)));
	int a = std::max<int>(0, std::min<int>(sizem-1, (int)floorf(sqrtf(alpha) * sizem)));

	return tabM[a + t*sizem];
}

float amplitude_GGX(const float theta, const float alpha)
{
	int t = std::max<int>(0, std::min<int>(sizem-1, (int)floorf(theta / (0.5f*3.14159f) * sizem)));
	int a = std::max<int>(0, std::min<int>(sizem-1, (int)floorf(sqrtf(alpha) * sizem)));

	return tabAmplitude[a + t*sizem];
}
#endif
