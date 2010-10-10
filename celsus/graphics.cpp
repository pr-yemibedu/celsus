#include "stdafx.h"
#include "graphics.hpp"
#include "error2.hpp"
#include "Logger.hpp"
#include "D3D11Descriptions.hpp"

#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "D3D10_1.lib")
#pragma comment (lib, "D2D1.lib")
#pragma comment (lib, "DXErr.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "Dwrite.lib")

// this is ugly hax from when i was testing direct2d stuff..
/*
extern ID2D1RenderTarget *m_pBackBufferRT;
extern ID2D1SolidColorBrush *m_pBackBufferTextBrush;
extern IDWriteFactory *m_pDWriteFactory;
extern IDWriteTextFormat *m_pTextFormat;
*/
ID2D1RenderTarget *m_pBackBufferRT = NULL;
ID2D1SolidColorBrush *m_pBackBufferTextBrush = NULL;
IDWriteFactory *m_pDWriteFactory = NULL;
IDWriteTextFormat *m_pTextFormat = NULL;

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


bool Graphics::create_back_buffers(int width, int height)
{
  _width = width;
  _height = height;

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
	CComPtr<IDXGIAdapter1> adapter; 
	if (FAILED(dxgi_factory->EnumAdapters1(0, &adapter)))
		return false;

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

  _default_dss.Attach(rt::D3D11::DepthStencilDescription().Create(_device));
  _default_blend_state.Attach(rt::D3D11::BlendDescription().Create(_device));
  for (int i = 0; i < 4; ++i)
    _default_blend_factors[i] = 1.0f;
/*
  if (!init_dx10(adapter))
    return false;
*/
	return true;
}

bool Graphics::init_dx10(CComPtr<IDXGIAdapter1>& adapter)
{
  // We need feature level dx 10 to be able to use direct2d
  if (_feature_level >= D3D_FEATURE_LEVEL_10_0) {
    // Create the DX10 device
    if (FAILED(D3D10CreateDevice1(
      adapter,
      D3D10_DRIVER_TYPE_HARDWARE,
      NULL,
      D3D10_CREATE_DEVICE_DEBUG | D3D10_CREATE_DEVICE_BGRA_SUPPORT  | D3D11_CREATE_DEVICE_SINGLETHREADED,
      D3D10_FEATURE_LEVEL_9_3,
      D3D10_1_SDK_VERSION,
      &_device10)))
      return false;

    // Create the shared texture to draw D2D content to
    D3D11_TEXTURE2D_DESC sharedTextureDesc;
    D3D11_TEXTURE2D_DESC back_buffer_desc;
    _back_buffer->GetDesc(&back_buffer_desc);

    ZeroMemory(&sharedTextureDesc, sizeof(sharedTextureDesc));
    sharedTextureDesc.Width = back_buffer_desc.Width;
    sharedTextureDesc.Height = back_buffer_desc.Height;
    sharedTextureDesc.MipLevels = 1;
    sharedTextureDesc.ArraySize = 1;
    sharedTextureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sharedTextureDesc.SampleDesc.Count = 1;
    sharedTextureDesc.Usage = D3D11_USAGE_DEFAULT;
    sharedTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    sharedTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;

    HRESULT hResult = S_OK;
    hResult = _device->CreateTexture2D(&sharedTextureDesc, NULL, &_shared_texture);
    hResult = _device->CreateShaderResourceView(_shared_texture, NULL, &_shared_texture_view);

    // Get the keyed mutex for the shared texture (for D3D11)
    hResult = _shared_texture->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&_keyed_mutex_11);

    // Get the shared handle needed to open the shared texture in D3D10.1
    CComPtr<IDXGIResource> pSharedResource11;
    HANDLE hSharedHandle11;
    hResult = _shared_texture->QueryInterface(__uuidof(IDXGIResource), (void**)&pSharedResource11);
    hResult = pSharedResource11->GetSharedHandle(&hSharedHandle11);


    // Open the surface for the shared texture in D3D10.1
    CComPtr<IDXGISurface1> pSharedSurface10;
    hResult = _device10->OpenSharedResource(hSharedHandle11, __uuidof(IDXGISurface1), (void**)&pSharedSurface10);

    // Get the keyed mutex for the shared texture (for D3D10.1)
    hResult = pSharedSurface10->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)&_keyed_mutex_10);

    CComPtr<ID2D1Factory> pD2DFactory;
    hResult = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&pD2DFactory);

    // Create D2D render target from the surface for the shared texture, which was opened in D3D10.1
    D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties;
    //CComPtr<ID2D1RenderTarget> pD2DRenderTarget;
    ZeroMemory(&renderTargetProperties, sizeof(renderTargetProperties));
    renderTargetProperties.type = D2D1_RENDER_TARGET_TYPE_HARDWARE;
    renderTargetProperties.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED);
    hResult = pD2DFactory->CreateDxgiSurfaceRenderTarget(pSharedSurface10, &renderTargetProperties, &m_pBackBufferRT);

    //CComPtr<ID2D1SolidColorBrush> pBrush;
    hResult = m_pBackBufferRT->CreateSolidColorBrush(D2D1::ColorF(1.0f, 1.0f, 0.0f, 1.0f), &m_pBackBufferTextBrush);

    // Create DWrite factory
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pDWriteFactory), reinterpret_cast<IUnknown **>(&m_pDWriteFactory) );
    static const WCHAR msc_fontName[] = L"Verdana";
    static const FLOAT msc_fontSize = 50;

    // Create DWrite text format object
    m_pDWriteFactory->CreateTextFormat(
      msc_fontName,
      NULL,
      DWRITE_FONT_WEIGHT_NORMAL,
      DWRITE_FONT_STYLE_NORMAL,
      DWRITE_FONT_STRETCH_NORMAL,
      msc_fontSize,
      L"", //locale
      &m_pTextFormat
      );
  }

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