#include "stdafx.h"
#include "graphics.hpp"
#include "error2.hpp"
#include "Logger.hpp"
#include "D3D11Descriptions.hpp"
#include <DxErr.h>

using namespace std;

#pragma comment(lib, "DXGI.lib")

Graphics* Graphics::_instance = NULL;
bool Graphics::_deleted = false;

Graphics& Graphics::instance()
{
  assert(!_deleted);
	if (_instance == NULL) {
		_instance = new Graphics();
	}

	return *_instance;
}

Graphics::Graphics()
	: _width(-1)
	, _height(-1)
  , _clear_color(0,0,0,1)
  , _start_fps_time(0xffffffff)
  , _frame_count(0)
  , _fps(0)
{
}

Graphics::~Graphics()
{
  _deleted = true;
}

struct ErrorCode
{
	explicit ErrorCode(HRESULT hr);
	explicit ErrorCode(bool b);
	operator bool() { return SUCCEEDED(hr); }
	string2 error_string() const;

	HRESULT hr;
};

ErrorCode::ErrorCode(HRESULT hr)
	: hr(hr)
{
}

#pragma comment(lib, "DxErr.lib")

string2 ErrorCode::error_string() const
{
	//SCOPED_VAR(LPTSTR, buf, LocalFree);
	//DWORD res = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_FROM_HMODULE, 0, hr & 0xffff, 0, (LPTSTR)&buf, 0, NULL);
	return string2(DXGetErrorString(hr));
}


bool Graphics::create_back_buffers(int width, int height)
{
  _width = width;
  _height = height;

	GetCurrentThreadId()

  // release any existing buffers
  const bool existing_buffers = _back_buffer || _render_target_view;
  if (_back_buffer) _back_buffer = NULL;
  if (_render_target_view) _render_target_view = NULL;
  if (_depth_stencil) _depth_stencil = NULL;
  if (_depth_stencil_view) _depth_stencil_view = NULL;

  if (existing_buffers)
    _swap_chain->ResizeBuffers(1, width, height, _buffer_format, 0);

  // Get the dx11 back buffer
  RETURN_ON_FAIL_BOOL(_swap_chain->GetBuffer(0, IID_PPV_ARGS(&_back_buffer)), LOG_ERROR_LN);
  D3D11_TEXTURE2D_DESC back_buffer_desc;
  _back_buffer->GetDesc(&back_buffer_desc);

	ErrorCode e(_device->CreateRenderTargetView(_back_buffer, NULL, &_render_target_view));
	if (!e)
		return e;

  RETURN_ON_FAIL_BOOL(_device->CreateRenderTargetView(_back_buffer, NULL, &_render_target_view), LOG_ERROR_LN);

  // depth buffer
  D3D11_TEXTURE2D_DESC depthBufferDesc;
  depthBufferDesc.Width = width;
  depthBufferDesc.Height = height;
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize = 1;
  depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  RETURN_ON_FAIL_BOOL(_device->CreateTexture2D(&depthBufferDesc, NULL, &_depth_stencil), LOG_ERROR_LN);
  RETURN_ON_FAIL_BOOL(_device->CreateDepthStencilView(_depth_stencil, NULL, &_depth_stencil_view), LOG_ERROR_LN);

  _viewport = CD3D11_VIEWPORT (0.0f, 0.0f, (float)_width, (float)_height);

  set_default_render_target();

  return true;
}

bool Graphics::init_directx(const HWND hwnd, const int width, const int height)
{
	_width = width;
	_height = height;
	_buffer_format = DXGI_FORMAT_B8G8R8A8_UNORM; //DXGI_FORMAT_R8G8B8A8_UNORM;

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 2;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = _buffer_format;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	// Create DXGI factory to enumerate adapters
	CComPtr<IDXGIFactory1> dxgi_factory;
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&dxgi_factory)))
		return false;

	// Use the first adapter
	vector<CComPtr<IDXGIAdapter1> > adapters;
  UINT i = 0;
  IDXGIAdapter1 *adapter = nullptr;
  bool found_perfhud = false;
  while (!FAILED(dxgi_factory->EnumAdapters1(i++, &adapter))) {
    adapters.push_back(adapter);
    DXGI_ADAPTER_DESC desc;
    adapter->GetDesc(&desc);
    if (wcscmp(desc.Description, L"NVIDIA PerfHud") == 0) {
      found_perfhud = true;
    }
  }

  if (!found_perfhud)
    adapter = adapters.front();


	const int flags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED;

	// Create the DX11 device
	RETURN_ON_FAIL_BOOL(D3D11CreateDeviceAndSwapChain(
		adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, flags, NULL, 0, D3D11_SDK_VERSION, &sd, &_swap_chain, &_device, &_feature_level, &_immediate_context),
    LOG_ERROR_LN);

	if (_feature_level < D3D_FEATURE_LEVEL_9_3) {
    LOG_ERROR_LN("Card must support at least D3D_FEATURE_LEVEL_9_3");
		return false;
	}

  RETURN_ON_FAIL_BOOL(_device->QueryInterface(IID_ID3D11Debug, (void **)&_d3d_debug), LOG_ERROR_LN);

  create_back_buffers(width, height);

  _default_sampler_state.Attach(rt::D3D11::SamplerDescription().Create(_device));
  _default_rasterizer_state.Attach(rt::D3D11::RasterizerDescription().Create(_device));
  _default_depth_stencil_state.Attach(rt::D3D11::DepthStencilDescription().Create(_device));
  _default_blend_state.Attach(rt::D3D11::BlendDescription().Create(_device));
  for (int i = 0; i < 4; ++i)
    _default_blend_factors[i] = 1.0f;
	return true;
}

void Graphics::set_default_render_target()
{
	ID3D11RenderTargetView* render_targets[] = { _render_target_view };
	_immediate_context->OMSetRenderTargets(1, render_targets, _depth_stencil_view);
	_immediate_context->RSSetViewports(1, &_viewport);
}

bool Graphics::close()
{
  delete this;
  _instance = NULL;
  return true;
}

void Graphics::clear()
{
  clear(_clear_color);
}

void Graphics::clear(const D3DXCOLOR& c)
{
	_immediate_context->ClearRenderTargetView(_render_target_view, c);
	_immediate_context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0 );
}

void Graphics::present()
{
  const DWORD now = timeGetTime();
  if (_start_fps_time == 0xffffffff) {
    _start_fps_time = now;
  } else if (++_frame_count == 50) {
    _fps = 50.0f * 1000.0f / (timeGetTime() - _start_fps_time);
    _start_fps_time = now;
    _frame_count = 0;
  }

	_swap_chain->Present(0,0);
}

void Graphics::resize(const int width, const int height)
{
  if (!_swap_chain || width == 0 || height == 0)
    return;
  create_back_buffers(width, height);
}

void Graphics::tick()
{

}