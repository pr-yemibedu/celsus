#pragma once
#include <D3DX10math.h>

inline D3DXVECTOR3 vec3_cross(const D3DXVECTOR3& a, const D3DXVECTOR3& b)
{
  return D3DXVECTOR3(
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
    );
}

inline float vec3_len(const D3DXVECTOR3& a)
{
  return sqrtf(a.x * a.x + a.y * a.y + a.z * a.z);
}

inline D3DXVECTOR3 vec3_normalize(const D3DXVECTOR3& a)
{
  D3DXVECTOR3 res;
  const float len = vec3_len(a);
  if (len == 0)
    res.x = res.y = res.z = 0;
  else
    res = a / len;
  return res;
}
