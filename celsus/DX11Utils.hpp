#ifndef _DX11_UTILS_HPP_
#define _DX11_UTILS_HPP_

#include <d3d11.h>
#include <stdint.h>

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

HRESULT create_static_vertex_buffer(ID3D11Device* device, const uint32_t vertex_count, const uint32_t vertex_size, const uint8_t* data, ID3D11Buffer** vertex_buffer);
HRESULT create_static_index_buffer(ID3D11Device* device, const uint32_t index_count, const uint32_t index_size, const uint8_t* data, ID3D11Buffer** index_buffer);

#endif