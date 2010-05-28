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

bool EffectWrapper::load_shaders(const char *filename, const char *vs, const char *ps)
{
  if (!load_vertex_shader(filename, vs))
    return false;

  if (!load_pixel_shader(filename, ps))
    return false;

  return true;
}

bool EffectWrapper::load_vertex_shader(const char* filename, const char* entry_point)
{
	return load_inner(filename, entry_point, true);
}

bool EffectWrapper::load_pixel_shader(const char* filename, const char* entry_point)
{
	return load_inner(filename, entry_point, false);
}

bool EffectWrapper::load_inner(const char* filename, const char* entry_point, bool vertex_shader)
{
	uint8_t* buf = NULL;
	uint32_t len = 0;
	if ((buf = load_file(filename, &len)) == NULL)
    return false;

	ID3DBlob* error_blob = NULL;

  // Must use ps_4_0_level_9_3 or ps_4_0_level_9_1
  // Set shader version depending on feature level
  std::string vs, ps;
  switch(Graphics::instance().feature_level()) {
  case D3D_FEATURE_LEVEL_9_1:
    vs = "vs_4_0_level_9_1";
    ps = "ps_4_0_level_9_1";
    break;
  case D3D_FEATURE_LEVEL_9_2:
  case D3D_FEATURE_LEVEL_9_3:
    vs = "vs_4_0_level_9_3";
    ps = "ps_4_0_level_9_3";
    break;
  default:
    vs = "vs_4_0";
    ps = "ps_4_0";
    break;
  }

	if (vertex_shader) {

		if (FAILED(D3DCompile(buf, len, filename, NULL, NULL, entry_point, vs.c_str(), D3D10_SHADER_ENABLE_STRICTNESS, 0, &_vs._blob, &error_blob))) {
			LOG_ERROR_LN("%s", error_blob->GetBufferPointer());
			return false;
		}
		RETURN_ON_FAIL_BOOL(Graphics::instance().device()->CreateVertexShader(_vs._blob->GetBufferPointer(), _vs._blob->GetBufferSize(), NULL, &_vs._shader),
			LOG_ERROR_LN);

	} else {
		if (FAILED(D3DCompile(buf, len, filename, NULL, NULL, entry_point, ps.c_str(), D3D10_SHADER_ENABLE_STRICTNESS, 0, &_ps._blob, &error_blob))) {
			LOG_ERROR_LN("%s", error_blob->GetBufferPointer());
			return false;
		}
		RETURN_ON_FAIL_BOOL(Graphics::instance().device()->CreatePixelShader(_ps._blob->GetBufferPointer(), _ps._blob->GetBufferSize(), NULL, &_ps._shader),
			LOG_ERROR_LN);
	}

	RETURN_ON_FAIL_BOOL(do_reflection(), LOG_ERROR_LN);

  _filename = filename;
  return true;
}


bool EffectWrapper::do_reflection()
{
  if (_vs._shader && !_vs.do_reflection())
    return false;
  if (_ps._shader && !_ps.do_reflection())
    return false;
	return true;
}


bool EffectWrapper::set_resource(const std::string& /*name*/, ID3D11ShaderResourceView* /*resource*/)
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
  context->VSSetShader(vertex_shader(), NULL, 0);
  context->PSSetShader(pixel_shader(), NULL, 0);
  context->GSSetShader(NULL, NULL, 0);
}
