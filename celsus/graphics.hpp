#ifndef _GRAPHICS_HPP_
#define _GRAPHICS_HPP_

#include "celsus.hpp"
#include <d3d11.h>
#include <D3DX10math.h>
#include <stdint.h>
#include <atlbase.h>

class Graphics
{
public:

	static Graphics& instance();

	bool init_directx(const HWND hwnd, const int width, const int height);
	bool close();
  void	clear(const D3DXCOLOR& c);
  void	clear();
  void  set_clear_color(const D3DXCOLOR& c) { _clear_color = c; }
	void	present();
  void  tick();
	void resize(const int width, const int height);

	ID3D11Device* device() { return _device; }
  ID3D11DeviceContext* context() { return _immediate_context; }

  const D3D11_VIEWPORT& viewport() const { return _viewport; }

	void set_default_render_target();

  D3D_FEATURE_LEVEL feature_level() const { return _feature_level; }

  ID3D11DepthStencilState *default_dss() const { return _default_dss; }
  uint32_t default_stencil_ref() const { return 0; }
  ID3D11BlendState *default_blend_state() const { return _default_blend_state; }
  const float *default_blend_factors() const { return _default_blend_factors; }
  uint32_t default_sample_mask() const { return 0xffffffff; }

  CComPtr<IDXGISwapChain>& swap_chain() { return _swap_chain; }

  float fps() const { return _fps; }
  int width() const { return _width; }
  int height() const { return _height; }

private:
	DISALLOW_COPY_AND_ASSIGN(Graphics);

	Graphics();
	~Graphics();

  bool create_back_buffers(int width, int height);
  bool init_dx10(CComPtr<IDXGIAdapter1>& adapter);

	static Graphics* _instance;
  static bool _deleted;

	int _width;
	int _height;
	D3D11_VIEWPORT _viewport;
	DXGI_FORMAT _buffer_format;
  D3D_FEATURE_LEVEL _feature_level;
	CComPtr<ID3D11Device> _device;
	CComPtr<ID3D10Device1> _device10;
	CComPtr<IDXGISwapChain> _swap_chain;
	CComPtr<ID3D11DeviceContext> _immediate_context;
	CComPtr<ID3D11RenderTargetView> _render_target_view;
	CComPtr<ID3D11Texture2D> _depth_stencil;
	CComPtr<ID3D11DepthStencilView> _depth_stencil_view;

	CComPtr<ID3D11Texture2D> _back_buffer;
	CComPtr<ID3D11ShaderResourceView> _shared_texture_view;
	CComPtr<ID3D11Texture2D> _shared_texture;
	CComPtr<IDXGIKeyedMutex> _keyed_mutex_10;
	CComPtr<IDXGIKeyedMutex> _keyed_mutex_11;

  CComPtr<ID3D11Debug> _d3d_debug;

  CComPtr<ID3D11DepthStencilState> _default_dss;
  float _default_blend_factors[4];
  CComPtr<ID3D11BlendState> _default_blend_state;

  D3DXCOLOR _clear_color;
  DWORD _start_fps_time;
  int32_t _frame_count;
  float _fps;
};

#endif
