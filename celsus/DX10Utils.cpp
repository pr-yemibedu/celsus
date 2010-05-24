#include "stdafx.h"
#include "DX10Utils.hpp"
#include "celsus.hpp"
#include "Logger.hpp"
#include "CelsusExtra.hpp"

ID3D10Effect* load_effect(const char* filename, ID3D10Device* device)
{
  uint32_t len = 0;
  uint8_t* hlsl = load_file_with_zero_terminate(filename, &len);
  
  if (!hlsl) {
    LOG_WARNING_LN("Error loading file: %s", filename);
    return NULL;
  }
  MAKE_SCOPED(boost::scoped_array<uint8_t>)(hlsl);

  ID3D10Effect* d3d_effect = NULL;
  ID3D10Blob* errors = NULL;
  if (FAILED(D3DX10CreateEffectFromMemory(
    hlsl, len, filename, NULL, NULL, "fx_4_0", 
    D3D10_SHADER_ENABLE_STRICTNESS|D3D10_SHADER_OPTIMIZATION_LEVEL3, 
    D3D10_EFFECT_SINGLE_THREADED, device, NULL, NULL, &d3d_effect, &errors, NULL))) {
      if (errors != NULL) {
        if (void* buffer = errors->GetBufferPointer()) {
          LOG_WARNING_LN("%s", buffer);
          return NULL;
        }
      } else {
        LOG_WARNING_LN("Error in call to D3DX10CreateEffectFromMemory for: %s", filename);
      }
      return NULL;
  }
  return d3d_effect;
}


D3D10_VIEWPORT* init_d3d10_viewport(D3D10_VIEWPORT* viewport,     
                                  const int32_t top_left_x, const int32_t top_left_y, 
                                  const uint32_t width, const uint32_t height, 
                                  const float min_depth, const float max_depth) 
{
  viewport->TopLeftX = top_left_x;
  viewport->TopLeftY = top_left_y;
  viewport->Width = width;
  viewport->Height = height;
  viewport->MinDepth = min_depth;
  viewport->MaxDepth = max_depth;
  return viewport;
}

DXGI_SAMPLE_DESC* init_dxgi_sample_desc(DXGI_SAMPLE_DESC* desc, const uint32_t count, const uint32_t quality)  
{
  desc->Count = count;
  desc->Quality = quality;
  return desc;
}

DXGI_SAMPLE_DESC init_dxgi_sample_desc(const uint32_t count, const uint32_t quality)  
{
  DXGI_SAMPLE_DESC desc;
  init_dxgi_sample_desc(&desc, count, quality);
  return desc;
}

D3D10_TEXTURE2D_DESC* init_d3d10_texture2d_desc(D3D10_TEXTURE2D_DESC* desc,
                                             const uint32_t width, const uint32_t height,
                                             const uint32_t mip_levels,
                                             const uint32_t array_size,
                                             const DXGI_FORMAT format,
                                             const DXGI_SAMPLE_DESC& sample_desc,
                                             const D3D10_USAGE usage,
                                             const uint32_t bind_flags,
                                             const uint32_t cpu_access_flags,
                                             const uint32_t misc_flags) 
{
  desc->Width = width;
  desc->Height = height;
  desc->MipLevels = mip_levels;
  desc->ArraySize = array_size;
  desc->Format = format;
  desc->SampleDesc = sample_desc;
  desc->Usage = usage;
  desc->BindFlags = bind_flags;
  desc->CPUAccessFlags = cpu_access_flags;
  desc->MiscFlags = misc_flags;
  return desc;
}

D3D10_TEXTURE2D_DESC* init_d3d10_texture2d_desc(D3D10_TEXTURE2D_DESC* desc,
                                             const uint32_t width, const uint32_t height,
                                             const uint32_t mip_levels,
                                             const uint32_t array_size,
                                             const DXGI_FORMAT format,
                                             const uint32_t count,
                                             const uint32_t quality,
                                             const D3D10_USAGE usage,
                                             const uint32_t bind_flags,
                                             const uint32_t cpu_access_flags,
                                             const uint32_t misc_flags) 
{
  desc->Width = width;
  desc->Height = height;
  desc->MipLevels = mip_levels;
  desc->ArraySize = array_size;
  desc->Format = format;
  desc->SampleDesc.Count = count;
  desc->SampleDesc.Quality = quality;
  desc->Usage = usage;
  desc->BindFlags = bind_flags;
  desc->CPUAccessFlags = cpu_access_flags;
  desc->MiscFlags = misc_flags;
  return desc;
}

