#ifndef _DX11_UTILS_HPP_
#define _DX11_UTILS_HPP_

#include <d3d11.h>
#include <D3DX10math.h>
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

struct StaticBuffer {
	CComPtr<ID3D11Buffer> buffer;
	int stride;
	int num_elems;
};

template<class T>
HRESULT create_static_vertex_buffer(ID3D11Device* device, const std::vector<T>& v, ID3D11Buffer** vertex_buffer)
{
  return create_static_vertex_buffer(device, v.size(), sizeof(T), (const uint8_t*)&v[0], vertex_buffer);
}

template<class T>
HRESULT create_static_index_buffer(ID3D11Device* device, const std::vector<T>& v, ID3D11Buffer** index_buffer)
{
	return create_static_index_buffer(device, v.size(), sizeof(T), (const uint8_t*)&v[0], vertex_buffer);
}

void set_ib(ID3D11DeviceContext *context, const StaticBuffer &ib);
void set_vb(ID3D11DeviceContext *context, const StaticBuffer &vb);

void set_ib(ID3D11DeviceContext *context, ID3D11Buffer *buf);
void set_vb(ID3D11DeviceContext *context, ID3D11Buffer *buf, const uint32_t stride);

HRESULT create_static_vertex_buffer(ID3D11Device* device, const uint32_t vertex_count, const uint32_t vertex_size, const void* data, StaticBuffer *vb);
HRESULT create_static_vertex_buffer(ID3D11Device* device, const uint32_t vertex_count, const uint32_t vertex_size, const void* data, ID3D11Buffer** vertex_buffer);

HRESULT create_static_index_buffer(ID3D11Device* device, const uint32_t index_count, const uint32_t index_size, const void* data, StaticBuffer *ib);
HRESULT create_static_index_buffer(ID3D11Device* device, const uint32_t index_count, const uint32_t index_size, const void* data, ID3D11Buffer** index_buffer);

HRESULT create_dynamic_vertex_buffer(ID3D11Device *device, const uint32_t vertex_count, const uint32_t vertex_size, ID3D11Buffer** vertex_buffer);
HRESULT create_cbuffer(ID3D11Device *device, uint32_t buffer_size, ID3D11Buffer **buffer);

void* map_buffer(ID3D11DeviceContext *context, ID3D11Buffer *buffer);
void unmap_buffer(ID3D11DeviceContext *context, ID3D11Buffer *buffer);

// maps from screen space (0,0) top left, (width-1, height-1) bottom right
// to clip space (-1,+1) top left, (+1, -1) bottom right
D3DXVECTOR3 screen_to_clip(const D3DXVECTOR3& screen, const D3D11_VIEWPORT& v);
void screen_to_clip(float x, float y, float w, float h, float *ox, float *oy);

inline DXGI_FORMAT index_size_to_format(int size) 
{
	switch (size) {
	case 2: return DXGI_FORMAT_R16_UINT;
	case 4: return DXGI_FORMAT_R32_UINT;
	default: assert(false);
	}
	return DXGI_FORMAT_UNKNOWN;
}

#endif
