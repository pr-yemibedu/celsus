#pragma once
#include <D3DX10math.h>
#include <cassert>

static const double kPi = 3.14159265358979323846;

static const D3DXMATRIX kMtxId(1,0,0,0,
	0,1,0,0,
	0,0,1,0,
	0,0,0,1);

static const D3DXVECTOR3 kVec3Zero(0,0,0);
static const D3DXVECTOR3 kVec3One(1,1,1);
static const D3DXQUATERNION kQuatId(0, 0, 0, 1);

// A frame is a ON base and a position
struct Frame
{
	Frame() : e(0,0,0), x(1,0,0), y(0,1,0), z(0,0,1) {}
	Frame(const D3DXVECTOR3& x, const D3DXVECTOR3& y, const D3DXVECTOR3& z, const D3DXVECTOR3& e) : x(x), y(y), z(z), e(e) {}
	D3DXVECTOR3 x, y, z;
	D3DXVECTOR3 e;
};

inline D3DXVECTOR3 vec3_cross(const D3DXVECTOR3& a, const D3DXVECTOR3& b)
{
  return D3DXVECTOR3(
    a.y * b.z - a.z * b.y,
    a.z * b.x - a.x * b.z,
    a.x * b.y - a.y * b.x
    );
}

inline float vec3_dot(const D3DXVECTOR3& a, const D3DXVECTOR3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
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

float inline deg_to_rad(const float deg)
{
	return (float)D3DX_PI * deg / 180.0f;
}


inline D3DXMATRIX get_rotation(const D3DXMATRIX& mtx)
{
	return D3DXMATRIX(
		mtx._11, mtx._12, mtx._13, 0,
		mtx._21, mtx._22, mtx._23, 0,
		mtx._31, mtx._32, mtx._33, 0,
		0, 0, 0, 1);
}

inline D3DXVECTOR3 get_translation(const D3DXMATRIX& transform)
{
	return D3DXVECTOR3(transform._41, transform._42, transform._43);
}

inline D3DXVECTOR3 get_scale(const D3DXMATRIX& transform)
{
	return D3DXVECTOR3(transform._11, transform._22, transform._33);
}

// Find a vector orthogonal to r, according to rtr (eq. 4.24, pg 71)
D3DXVECTOR3 find_orthogonal(const D3DXVECTOR3& r);


// phi is angle about x-axis. 0 = 1,0,0
// theta is angle about y-axis. 0 = 0,1,0
void spherical_to_cart(float phi, float theta, float r, D3DXVECTOR3 *result);

inline D3DXMATRIX matrix_from_vectors(const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3, const D3DXVECTOR3& v4)
{
  return D3DXMATRIX(
    v1.x, v1.y, v1.z, 0,
    v2.x, v2.y, v2.z, 0,
    v3.x, v3.y, v3.z, 0,
    v4.x, v4.y, v4.z, 1);
}

inline D3DXMATRIX matrix_from_vectors(const D3DXVECTOR3& v1, const D3DXVECTOR3& v2, const D3DXVECTOR3& v3)
{
  return D3DXMATRIX(
    v1.x, v1.y, v1.z, 0,
    v2.x, v2.y, v2.z, 0,
    v3.x, v3.y, v3.z, 0,
    0, 0, 0, 1);
}

inline D3DXVECTOR3 matrix_get_row(const D3DXMATRIX& m, int row)
{
  switch (row) {
  case 0: return D3DXVECTOR3(m._11, m._12, m._13);
  case 1: return D3DXVECTOR3(m._21, m._22, m._23);
  case 2: return D3DXVECTOR3(m._31, m._32, m._33);
  case 3: return D3DXVECTOR3(m._41, m._42, m._43);
  default:
    assert(false);
  }
  return D3DXVECTOR3(0,0,0);
}

inline void matrix_set_row(D3DXMATRIX &m, int row, const D3DXVECTOR3& v)
{
  switch (row) {
  case 0: m._11 = v.x; m._12 = v.y; m._13 = v.z; break;
  case 1: m._21 = v.x; m._22 = v.y; m._23 = v.z; break;
  case 2: m._31 = v.x; m._32 = v.y; m._33 = v.z; break;
  case 3: m._41 = v.x; m._42 = v.y; m._43 = v.z; break;
  }
}

// if mtx = proj-space, planes are in camera-space
// if mtx = view*proj-space, planes are in world-space and so on
void calc_planes(const D3DXMATRIX& mtx, D3DXPLANE *planes);

// computes the intersection of 2 planes, according to RTCD, page 209 (unoptimized version)
bool intersect_unopt(const D3DXPLANE& p1, const D3DXPLANE& p2, D3DXVECTOR3 *p, D3DXVECTOR3 *d);
bool intersect(const D3DXPLANE& p1, const D3DXPLANE& p2, D3DXVECTOR3 *p, D3DXVECTOR3 *d);

// computes the point at which 3 planes intersect (if at all)
bool intersect(const D3DXPLANE& p1, const D3DXPLANE& p2, const D3DXPLANE& p3, D3DXVECTOR3 *p);
