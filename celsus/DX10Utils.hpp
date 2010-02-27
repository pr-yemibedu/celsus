#ifndef DX_UTILS_HPP
#define DX_UTILS_HPP

#include <D3DX10.h>
#include <utility>
#include <vector>
#include <stdint.h>
#include "D3D10Types.hpp"

ID3D10Effect* load_effect(const char* filename, ID3D10Device* device);

const D3DXMATRIX kMtxId(1,0,0,0,
                        0,1,0,0,
                        0,0,1,0,
                        0,0,0,1);

const D3DXVECTOR3 kVec3Zero(0,0,0);
const D3DXVECTOR3 kVec3One(1,1,1);
const D3DXQUATERNION kQuatId(0, 0, 0, 1);

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

inline D3DXVECTOR3 normalize(const D3DXVECTOR3& a)
{
  const float len = D3DXVec3Length(&a);
  if (len > 0 ) {
    return a / len;
  }
  return kVec3Zero;
}

struct D3DXNORMAL : public D3DXVECTOR3
{
  D3DXNORMAL() {}
  D3DXNORMAL( CONST FLOAT * f) : D3DXVECTOR3(f) {}
  D3DXNORMAL( CONST D3DVECTOR& v) : D3DXVECTOR3(v) {}
  D3DXNORMAL( CONST D3DXFLOAT16 * f) : D3DXVECTOR3(f) {}
  D3DXNORMAL( FLOAT x, FLOAT y, FLOAT z ) : D3DXVECTOR3(x, y, z) {}
};

struct layout_maker
{
  template<typename T> struct info_from_type;
  typedef std::pair< const char*, DXGI_FORMAT > NameFormat;
  template<> struct info_from_type<D3DXVECTOR3> { static NameFormat info() { return std::make_pair("POSITION", DXGI_FORMAT_R32G32B32_FLOAT); } };
  template<> struct info_from_type<D3DXNORMAL> { static NameFormat info() { return std::make_pair("NORMAL", DXGI_FORMAT_R32G32B32_FLOAT); } };
  template<> struct info_from_type<D3DXVECTOR2> { static NameFormat info() { return std::make_pair("TEXCOORD", DXGI_FORMAT_R32G32_FLOAT); } };
  template<> struct info_from_type<D3DXCOLOR> { static NameFormat info() { return std::make_pair("COLOR", DXGI_FORMAT_R32G32B32A32_FLOAT); } };

  typedef std::vector<D3D10_INPUT_ELEMENT_DESC> InputDescs;

  layout_maker(InputDescs& descs)
    : offset_(0)
    , descs_(descs)
  {
    descs_.clear();
  }

  template<class T>
  void operator()(const T& t)
  {
    NameFormat f = info_from_type<T>::info();
    D3D10_INPUT_ELEMENT_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.SemanticName = f.first;
    desc.Format = f.second;
    desc.AlignedByteOffset = offset_;

    offset_ += sizeof(T);
    descs_.push_back(desc);
  }

  uint32_t offset_;
  InputDescs& descs_;
};

// TList is a boost mpl::vector of the elements, ex mpl::vector<D3DXVECTOR3, D3DXCOLOR>
template<class TList>
bool create_input_layout(ID3D10InputLayoutPtr& layout, const D3D10_PASS_DESC& desc, ID3D10Device* device)
{
  layout_maker::InputDescs v;
  mpl::for_each<TList>(layout_maker(v));

  ID3D10InputLayout* layout_ptr = NULL;
  if (FAILED(device->CreateInputLayout(&v[0], v.size(), desc.pIAInputSignature, desc.IAInputSignatureSize, &layout_ptr))) {
    return false;
  }
  layout.Attach(layout_ptr);
  return true;
}

template<class TList>
bool create_input_layout(ID3D10InputLayout*& layout, const D3D10_PASS_DESC& desc, ID3D10Device* device)
{
  layout_maker::InputDescs v;
  mpl::for_each<TList>(layout_maker(v));

  if (FAILED(device->CreateInputLayout(&v[0], v.size(), desc.pIAInputSignature, desc.IAInputSignatureSize, &layout))) {
    return false;
  }
  return true;
}

D3D10_VIEWPORT* init_d3d10_viewport(D3D10_VIEWPORT* viewport,     
                                  const int32_t top_left_x, const int32_t top_left_y, 
                                  const uint32_t width, const uint32_t height, 
                                  const float min_depth, const float max_depth);

DXGI_SAMPLE_DESC* init_dxgi_sample_desc(DXGI_SAMPLE_DESC* desc, const uint32_t count, const uint32_t quality);

DXGI_SAMPLE_DESC init_dxgi_sample_desc(const uint32_t count, const uint32_t quality);

D3D10_TEXTURE2D_DESC* init_d3d10_texture2d_desc(D3D10_TEXTURE2D_DESC* desc,
                                             const uint32_t width, const uint32_t height,
                                             const uint32_t mip_levels,
                                             const uint32_t array_size,
                                             const DXGI_FORMAT format,
                                             const DXGI_SAMPLE_DESC& sample_desc,
                                             const D3D10_USAGE usage,
                                             const uint32_t bind_flags,
                                             const uint32_t cpu_access_flags = 0,
                                             const uint32_t misc_flags = 0);

D3D10_TEXTURE2D_DESC* init_d3d10_texture2d_desc(D3D10_TEXTURE2D_DESC* desc,
                                             const uint32_t width, const uint32_t height,
                                             const uint32_t mip_levels,
                                             const uint32_t array_size,
                                             const DXGI_FORMAT format,
                                             const uint32_t count,
                                             const uint32_t quality,
                                             const D3D10_USAGE usage,
                                             const uint32_t bind_flags,
                                             const uint32_t cpu_access_flags = 0,
                                             const uint32_t misc_flags = 0);

HRESULT create_render_target(ID3D10Texture2D*& render_target, ID3D10Device* device, const uint32_t width, const uint32_t height, const DXGI_FORMAT format);
HRESULT create_render_target_and_view(ID3D10Texture2D*& render_target, ID3D10ShaderResourceView*& shader_resource_view, ID3D10RenderTargetView*& view, 
                                      ID3D10Device* device, const uint32_t width, const uint32_t height, const DXGI_FORMAT format);

HRESULT create_depth_stencil(ID3D10Texture2D*& depth_stencil_, ID3D10Device* device, const uint32_t width, const uint32_t height, const DXGI_FORMAT format);
HRESULT create_depth_stencil_and_view(ID3D10Texture2D*& depth_stencil_, ID3D10DepthStencilView*& depth_stencil_view_, 
                                      ID3D10Device* device, const uint32_t width, const uint32_t height, const DXGI_FORMAT format);


bool create_static_vertex_buffer(ID3D10Buffer*& vertex_buffer, ID3D10Device* device, 
                                 const uint8_t* data, const uint32_t vertex_count, const uint32_t vertex_size);

bool create_static_index_buffer(ID3D10Buffer*& index_buffer, ID3D10Device* device, 
                                const uint8_t* data, const uint32_t index_count, const uint32_t index_size);

#endif
