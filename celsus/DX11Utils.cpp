#include "stdafx.h"
#include "DX11Utils.hpp"

namespace
{
  HRESULT create_static_buffer_inner(ID3D11Device* device, const CD3D11_BUFFER_DESC& desc, const uint8_t* data, ID3D11Buffer** buffer)
  {
    D3D11_SUBRESOURCE_DATA init_data;
    ZeroMemory(&init_data, sizeof(init_data));
    init_data.pSysMem = data;
    return device->CreateBuffer(&desc, &init_data, buffer);
  }
}

HRESULT create_static_vertex_buffer(ID3D11Device* device, const uint32_t vertex_count, const uint32_t vertex_size, const uint8_t* data, ID3D11Buffer** vertex_buffer) 
{
  CD3D11_BUFFER_DESC buffer_desc(vertex_count * vertex_size, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE);
  return create_static_buffer_inner(device, buffer_desc, data, vertex_buffer);
}

HRESULT create_static_index_buffer(ID3D11Device* device, const uint32_t index_count, const uint32_t index_size, const uint8_t* data, ID3D11Buffer** index_buffer) 
{
  CD3D11_BUFFER_DESC buffer_desc(index_count * index_size, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE);
  return create_static_buffer_inner(device, buffer_desc, data, index_buffer);
}
