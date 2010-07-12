#pragma once
#include <D3DX10math.h>

// Some common vertex types collected in one place..

struct PosNormal
{
	PosNormal() {}
	PosNormal(const D3DXVECTOR3& pos, const D3DXVECTOR3& normal) : pos(pos), normal(normal) {}
  PosNormal(float x, float y, float z, float nx, float ny, float nz) : pos(x, y, z), normal(nx, ny, nz) {}
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
};

struct PosTex
{
  PosTex() {}
  PosTex(const D3DXVECTOR3& pos, const D3DXVECTOR2& tex) : pos(pos), tex(tex) {}
  PosTex(float x, float y, float z, float u, float v) : pos(x, y, z), tex(u, v) {}
  D3DXVECTOR3 pos;
  D3DXVECTOR2 tex;
};

struct PosCol
{
  PosCol() {}
	PosCol(float x, float y, float z, float r, float g, float b, float a) : pos(x, y, z), col(r, g, b, a) {}
  PosCol(const D3DXVECTOR3& pos, const D3DXCOLOR& col) : pos(pos), col(col) {}
  PosCol(const D3DXVECTOR3& pos) : pos(pos) {}
  D3DXVECTOR3 pos;
  D3DXCOLOR col;
};

struct PosNormalTex
{
	PosNormalTex() {}
	PosNormalTex(const D3DXVECTOR3& pos, const D3DXVECTOR3& normal, const D3DXVECTOR2& tex) : pos(pos), normal(normal), tex(tex) {}
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex;
};
