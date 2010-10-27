#include "stdafx.h"
#include "DX11Utils.hpp"

namespace
{
  HRESULT create_buffer_inner(ID3D11Device* device, const D3D11_BUFFER_DESC& desc, const void* data, ID3D11Buffer** buffer)
  {
    D3D11_SUBRESOURCE_DATA init_data;
    ZeroMemory(&init_data, sizeof(init_data));
    init_data.pSysMem = data;
    return device->CreateBuffer(&desc, data ? &init_data : NULL, buffer);
  }
}

HRESULT create_static_vertex_buffer(ID3D11Device* device, const uint32_t vertex_count, const uint32_t vertex_size, const void* data, ID3D11Buffer** vertex_buffer) 
{
  return create_buffer_inner(device, CD3D11_BUFFER_DESC(vertex_count * vertex_size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE), data, vertex_buffer);
}

HRESULT create_dynamic_vertex_buffer(ID3D11Device *device, const uint32_t vertex_count, const uint32_t vertex_size, ID3D11Buffer** vertex_buffer)
{
  return create_buffer_inner(device, CD3D11_BUFFER_DESC(vertex_count * vertex_size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), NULL, vertex_buffer);
}

HRESULT create_static_index_buffer(ID3D11Device* device, const uint32_t index_count, const uint32_t index_size, const void* data, ID3D11Buffer** index_buffer) 
{
  return create_buffer_inner(device, CD3D11_BUFFER_DESC(index_count * index_size, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE), data, index_buffer);
}

HRESULT create_cbuffer(ID3D11Device *device, uint32_t buffer_size, ID3D11Buffer **buffer)
{
  return create_buffer_inner(device, CD3D11_BUFFER_DESC(buffer_size, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE), NULL, buffer);
}

void set_vb(ID3D11DeviceContext *context, ID3D11Buffer *buf, const uint32_t stride)
{
  UINT ofs[] = { 0 };
  ID3D11Buffer* bufs[] = { buf };
  uint32_t strides[] = { stride };
  context->IASetVertexBuffers(0, 1, bufs, strides, ofs);
}

void* map_buffer(ID3D11DeviceContext *context, ID3D11Buffer *buffer)
{
  D3D11_MAPPED_SUBRESOURCE res;
  context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
  return res.pData;
}

void unmap_buffer(ID3D11DeviceContext *context, ID3D11Buffer *buffer)
{
  context->Unmap(buffer, 0);
}

D3DXVECTOR3 screen_to_clip(const D3DXVECTOR3& screen, const D3D11_VIEWPORT& v)
{
	return D3DXVECTOR3((screen.x - v.Width / 2) / (v.Width / 2), (v.Height / 2 - screen.y) / (v.Height / 2), 0);
}

void screen_to_clip(float x, float y, float w, float h, float *ox, float *oy)
{
  *ox = (x - w / 2) / (w / 2);
  *oy = (h/2 - y) / (h/2);
}
