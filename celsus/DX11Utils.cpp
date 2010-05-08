#include "stdafx.h"
#include "DX11Utils.hpp"

namespace
{
  HRESULT create_static_buffer_inner(ID3D11Device* device, const D3D11_BUFFER_DESC& desc, const uint8_t* data, ID3D11Buffer** buffer)
  {
    D3D11_SUBRESOURCE_DATA init_data;
    ZeroMemory(&init_data, sizeof(init_data));
    init_data.pSysMem = data;
    return device->CreateBuffer(&desc, &init_data, buffer);
  }
}

HRESULT create_static_vertex_buffer(ID3D11Device* device, const uint32_t vertex_count, const uint32_t vertex_size, const uint8_t* data, ID3D11Buffer** vertex_buffer) 
{
  return create_static_buffer_inner(device, 
    CD3D11_BUFFER_DESC(vertex_count * vertex_size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE), data, vertex_buffer);
}

HRESULT create_static_index_buffer(ID3D11Device* device, const uint32_t index_count, const uint32_t index_size, const uint8_t* data, ID3D11Buffer** index_buffer) 
{
  return create_static_buffer_inner(device, 
    CD3D11_BUFFER_DESC(index_count * index_size, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE), data, index_buffer);
}

void set_vb(ID3D11DeviceContext *context, ID3D11Buffer *buf, const uint32_t stride)
{
  UINT ofs[] = { 0 };
  ID3D11Buffer* bufs[] = { buf };
  uint32_t strides[] = { stride };
  context->IASetVertexBuffers(0, 1, bufs, strides, ofs);
}

