#include "stdafx.h"
#include "effect_wrapper.hpp"
#include "graphics.hpp"
#include "file_utils.hpp"

EffectWrapper::EffectWrapper()
{
}

EffectWrapper::~EffectWrapper()
{
}

bool EffectWrapper::load_shaders(const char *filename, const char *vs, const char *gs, const char *ps)
{
	if (vs && !load_inner(filename, vs, VertexShader))
		return false;

	if (gs && !load_inner(filename, gs, GeometryShader))
		return false;

	if (ps && !load_inner(filename, ps, PixelShader))
		return false;

	return true;
}

bool EffectWrapper::load_inner(const char* filename, const char* entry_point, ShaderType type)
{
	uint8_t* buf = NULL;
	uint32_t len = 0;
	if ((buf = load_file(filename, &len)) == NULL)
    return false;

	ID3DBlob* error_blob = NULL;

  // Must use ps_4_0_level_9_3 or ps_4_0_level_9_1
  // Set shader version depending on feature level
  string2 vs, ps, gs;
  switch(Graphics::instance().feature_level()) {
  case D3D_FEATURE_LEVEL_9_1:
    vs = "vs_4_0_level_9_1";
		ps = "ps_4_0_level_9_1";
		gs = "gs_4_0_level_9_1";
    break;
  case D3D_FEATURE_LEVEL_9_2:
  case D3D_FEATURE_LEVEL_9_3:
    vs = "vs_4_0_level_9_3";
    ps = "ps_4_0_level_9_3";
		gs = "gs_4_0_level_9_3";
    break;
  default:
    vs = "vs_4_0";
    ps = "ps_4_0";
		gs = "gs_4_0";
    break;
  }

	ID3D11Device *device = Graphics::instance().device();
	switch (type)
	{
	case VertexShader:
		if (FAILED(D3DCompile(buf, len, filename, NULL, NULL, entry_point, vs, D3D10_SHADER_ENABLE_STRICTNESS, 0, &_vs._blob, &error_blob))) {
			LOG_ERROR_LN("%s", error_blob->GetBufferPointer());
			return false;
		}
		RETURN_ON_FAIL_BOOL(device->CreateVertexShader(_vs._blob->GetBufferPointer(), _vs._blob->GetBufferSize(), NULL, &_vs._shader), LOG_ERROR_LN);
		RETURN_ON_FAIL_BOOL(_vs.do_reflection(), LOG_ERROR_LN);
		break;

	case GeometryShader:
		if (FAILED(D3DCompile(buf, len, filename, NULL, NULL, entry_point, gs, D3D10_SHADER_ENABLE_STRICTNESS, 0, &_gs._blob, &error_blob))) {
			LOG_ERROR_LN("%s", error_blob->GetBufferPointer());
			return false;
		}
		RETURN_ON_FAIL_BOOL(device->CreateGeometryShader(_gs._blob->GetBufferPointer(), _gs._blob->GetBufferSize(), NULL, &_gs._shader), LOG_ERROR_LN);
		RETURN_ON_FAIL_BOOL(_gs.do_reflection(), LOG_ERROR_LN);
		break;

	case PixelShader:
		if (FAILED(D3DCompile(buf, len, filename, NULL, NULL, entry_point, ps, D3D10_SHADER_ENABLE_STRICTNESS, 0, &_ps._blob, &error_blob))) {
			LOG_ERROR_LN("%s", error_blob->GetBufferPointer());
			return false;
		}
		RETURN_ON_FAIL_BOOL(device->CreatePixelShader(_ps._blob->GetBufferPointer(), _ps._blob->GetBufferSize(), NULL, &_ps._shader), LOG_ERROR_LN);
		RETURN_ON_FAIL_BOOL(_ps.do_reflection(), LOG_ERROR_LN);
		break;
	}

  _filename = filename;
  return true;
}


bool EffectWrapper::set_resource(const string2& /*name*/, ID3D11ShaderResourceView* /*resource*/)
{
  return false;
}

void EffectWrapper::set_cbuffer()
{
	ID3D11DeviceContext* context = Graphics::instance().context();
	if (_vs._shader) {
		for (auto i = _vs._constant_buffers.begin(), e = _vs._constant_buffers.end(); i != e; ++i) {
			ConstantBuffer *b = i->second;
      if (b->_mapped) {
        b->_mapped = false;
        context->Unmap(b->_buffer, 0);
      }
			ID3D11Buffer* buf[1] = { b->_buffer };
			context->VSSetConstantBuffers(0, 1, buf);
		}
	} 

  if (_ps._shader) {
    for (auto i = _ps._constant_buffers.begin(), e = _ps._constant_buffers.end(); i != e; ++i) {
      ConstantBuffer *b = i->second;
      if (b->_mapped) {
        b->_mapped = false;
        context->Unmap(b->_buffer, 0);
      }
      ID3D11Buffer* buf[1] = { b->_buffer };
      context->PSSetConstantBuffers(0, 1, buf);
    }
  }
}

void EffectWrapper::unmap_buffers()
{
  _vs.unmap_buffers();
  _ps.unmap_buffers();
}

ID3D11InputLayout* EffectWrapper::create_input_layout(const D3D11_INPUT_ELEMENT_DESC* elems, const int num_elems)
{
	ID3D11InputLayout* layout = NULL;

	RETURN_ON_FAIL_PTR(Graphics::instance().device()->CreateInputLayout(elems, num_elems, _vs._blob->GetBufferPointer(), _vs._blob->GetBufferSize(), &layout),
		LOG_ERROR_LN);
	return layout;
}

ID3D11InputLayout* EffectWrapper::create_input_layout(const std::vector<D3D11_INPUT_ELEMENT_DESC>& elems)
{
	return create_input_layout(&elems[0], elems.size());
}

void EffectWrapper::set_shaders(ID3D11DeviceContext *context)
{
  if (vertex_shader()) 
    context->VSSetShader(vertex_shader(), NULL, 0);
  if (geometry_shader())
    context->GSSetShader(geometry_shader(), NULL, 0);
  if (pixel_shader())
    context->PSSetShader(pixel_shader(), NULL, 0);
}
