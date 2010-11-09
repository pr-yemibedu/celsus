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


void calc_planes(const D3DXMATRIX& m, D3DXPLANE *planes)
{
  // From "Fast Extraction of Viewing Frustum Planes from the World-View-Projection Matrix"

  // left
  planes[0].a = m._14 + m._11;
  planes[0].b = m._24 + m._21;
  planes[0].c = m._34 + m._31;
  planes[0].d = m._44 + m._41;

  // right
  planes[1].a = m._14 - m._11;
  planes[1].b = m._24 - m._21;
  planes[1].c = m._34 - m._31;
  planes[1].d = m._44 - m._41;

  // bottom
  planes[2].a = m._14 + m._12;
  planes[2].b = m._24 + m._22;
  planes[2].c = m._34 + m._32;
  planes[2].d = m._44 + m._42;

  // top
  planes[3].a = m._14 - m._12;
  planes[3].b = m._24 - m._22;
  planes[3].c = m._34 - m._32;
  planes[3].d = m._44 - m._42;

  // near
  planes[4].a = m._13;
  planes[4].b = m._23;
  planes[4].c = m._33;
  planes[4].d = m._43;

  // far
  planes[5].a = m._14 - m._13;
  planes[5].b = m._24 - m._23;
  planes[5].c = m._34 - m._33;
  planes[5].d = m._44 - m._43;

  for (int i = 0; i < 6; ++i)
    D3DXPlaneNormalize(&planes[i], &planes[i]);
}

bool intersect(const D3DXPLANE& p1, const D3DXPLANE& p2, D3DXVECTOR3 *p, D3DXVECTOR3 *d)
{
  // note, d3dx stores planes in ax + by + cz + d = 0 form, but rtcd wants them
  // in ax + by + cz = d form.
  const D3DXVECTOR3 p1n(p1.a, p1.b, p1.c);
  const D3DXVECTOR3 p2n(p2.a, p2.b, p2.c);
  const float p1d = -p1.d;
  const float p2d = -p2.d;

  const float kEps = 0.0001f;
  // compute direction of intersection line
  *d = vec3_cross(p1n, p2n);

  // if the planes are parallel, they're not considered intersecting
  if (vec3_dot(*d, *d) < kEps)
    return false;

  const float d11 = vec3_dot(p1n, p1n);
  const float d12 = vec3_dot(p1n, p2n);
  const float d22 = vec3_dot(p2n, p2n);

  const float denom = d11*d22 - d12*d12;
  const float k1 = (p1d*d22 - p2d*d12) / denom;
  const float k2 = (p2d*d11 - p1d*d12) / denom;
  *p = k1*p1n + k2*p2n;

  return true;
}
