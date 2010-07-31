#pragma once
#include <D3DX10math.h>

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


// phi is angle about x-axis. 0 = 1,0,0
// theta is angle about y-axis. 0 = 0,1,0
void spherical_to_cart(float phi, float theta, float r, D3DXVECTOR3 *result);
