#include "stdafx.h"
#include "math_utils.hpp"

void spherical_to_cart(float phi, float theta, float r, D3DXVECTOR3 *result)
{
  const float sin_p = sinf(phi);
  const float cos_p = cosf(phi);
  const float sin_t = sinf(theta);
  const float cos_t = cosf(theta);
  result->x = r * sin_p * cos_t;
  result->y = r * cos_p;
  result->z = r * sin_p * sin_t;
}
