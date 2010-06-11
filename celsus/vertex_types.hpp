#pragma once
#include <D3DX10math.h>

// Some common vertex types collected in one place..
struct PosTex
{
  PosTex() {}
  PosTex(const D3DXVECTOR3& pos, const D3DXVECTOR2& tex) : pos(pos), tex(tex) {}
  D3DXVECTOR3 pos;
  D3DXVECTOR2 tex;
};

struct PosCol
{
  PosCol() {}
  PosCol(const D3DXVECTOR3& pos, const D3DXCOLOR& col) : pos(pos), col(col) {}
  PosCol(const D3DXVECTOR3& pos) : pos(pos) {}
  D3DXVECTOR3 pos;
  D3DXCOLOR col;
};
