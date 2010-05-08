#ifndef _DX11_UTILS_HPP_
#define _DX11_UTILS_HPP_

#include <d3d11.h>
#include <stdint.h>
#include <vector>

struct CD3D11_INPUT_ELEMENT_DESC : public D3D11_INPUT_ELEMENT_DESC
{
	CD3D11_INPUT_ELEMENT_DESC(LPCSTR name, UINT index, DXGI_FORMAT fmt, UINT slot, 
		UINT byte_offset = D3D11_APPEND_ALIGNED_ELEMENT, 
		D3D11_INPUT_CLASSIFICATION slot_class = D3D11_INPUT_PER_VERTEX_DATA, 
		UINT step_rate = 0)
	{
		SemanticName = name;
		SemanticIndex = index;
		Format = fmt;
		InputSlot = slot;
		AlignedByteOffset = byte_offset;
		InputSlotClass = slot_class;
		InstanceDataStepRate = step_rate;
	}
};

template<class T>
HRESULT create_static_vertex_buffer(ID3D11Device* device, const std::vector<T>& v, ID3D11Buffer** vertex_buffer)
{
  return create_static_vertex_buffer(device, v.size(), sizeof(T), (const uint8_t*)&v[0], vertex_buffer);
}

void set_vb(ID3D11DeviceContext *context, ID3D11Buffer *buf, const uint32_t stride);

HRESULT create_static_vertex_buffer(ID3D11Device* device, const uint32_t vertex_count, const uint32_t vertex_size, const uint8_t* data, ID3D11Buffer** vertex_buffer);
HRESULT create_static_index_buffer(ID3D11Device* device, const uint32_t index_count, const uint32_t index_size, const uint8_t* data, ID3D11Buffer** index_buffer);

#endif