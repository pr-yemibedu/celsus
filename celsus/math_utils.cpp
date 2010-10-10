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

D3DXVECTOR3 find_orthogonal(const D3DXVECTOR3& r)
{
  const float abs_x = fabs(r.x);
  const float abs_y = fabs(r.y);
  const float abs_z = fabs(r.z);
  D3DXVECTOR3 res;

  if (abs_x < abs_y && abs_x < abs_z) {
    res = D3DXVECTOR3(0, -r.z, r.y);
  } else if (abs_y < abs_x && abs_y < abs_z) {
    res = D3DXVECTOR3(-r.z, 0, r.x);
  } else {
    res = D3DXVECTOR3(-r.y, r.x, 0);
  }
  return res;
}