HRESULT create_render_target(ID3D10Texture2D*& render_target, ID3D10Device* device, const uint32_t width, const uint32_t height, const DXGI_FORMAT format) 
{
  D3D10_TEXTURE2D_DESC desc;
  init_d3d10_texture2d_desc(&desc, 
    width, height, 1, 1, format, init_dxgi_sample_desc(1, 0), D3D10_USAGE_DEFAULT, D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE);
  return device->CreateTexture2D(&desc, NULL, &render_target);
}

HRESULT create_render_target_and_view(ID3D10Texture2D*& render_target, ID3D10ShaderResourceView*& shader_resource_view, ID3D10RenderTargetView*& view, 
                                      ID3D10Device* device, const uint32_t width, const uint32_t height, const DXGI_FORMAT format) 
{
  HRESULT res = S_OK;
  if ( (res = create_render_target(render_target, device, width, height, format)) != S_OK) {
    return res;
  }

  if ((res = device->CreateRenderTargetView(render_target, NULL, &view)) != S_OK ) {
    return res;
  }

  // Create the shader-resource view
  if ( (res = device->CreateShaderResourceView( render_target, NULL, &shader_resource_view)) != S_OK ) {
    return res;
  }

  return S_OK;
}

HRESULT create_depth_stencil(ID3D10Texture2D*& depth_stencil_, ID3D10Device* device, const uint32_t width, const uint32_t height, const DXGI_FORMAT format)
{
  D3D10_TEXTURE2D_DESC descDepth;
  init_d3d10_texture2d_desc(&descDepth, width, height, 1, 1, format, 
    *init_dxgi_sample_desc(&descDepth.SampleDesc, 1, 0), D3D10_USAGE_DEFAULT, D3D10_BIND_DEPTH_STENCIL);

  return device->CreateTexture2D( &descDepth, NULL, &depth_stencil_ );
}

HRESULT create_depth_stencil_and_view(ID3D10Texture2D*& depth_stencil_, ID3D10DepthStencilView*& depth_stencil_view_, 
                                      ID3D10Device* device, const uint32_t width, const uint32_t height, const DXGI_FORMAT format)
{

  D3D10_TEXTURE2D_DESC descDepth;
  init_d3d10_texture2d_desc(&descDepth, width, height, 1, 1, format, 
    *init_dxgi_sample_desc(&descDepth.SampleDesc, 1, 0), D3D10_USAGE_DEFAULT, D3D10_BIND_DEPTH_STENCIL);

  HRESULT res = device->CreateTexture2D( &descDepth, NULL, &depth_stencil_ );
  if (FAILED(res)) {
    return res;
  }

  D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
  descDSV.Format = descDepth.Format;
  descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
  descDSV.Texture2D.MipSlice = 0;
  return device->CreateDepthStencilView( depth_stencil_, &descDSV, &depth_stencil_view_);
}

bool create_static_vertex_buffer(ID3D10Buffer*& vertex_buffer, ID3D10Device* device, 
                                 const uint8_t* data, const uint32_t vertex_count, const uint32_t vertex_size) 
{
  D3D10_BUFFER_DESC buffer_desc;
  ZeroMemory(&buffer_desc, sizeof(buffer_desc));
  buffer_desc.Usage     = D3D10_USAGE_IMMUTABLE;
  buffer_desc.ByteWidth = vertex_count * vertex_size;
  buffer_desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;

  D3D10_SUBRESOURCE_DATA init_data;
  ZeroMemory(&init_data, sizeof(init_data));
  init_data.pSysMem = data;
  vertex_buffer = NULL;
  return SUCCEEDED(device->CreateBuffer(&buffer_desc, &init_data, &vertex_buffer));
}

bool create_static_index_buffer(ID3D10Buffer*& index_buffer, ID3D10Device* device, 
                                const uint8_t* data, const uint32_t index_count, const uint32_t index_size) 
{
  D3D10_BUFFER_DESC buffer_desc;
  ZeroMemory(&buffer_desc, sizeof(buffer_desc));
  buffer_desc.Usage     = D3D10_USAGE_IMMUTABLE;
  buffer_desc.ByteWidth = index_count * index_size;
  buffer_desc.BindFlags = D3D10_BIND_INDEX_BUFFER;

  D3D10_SUBRESOURCE_DATA init_data;
  ZeroMemory(&init_data, sizeof(init_data));
  init_data.pSysMem = data;
  return SUCCEEDED(device->CreateBuffer(&buffer_desc, &init_data, &index_buffer));
}
