#ifndef D3DTYPES_HPP
#define D3DTYPES_HPP

#include <comip.h>
#include <D3D10.h>
#include <D3DX10.h>

// Macro to create typedefs for COM smart ptrs
#define D3D10_PTR(x) typedef _com_ptr_t<_com_IIID<x, &__uuidof(x)> > x ## Ptr;

D3D10_PTR(ID3D10Device);
D3D10_PTR(ID3D10Texture2D);
D3D10_PTR(IDXGISwapChain);
D3D10_PTR(ID3D10RenderTargetView);
D3D10_PTR(ID3D10DepthStencilView);
D3D10_PTR(ID3D10ShaderResourceView);
D3D10_PTR(ID3D10InputLayout);
D3D10_PTR(ID3D10Buffer);
typedef _com_ptr_t< _com_IIID<ID3D10Effect, &IID_ID3D10Effect> > ID3D10EffectPtr;
typedef _com_ptr_t< _com_IIID<ID3DX10Font, &IID_ID3DX10Font> > ID3DX10FontPtr;
D3D10_PTR(ID3D10BlendState);
D3D10_PTR(ID3D10DepthStencilState);

#endif
