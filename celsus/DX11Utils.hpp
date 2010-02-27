#ifndef _DX11_UTILS_HPP_
#define _DX11_UTILS_HPP_

#include <d3d11.h>
#include <stdint.h>

HRESULT create_static_vertex_buffer(ID3D11Device* device, const uint32_t vertex_count, const uint32_t vertex_size, const uint8_t* data, ID3D11Buffer** vertex_buffer);
HRESULT create_static_index_buffer(ID3D11Device* device, const uint32_t index_count, const uint32_t index_size, const uint8_t* data, ID3D11Buffer** index_buffer);

#endif