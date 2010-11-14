#if !defined(D3D11_HELPERS_H)
#define D3D11_HELPERS_H

// from http://legalizeadulthood.wordpress.com/2009/07/12/description-helpers-for-direct3d-10-10-1-and-11/

#include <cassert>
#include <d3d11.h>
//#include <rt/Exceptions/COMException.h>

//#define NULL_ON_FAIL(expr_) rt::COMException::ThrowHResult(expr_, _T(#expr_), _T(__FILE__), __LINE__)
#define NULL_ON_FAIL(expr) if (FAILED(expr)) return NULL;

namespace rt { 

namespace D3D11
{
#define D3D11_HELPER_STRUCT_SETTER(structType_, methodName_, valueType_, structMember_)	\
	structType_ &methodName_(valueType_ value)											\
	{																					\
		structMember_ = value;															\
		return *this;																	\
	}
#define D3D11_HELPER_SETTER_DESC(structType_, methodName_, structMember_, interfaceType_, descType_, descMember_) \
	structType_ &methodName_(interfaceType_ *texture)	\
	{													\
		descType_ desc;									\
		texture->GetDesc(&desc);						\
		structMember_ = desc.descMember_;				\
		return *this;									\
	}

	struct DepthStencilOpDescription : public D3D11_DEPTH_STENCILOP_DESC
	{
#define DEPTH_STENCIL_OP_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(DepthStencilOpDescription, structMember_##_, valueType_, structMember_)

		explicit DepthStencilOpDescription()
			: D3D11_DEPTH_STENCILOP_DESC()
		{
			StencilFailOp = D3D11_STENCIL_OP_KEEP;
			StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			StencilPassOp = D3D11_STENCIL_OP_KEEP;
			StencilFunc = D3D11_COMPARISON_ALWAYS;
		}
		DEPTH_STENCIL_OP_DESCRIPTION_SETTER(D3D11_STENCIL_OP, StencilFailOp)
		DEPTH_STENCIL_OP_DESCRIPTION_SETTER(D3D11_STENCIL_OP, StencilDepthFailOp)
		DEPTH_STENCIL_OP_DESCRIPTION_SETTER(D3D11_STENCIL_OP, StencilPassOp)
		DEPTH_STENCIL_OP_DESCRIPTION_SETTER(D3D11_COMPARISON_FUNC, StencilFunc)

#undef DEPTH_STENCIL_OP_DESCRIPTION_SETTER
	};

	struct DepthStencilDescription : public CD3D11_DEPTH_STENCIL_DESC
	{
#define DEPTH_STENCIL_DESCRIPTION_SETTER(methodName_, valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(DepthStencilDescription, methodName_, valueType_, structMember_)

		explicit DepthStencilDescription() : CD3D11_DEPTH_STENCIL_DESC(D3D11_DEFAULT)
		{}
		DEPTH_STENCIL_DESCRIPTION_SETTER(DepthEnable_, BOOL, DepthEnable)
		DEPTH_STENCIL_DESCRIPTION_SETTER(DepthWriteMask_, D3D11_DEPTH_WRITE_MASK, DepthWriteMask)
		DEPTH_STENCIL_DESCRIPTION_SETTER(DepthFunc_, D3D11_COMPARISON_FUNC, DepthFunc)
		DEPTH_STENCIL_DESCRIPTION_SETTER(StencilEnable_, BOOL, StencilEnable)
		DEPTH_STENCIL_DESCRIPTION_SETTER(StencilReadMask_, UINT8, StencilReadMask)
		DEPTH_STENCIL_DESCRIPTION_SETTER(StencilWriteMask_, UINT8, StencilWriteMask)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontFace_, D3D11_DEPTH_STENCILOP_DESC const &, FrontFace)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontStencilFailOp_, D3D11_STENCIL_OP, FrontFace.StencilFailOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontStencilDepthFailOp_, D3D11_STENCIL_OP, FrontFace.StencilDepthFailOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontStencilPassOp_, D3D11_STENCIL_OP, FrontFace.StencilPassOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontStencilFunc_, D3D11_COMPARISON_FUNC, FrontFace.StencilFunc)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackFace_, D3D11_DEPTH_STENCILOP_DESC const &, BackFace)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackStencilFailOp_, D3D11_STENCIL_OP, BackFace.StencilFailOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackStencilDepthFailOp_, D3D11_STENCIL_OP, BackFace.StencilDepthFailOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackStencilPassOp_, D3D11_STENCIL_OP, BackFace.StencilPassOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackStencilFunc_, D3D11_COMPARISON_FUNC, BackFace.StencilFunc)

		ID3D11DepthStencilState *Create(ID3D11Device *device)
		{
			ID3D11DepthStencilState *result = 0;
			NULL_ON_FAIL(device->CreateDepthStencilState(this, &result));
			return result;
		}

#undef DEPTH_STENCIL_DESCRIPTION_SETTER
	};

	struct RenderTargetBlendDescription : public D3D11_RENDER_TARGET_BLEND_DESC
	{
#define RENDER_TARGET_BLEND_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(RenderTargetBlendDescription, structMember_##_, valueType_, structMember_)

		explicit RenderTargetBlendDescription() : D3D11_RENDER_TARGET_BLEND_DESC()
		{
			BlendEnable = FALSE;
			SrcBlend = D3D11_BLEND_ONE;
			DestBlend = D3D11_BLEND_ZERO;
			BlendOp = D3D11_BLEND_OP_ADD;
			SrcBlendAlpha = D3D11_BLEND_ONE;
			DestBlendAlpha = D3D11_BLEND_ZERO;
			BlendOpAlpha = D3D11_BLEND_OP_ADD;
			RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		RENDER_TARGET_BLEND_DESCRIPTION_SETTER(BOOL, BlendEnable)
		RENDER_TARGET_BLEND_DESCRIPTION_SETTER(D3D11_BLEND, SrcBlend)
		RENDER_TARGET_BLEND_DESCRIPTION_SETTER(D3D11_BLEND, DestBlend)
		RENDER_TARGET_BLEND_DESCRIPTION_SETTER(D3D11_BLEND_OP, BlendOp)
		RENDER_TARGET_BLEND_DESCRIPTION_SETTER(D3D11_BLEND, SrcBlendAlpha)
		RENDER_TARGET_BLEND_DESCRIPTION_SETTER(D3D11_BLEND, DestBlendAlpha)
		RENDER_TARGET_BLEND_DESCRIPTION_SETTER(D3D11_BLEND_OP, BlendOpAlpha)
		RENDER_TARGET_BLEND_DESCRIPTION_SETTER(UINT8, RenderTargetWriteMask)

#undef RENDER_TARGET_BLEND_DESCRIPTION_SETTER
	};

	struct BlendDescription : public CD3D11_BLEND_DESC
	{
#define BLEND_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(BlendDescription, structMember_##_, valueType_, structMember_)

		explicit BlendDescription()
			: CD3D11_BLEND_DESC(D3D11_DEFAULT)
		{
		}
		BLEND_DESCRIPTION_SETTER(BOOL, AlphaToCoverageEnable)
		BLEND_DESCRIPTION_SETTER(BOOL, IndependentBlendEnable)
		BlendDescription &RenderTarget_(UINT index, D3D11_RENDER_TARGET_BLEND_DESC const &value)
		{
			RenderTarget[index] = value;
			return *this;
		}

		ID3D11BlendState *Create(ID3D11Device *device)
		{
			ID3D11BlendState *result = 0;
			NULL_ON_FAIL(device->CreateBlendState(this, &result));
			return result;
		}

#undef BLEND_DESCRIPTION_SETTER
	};

	struct RasterizerDescription : public CD3D11_RASTERIZER_DESC
	{
#define RASTERIZER_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(RasterizerDescription, structMember_##_, valueType_, structMember_)

		explicit RasterizerDescription()
			: CD3D11_RASTERIZER_DESC(D3D11_DEFAULT)
		{
		}
		RASTERIZER_DESCRIPTION_SETTER(D3D11_FILL_MODE, FillMode)
		RASTERIZER_DESCRIPTION_SETTER(D3D11_CULL_MODE, CullMode)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, FrontCounterClockwise)
		RASTERIZER_DESCRIPTION_SETTER(int, DepthBias)
		RASTERIZER_DESCRIPTION_SETTER(float, DepthBiasClamp)
		RASTERIZER_DESCRIPTION_SETTER(float, SlopeScaledDepthBias)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, DepthClipEnable)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, ScissorEnable)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, MultisampleEnable)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, AntialiasedLineEnable)

		ID3D11RasterizerState *Create(ID3D11Device *device)
		{
			ID3D11RasterizerState *result = 0;
			NULL_ON_FAIL(device->CreateRasterizerState(this, &result));
			return result;
		}

#undef RASTERIZER_DESCRIPTION_SETTER
	};

	struct BufferDescription : public CD3D11_BUFFER_DESC
	{
#define BUFFER_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(BufferDescription, structMember_##_, valueType_, structMember_)

		explicit BufferDescription(UINT byteWidth, UINT bindFlags)
			: CD3D11_BUFFER_DESC(byteWidth, bindFlags)
		{
		}
		BUFFER_DESCRIPTION_SETTER(D3D11_USAGE, Usage)
		BUFFER_DESCRIPTION_SETTER(UINT, CPUAccessFlags)
		BUFFER_DESCRIPTION_SETTER(UINT, MiscFlags)
		BUFFER_DESCRIPTION_SETTER(UINT, StructureByteStride)

		ID3D11Buffer *Create(ID3D11Device *device, D3D11_SUBRESOURCE_DATA const *data = 0)
		{
			assert(data || (Usage != D3D11_USAGE_IMMUTABLE));
			ID3D11Buffer *result = 0;
			NULL_ON_FAIL(device->CreateBuffer(this, data, &result));
			return result;
		}

#undef BUFFER_DESCRIPTION_SETTER
	};

	struct Texture1DDescription : public CD3D11_TEXTURE1D_DESC
	{
#define TEXTURE1D_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(Texture1DDescription, structMember_##_, valueType_, structMember_)

		explicit Texture1DDescription(DXGI_FORMAT format, UINT width)
			: CD3D11_TEXTURE1D_DESC(format, width)
		{
		}
		TEXTURE1D_DESCRIPTION_SETTER(UINT, ArraySize)
		TEXTURE1D_DESCRIPTION_SETTER(UINT, MipLevels)
		TEXTURE1D_DESCRIPTION_SETTER(UINT, BindFlags)
		TEXTURE1D_DESCRIPTION_SETTER(D3D11_USAGE, Usage)
		TEXTURE1D_DESCRIPTION_SETTER(UINT, CPUAccessFlags)
		TEXTURE1D_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D11Texture1D *Create(ID3D11Device *device, D3D11_SUBRESOURCE_DATA const *data = 0)
		{
			assert(data || (Usage != D3D11_USAGE_IMMUTABLE));
			ID3D11Texture1D *result = 0;
			NULL_ON_FAIL(device->CreateTexture1D(this, data, &result));
			return result;
		}

#undef TEXTURE1D_DESCRIPTION_SETTER
	};

	struct Texture2DDescription : public CD3D11_TEXTURE2D_DESC
	{
#define TEXTURE2D_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(Texture2DDescription, structMember_##_, valueType_, structMember_)

		explicit Texture2DDescription(DXGI_FORMAT format, UINT width, UINT height)
			: CD3D11_TEXTURE2D_DESC(format, width, height)
		{
		}
		TEXTURE2D_DESCRIPTION_SETTER(UINT, ArraySize)
		TEXTURE2D_DESCRIPTION_SETTER(UINT, MipLevels)
		TEXTURE2D_DESCRIPTION_SETTER(UINT, BindFlags)
		TEXTURE2D_DESCRIPTION_SETTER(D3D11_USAGE, Usage)
		TEXTURE2D_DESCRIPTION_SETTER(UINT, CPUAccessFlags)
		D3D11_HELPER_STRUCT_SETTER(Texture2DDescription, SampleCount_, UINT, SampleDesc.Count)
		D3D11_HELPER_STRUCT_SETTER(Texture2DDescription, SampleQuality_, UINT, SampleDesc.Quality)
		TEXTURE2D_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D11Texture2D *Create(ID3D11Device *device, D3D11_SUBRESOURCE_DATA const *data = 0)
		{
			assert(data || (Usage != D3D11_USAGE_IMMUTABLE));
			ID3D11Texture2D *result = 0;
			NULL_ON_FAIL(device->CreateTexture2D(this, data, &result));
			return result;
		}

#undef TEXTURE2D_DESCRIPTION_SETTER
	};

	struct Texture3DDescription : public CD3D11_TEXTURE3D_DESC
	{
#define TEXTURE3D_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(Texture3DDescription, structMember_##_, valueType_, structMember_)

		explicit Texture3DDescription(DXGI_FORMAT format, UINT width, UINT height, UINT depth)
			: CD3D11_TEXTURE3D_DESC(format, width, height, depth)
		{
		}
		TEXTURE3D_DESCRIPTION_SETTER(UINT, MipLevels)
		TEXTURE3D_DESCRIPTION_SETTER(UINT, BindFlags)
		TEXTURE3D_DESCRIPTION_SETTER(D3D11_USAGE, Usage)
		TEXTURE3D_DESCRIPTION_SETTER(UINT, CPUAccessFlags)
		TEXTURE3D_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D11Texture3D *Create(ID3D11Device *device, D3D11_SUBRESOURCE_DATA const *data = 0)
		{
			assert(data || (Usage != D3D11_USAGE_IMMUTABLE));
			ID3D11Texture3D *result = 0;
			NULL_ON_FAIL(device->CreateTexture3D(this, data, &result));
			return result;
		}

#undef TEXTURE3D_DESCRIPTION_SETTER
	};

	struct ShaderResourceViewDescription : public CD3D11_SHADER_RESOURCE_VIEW_DESC
	{
#define SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(ShaderResourceViewDescription, \
		structMember_##_, valueType_, structMember_)
#define SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_) \
	D3D11_HELPER_SETTER_DESC(ShaderResourceViewDescription, \
		structMember_##_, structMember_, interfaceType_, descType_, structMember_)

		explicit ShaderResourceViewDescription(D3D11_SRV_DIMENSION viewDimension)
			: CD3D11_SHADER_RESOURCE_VIEW_DESC(viewDimension)
		{
		}
		SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER(DXGI_FORMAT, Format)
		SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture1D, D3D11_TEXTURE1D_DESC)
		SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture2D, D3D11_TEXTURE2D_DESC)
		SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture3D, D3D11_TEXTURE3D_DESC)

		ID3D11ShaderResourceView *Create(ID3D11Device *device, ID3D11Resource *resource)
		{
			ID3D11ShaderResourceView *result = 0;
			NULL_ON_FAIL(device->CreateShaderResourceView(resource, this, &result));
			return result;
		}

#undef SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER
#undef SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderBufferViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_BUFFER_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(ShaderBufferViewDescription, structMember_##_, valueType_, Buffer.structMember_)

		explicit ShaderBufferViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_BUFFER)
		{
		}
		SHADER_BUFFER_VIEW_DESCRIPTION_SETTER(UINT, FirstElement)
		SHADER_BUFFER_VIEW_DESCRIPTION_SETTER(UINT, ElementOffset)
		SHADER_BUFFER_VIEW_DESCRIPTION_SETTER(UINT, NumElements)
		SHADER_BUFFER_VIEW_DESCRIPTION_SETTER(UINT, ElementWidth)

#undef SHADER_BUFFER_VIEW_DESCRIPTION_SETTER
	};

	struct ShaderTexture1DViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(ShaderTexture1DViewDescription, \
		structMember_##_, valueType_, Texture1D.structMember_)
#define SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(ShaderTexture1DViewDescription, \
		structMember_##_, Texture1D.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture1DViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_TEXTURE1D)
		{
		}
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, MipLevels - Texture1D.MostDetailedMip)
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, MipLevels - Texture1D.MostDetailedMip)
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, MipLevels - Texture1D.MostDetailedMip)

#undef SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture1DArrayViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(ShaderTexture1DArrayViewDescription,					\
		structMember_##_, valueType_, Texture1DArray.structMember_)
#define SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(ShaderTexture1DArrayViewDescription, \
		structMember_##_, Texture1DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture1DArrayViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_TEXTURE1DARRAY)
		{
		}
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, MipLevels - Texture1DArray.MostDetailedMip)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, MipLevels - Texture1DArray.MostDetailedMip)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, MipLevels - Texture1DArray.MostDetailedMip)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture1DArray.FirstArraySlice)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture1DArray.FirstArraySlice)

#undef SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture2DArrayViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(ShaderTexture2DArrayViewDescription,					\
		structMember_##_, valueType_, Texture2DArray.structMember_)
#define SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(ShaderTexture2DArrayViewDescription, \
		structMember_##_, Texture2DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture2DArrayViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_TEXTURE2DARRAY)
		{
		}
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, MipLevels - Texture2DArray.MostDetailedMip)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, MipLevels - Texture2DArray.MostDetailedMip)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, MipLevels - Texture2DArray.MostDetailedMip)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture2DArray.FirstArraySlice)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture2DArray.FirstArraySlice)

#undef SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture2DMultiSampleArrayViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(ShaderTexture2DMultiSampleArrayViewDescription,					\
		structMember_##_, valueType_, Texture2DMSArray.structMember_)
#define SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(ShaderTexture2DMultiSampleArrayViewDescription, \
		structMember_##_, Texture2DMSArray.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture2DMultiSampleArrayViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY)
		{
		}
		SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)
		SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)

#undef SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture2DMultiSampleViewDescription : public ShaderResourceViewDescription
	{
		explicit ShaderTexture2DMultiSampleViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_TEXTURE2DMS)
		{
		}
	};

	struct ShaderTexture2DViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(ShaderTexture2DViewDescription, \
		structMember_##_, valueType_, Texture2D.structMember_)
#define SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(ShaderTexture2DViewDescription, \
		structMember_##_, Texture2D.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture2DViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_TEXTURE2D)
		{
		}
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, MipLevels - Texture2D.MostDetailedMip)
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, MipLevels - Texture2D.MostDetailedMip)
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, MipLevels - Texture2D.MostDetailedMip)

#undef SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture3DViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(ShaderTexture3DViewDescription, \
		structMember_##_, valueType_, Texture3D.structMember_)
#define SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(ShaderTexture3DViewDescription, \
		structMember_##_, Texture3D.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture3DViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_TEXTURE3D)
		{
		}
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, MipLevels - Texture3D.MostDetailedMip)
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, MipLevels - Texture3D.MostDetailedMip)
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, MipLevels - Texture3D.MostDetailedMip)

#undef SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTextureCubeArrayViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(ShaderTextureCubeArrayViewDescription, \
		structMember_##_, valueType_, TextureCubeArray.structMember_)
#define SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(ShaderTextureCubeArrayViewDescription, \
		structMember_##_, TextureCubeArray.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTextureCubeArrayViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_TEXTURECUBEARRAY)
		{
		}
		SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, First2DArrayFace)
		SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, NumCubes)
		SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, MipLevels - TextureCubeArray.MostDetailedMip)
		SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, MipLevels - TextureCubeArray.MostDetailedMip)
		SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, MipLevels - TextureCubeArray.MostDetailedMip)

#undef SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE_CUBE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTextureCubeViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(ShaderTextureCubeViewDescription, \
		structMember_##_, valueType_, TextureCube.structMember_)
#define SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(ShaderTextureCubeViewDescription, \
		structMember_##_, TextureCube.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTextureCubeViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_TEXTURECUBE)
		{
		}
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, MipLevels - TextureCube.MostDetailedMip)
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, MipLevels - TextureCube.MostDetailedMip)
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D11Texture3D, D3D11_TEXTURE3D_DESC, MipLevels - TextureCube.MostDetailedMip)

#undef SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderBufferExViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_BUFFER_EX_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(ShaderBufferExViewDescription, structMember_##_, valueType_, BufferEx.structMember_)

		explicit ShaderBufferExViewDescription()
			: ShaderResourceViewDescription(D3D11_SRV_DIMENSION_BUFFEREX)
		{
		}
        SHADER_BUFFER_EX_VIEW_DESCRIPTION_SETTER(UINT, FirstElement)
        SHADER_BUFFER_EX_VIEW_DESCRIPTION_SETTER(UINT, NumElements)
        SHADER_BUFFER_EX_VIEW_DESCRIPTION_SETTER(UINT, Flags)

#undef SHADER_BUFFER_EX_VIEW_DESCRIPTION_SETTER
	};

	struct RenderTargetViewDescription : public CD3D11_RENDER_TARGET_VIEW_DESC
	{
#define RENDER_TARGET_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(RenderTargetViewDescription, structMember_##_, valueType_, structMember_)
#define RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_) \
	D3D11_HELPER_SETTER_DESC(RenderTargetViewDescription, \
		structMember_##_, structMember_, interfaceType_, descType_, structMember_)

		explicit RenderTargetViewDescription(D3D11_RTV_DIMENSION viewDimension)
			: CD3D11_RENDER_TARGET_VIEW_DESC(viewDimension)
		{
		}
		RENDER_TARGET_VIEW_DESCRIPTION_SETTER(DXGI_FORMAT, Format)
		RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture1D, D3D11_TEXTURE1D_DESC)
		RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture2D, D3D11_TEXTURE2D_DESC)
		RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture3D, D3D11_TEXTURE3D_DESC)

		ID3D11RenderTargetView *Create(ID3D11Device *device, ID3D11Resource *resource)
		{
			ID3D11RenderTargetView *result = 0;
			NULL_ON_FAIL(device->CreateRenderTargetView(resource, this, &result));
			return result;
		}

#undef RENDER_TARGET_VIEW_DESCRIPTION_SETTER
#undef RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct RenderTargetBufferViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_BUFFER_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(RenderTargetBufferViewDescription, structMember_##_, valueType_, Buffer.structMember_)

		explicit RenderTargetBufferViewDescription()
			: RenderTargetViewDescription(D3D11_RTV_DIMENSION_BUFFER)
		{
		}
		RENDER_TARGET_BUFFER_VIEW_DESCRIPTION_SETTER(UINT, FirstElement)
		RENDER_TARGET_BUFFER_VIEW_DESCRIPTION_SETTER(UINT, NumElements)

#undef RENDER_TARGET_BUFFER_VIEW_DESCRIPTION_SETTER
	};

	struct RenderTargetTexture1DViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE1D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(RenderTargetTexture1DViewDescription,				\
		structMember_##_, valueType_, Texture1D.structMember_)

		explicit RenderTargetTexture1DViewDescription()
			: RenderTargetViewDescription(D3D11_RTV_DIMENSION_TEXTURE1D)
		{
		}
		RENDER_TARGET_TEXTURE1D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)

#undef RENDER_TARGET_TEXTURE1D_VIEW_DESCRIPTION_SETTER
	};

	struct RenderTargetTexture1DArrayViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(RenderTargetTexture1DArrayViewDescription,					\
		structMember_##_, valueType_, Texture1DArray.structMember_)
#define RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(RenderTargetViewDescription, \
		structMember_##_, Texture1DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit RenderTargetTexture1DArrayViewDescription()
			: RenderTargetViewDescription(D3D11_RTV_DIMENSION_TEXTURE1DARRAY)
		{
		}
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture1DArray.FirstArraySlice)
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture1DArray.FirstArraySlice)

#undef RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct RenderTargetTexture2DViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE2D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(RenderTargetTexture2DViewDescription,				\
		structMember_##_, valueType_, Texture2D.structMember_)

		explicit RenderTargetTexture2DViewDescription()
			: RenderTargetViewDescription(D3D11_RTV_DIMENSION_TEXTURE2D)
		{
		}
		RENDER_TARGET_TEXTURE2D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)

#undef RENDER_TARGET_TEXTURE2D_VIEW_DESCRIPTION_SETTER
	};

	struct RenderTargetTexture2DArrayViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(RenderTargetTexture2DArrayViewDescription,					\
		structMember_##_, valueType_, Texture2DArray.structMember_)
#define RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(RenderTargetTexture2DArrayViewDescription, \
		structMember_##_, Texture2DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit RenderTargetTexture2DArrayViewDescription()
			: RenderTargetViewDescription(D3D11_RTV_DIMENSION_TEXTURE2DARRAY)
		{
		}
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture2DArray.FirstArraySlice)
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture2DArray.FirstArraySlice)

#undef RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct RenderTargetTexture2DMultiSampleViewDescription : public RenderTargetViewDescription
	{
		explicit RenderTargetTexture2DMultiSampleViewDescription()
			: RenderTargetViewDescription(D3D11_RTV_DIMENSION_TEXTURE2DMS)
		{
		}
	};

	struct RenderTargetTexture2DMultiSampleArrayViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(RenderTargetTexture2DMultiSampleArrayViewDescription, structMember_##_,	\
		valueType_, Texture2DMSArray.structMember_)
#define RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(RenderTargetTexture2DMultiSampleArrayViewDescription, \
		structMember_##_, Texture2DMSArray.structMember_, interfaceType_, descType_, descMember_)

		explicit RenderTargetTexture2DMultiSampleArrayViewDescription()
			: RenderTargetViewDescription(D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY)
		{
		}
		RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)
		RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)

#undef RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER
#undef RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct RenderTargetTexture3DViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)		\
	D3D11_HELPER_STRUCT_SETTER(RenderTargetTexture3DViewDescription, structMember_##_,	\
		valueType_, Texture3D.structMember_)

		explicit RenderTargetTexture3DViewDescription()
			: RenderTargetViewDescription(D3D11_RTV_DIMENSION_TEXTURE3D)
		{
		}
		RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, FirstWSlice)
		RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, WSize)

#undef RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER
	};

	struct DepthStencilViewDescription : public CD3D11_DEPTH_STENCIL_VIEW_DESC
	{
#define DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(DepthStencilViewDescription, \
		structMember_##_, valueType_, structMember_)
#define DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_) \
	D3D11_HELPER_SETTER_DESC(DepthStencilViewDescription, \
		structMember_##_, structMember_, interfaceType_, descType_, structMember_)

		explicit DepthStencilViewDescription(D3D11_DSV_DIMENSION viewDimension)
			: CD3D11_DEPTH_STENCIL_VIEW_DESC(viewDimension)
		{
		}
		DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER(DXGI_FORMAT, Format)
		DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER(UINT, Flags)
		DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture1D, D3D11_TEXTURE1D_DESC)
		DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture2D, D3D11_TEXTURE2D_DESC)
		DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture3D, D3D11_TEXTURE3D_DESC)

		ID3D11DepthStencilView *Create(ID3D11Device *device, ID3D11Resource *resource)
		{
			ID3D11DepthStencilView *result = 0;
			NULL_ON_FAIL(device->CreateDepthStencilView(resource, this, &result));
			return result;
		}

#undef DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER
#undef DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct DepthStencilTexture1DViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE1D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(DepthStencilTexture1DViewDescription, structMember_##_, valueType_, Texture1D.structMember_)

		explicit DepthStencilTexture1DViewDescription()
			: DepthStencilViewDescription(D3D11_DSV_DIMENSION_TEXTURE1D)
		{
		}
		DEPTH_STENCIL_TEXTURE1D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)

#undef DEPTH_STENCIL_TEXTURE1D_VIEW_DESCRIPTION_SETTER
	};

	struct DepthStencilTexture1DArrayViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(DepthStencilTexture1DArrayViewDescription, \
		structMember_##_, valueType_, Texture1DArray.structMember_)
#define DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(DepthStencilTexture1DArrayViewDescription, \
		structMember_##_, Texture1DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit DepthStencilTexture1DArrayViewDescription()
			: DepthStencilViewDescription(D3D11_DSV_DIMENSION_TEXTURE1DARRAY)
		{
		}
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture1DArray.FirstArraySlice)
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture1DArray.FirstArraySlice)

#undef DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct DepthStencilTexture2DViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE2D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(DepthStencilTexture2DViewDescription, structMember_##_, valueType_, Texture2D.structMember_)

		explicit DepthStencilTexture2DViewDescription()
			: DepthStencilViewDescription(D3D11_DSV_DIMENSION_TEXTURE2D)
		{
		}
		DEPTH_STENCIL_TEXTURE2D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)

#undef DEPTH_STENCIL_TEXTURE2D_VIEW_DESCRIPTION_SETTER
	};

	struct DepthStencilTexture2DArrayViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(DepthStencilTexture2DArrayViewDescription, \
		structMember_##_, valueType_, Texture2DArray.structMember_)
#define DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(DepthStencilTexture2DArrayViewDescription, \
		structMember_##_, Texture2DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit DepthStencilTexture2DArrayViewDescription()
			: DepthStencilViewDescription(D3D11_DSV_DIMENSION_TEXTURE2DARRAY)
		{
		}
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture2DArray.FirstArraySlice)
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture2DArray.FirstArraySlice)

#undef DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct DepthStencilTexture2DMultiSampleViewDescription : public DepthStencilViewDescription
	{
		explicit DepthStencilTexture2DMultiSampleViewDescription()
			: DepthStencilViewDescription(D3D11_DSV_DIMENSION_TEXTURE2DMS)
		{
		}
	};

	struct DepthStencilTexture2DMultiSampleArrayViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(DepthStencilTexture2DMultiSampleArrayViewDescription, \
		structMember_##_, valueType_, Texture2DMSArray.structMember_)
#define DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(DepthStencilTexture2DMultiSampleArrayViewDescription, \
		structMember_##_, Texture2DMSArray.structMember_, interfaceType_, descType_, descMember_)

		explicit DepthStencilTexture2DMultiSampleArrayViewDescription()
			: DepthStencilViewDescription(D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY)
		{
		}
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)

#undef DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER
#undef DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct UnorderedAccessViewDescription : public CD3D11_UNORDERED_ACCESS_VIEW_DESC
	{
#define UNORDERED_ACCESS_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(UnorderedAccessViewDescription, \
		structMember_##_, valueType_, structMember_)
#define UNORDERED_ACCESS_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_) \
	D3D11_HELPER_SETTER_DESC(UnorderedAccessViewDescription, \
		structMember_##_, structMember_, interfaceType_, descType_, structMember_)

		explicit UnorderedAccessViewDescription(D3D11_UAV_DIMENSION viewDimension)
			: CD3D11_UNORDERED_ACCESS_VIEW_DESC(viewDimension)
		{
		}
		UNORDERED_ACCESS_VIEW_DESCRIPTION_SETTER(DXGI_FORMAT, Format)
		UNORDERED_ACCESS_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture1D, D3D11_TEXTURE1D_DESC)
		UNORDERED_ACCESS_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture2D, D3D11_TEXTURE2D_DESC)
		UNORDERED_ACCESS_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D11Texture3D, D3D11_TEXTURE3D_DESC)

		ID3D11UnorderedAccessView *Create(ID3D11Device *device, ID3D11Resource *resource)
		{
			ID3D11UnorderedAccessView *result = 0;
			NULL_ON_FAIL(device->CreateUnorderedAccessView(resource, this, &result));
			return result;
		}

#undef UNORDERED_ACCESS_VIEW_DESCRIPTION_SETTER
#undef UNORDERED_ACCESS_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct UnorderedTexture1DViewDescription : public UnorderedAccessViewDescription
	{
#define UNORDERED_TEXTURE1D_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(UnorderedTexture1DViewDescription,			\
		structMember_##_, valueType_, Texture1D.structMember_)

		explicit UnorderedTexture1DViewDescription()
			: UnorderedAccessViewDescription(D3D11_UAV_DIMENSION_TEXTURE1D)
		{
		}
		UNORDERED_TEXTURE1D_DESCRIPTION_SETTER(UINT, MipSlice)

#undef UNORDERED_TEXTURE1D_DESCRIPTION_SETTER
	};

	struct UnorderedTexture1DArrayViewDescription : public UnorderedAccessViewDescription
	{
#define UNORDERED_TEXTURE1D_ARRAY_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(UnorderedTexture1DArrayViewDescription,			\
		structMember_##_, valueType_, Texture1DArray.structMember_)
#define UNORDERED_TEXTURE1D_ARRAY_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(UnorderedTexture1DArrayViewDescription, \
		structMember_##_, Texture1DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit UnorderedTexture1DArrayViewDescription()
			: UnorderedAccessViewDescription(D3D11_UAV_DIMENSION_TEXTURE1DARRAY)
		{
		}
		UNORDERED_TEXTURE1D_ARRAY_DESCRIPTION_SETTER(UINT, ArraySize)
		UNORDERED_TEXTURE1D_ARRAY_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		UNORDERED_TEXTURE1D_ARRAY_DESCRIPTION_SETTER(UINT, MipSlice)
		UNORDERED_TEXTURE1D_ARRAY_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture1DArray.FirstArraySlice)
		UNORDERED_TEXTURE1D_ARRAY_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture1DArray.FirstArraySlice)

#undef UNORDERED_TEXTURE1D_ARRAY_DESCRIPTION_SETTER
#undef UNORDERED_TEXTURE1D_ARRAY_DESCRIPTION_SETTER_DESC
	};

	struct UnorderedTexture2DViewDescription : public UnorderedAccessViewDescription
	{
#define UNORDERED_TEXTURE2D_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(UnorderedTexture2DViewDescription,			\
		structMember_##_, valueType_, Texture2D.structMember_)

		explicit UnorderedTexture2DViewDescription()
			: UnorderedAccessViewDescription(D3D11_UAV_DIMENSION_TEXTURE2D)
		{
		}
		UNORDERED_TEXTURE2D_DESCRIPTION_SETTER(UINT, MipSlice)

#undef UNORDERED_TEXTURE2D_DESCRIPTION_SETTER
	};

	struct UnorderedTexture2DArrayViewDescription : public UnorderedAccessViewDescription
	{
#define UNORDERED_TEXTURE2D_ARRAY_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(UnorderedTexture2DArrayViewDescription,			\
		structMember_##_, valueType_, Texture2DArray.structMember_)
#define UNORDERED_TEXTURE2D_ARRAY_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D11_HELPER_SETTER_DESC(UnorderedTexture2DArrayViewDescription, \
		structMember_##_, Texture2DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit UnorderedTexture2DArrayViewDescription()
			: UnorderedAccessViewDescription(D3D11_UAV_DIMENSION_TEXTURE2DARRAY)
		{
		}
		UNORDERED_TEXTURE2D_ARRAY_DESCRIPTION_SETTER(UINT, ArraySize)
		UNORDERED_TEXTURE2D_ARRAY_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		UNORDERED_TEXTURE2D_ARRAY_DESCRIPTION_SETTER(UINT, MipSlice)
		UNORDERED_TEXTURE2D_ARRAY_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture1D, D3D11_TEXTURE1D_DESC, ArraySize - Texture2DArray.FirstArraySlice)
		UNORDERED_TEXTURE2D_ARRAY_DESCRIPTION_SETTER_DESC(ArraySize, ID3D11Texture2D, D3D11_TEXTURE2D_DESC, ArraySize - Texture2DArray.FirstArraySlice)

#undef UNORDERED_TEXTURE2D_ARRAY_DESCRIPTION_SETTER
#undef UNORDERED_TEXTURE2D_ARRAY_DESCRIPTION_SETTER_DESC
	};

	struct UnorderedTexture3DViewDescription : public UnorderedAccessViewDescription
	{
#define UNORDERED_TEXTURE3D_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(UnorderedTexture3DViewDescription,			\
		structMember_##_, valueType_, Texture3D.structMember_)

		explicit UnorderedTexture3DViewDescription()
			: UnorderedAccessViewDescription(D3D11_UAV_DIMENSION_TEXTURE3D)
		{
		}
		UNORDERED_TEXTURE3D_DESCRIPTION_SETTER(UINT, MipSlice)
		UNORDERED_TEXTURE3D_DESCRIPTION_SETTER(UINT, FirstWSlice)
		UNORDERED_TEXTURE3D_DESCRIPTION_SETTER(UINT, WSize)

#undef UNORDERED_TEXTURE3D_DESCRIPTION_SETTER
	};

	struct SamplerDescription : public CD3D11_SAMPLER_DESC
	{
#define SAMPLER_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(SamplerDescription, structMember_##_, valueType_, structMember_)

		explicit SamplerDescription()
			: CD3D11_SAMPLER_DESC(D3D11_DEFAULT)
		{
			Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
			MaxAnisotropy = 16;
			BorderColor[0] = 0.0f;
			BorderColor[1] = 0.0f;
			BorderColor[2] = 0.0f;
			BorderColor[3] = 0.0f;
		}
		SAMPLER_DESCRIPTION_SETTER(D3D11_FILTER, Filter)
		SAMPLER_DESCRIPTION_SETTER(D3D11_TEXTURE_ADDRESS_MODE, AddressU)
		SAMPLER_DESCRIPTION_SETTER(D3D11_TEXTURE_ADDRESS_MODE, AddressV)
		SAMPLER_DESCRIPTION_SETTER(D3D11_TEXTURE_ADDRESS_MODE, AddressW)
		SAMPLER_DESCRIPTION_SETTER(float, MipLODBias)
		SAMPLER_DESCRIPTION_SETTER(UINT, MaxAnisotropy)
		SAMPLER_DESCRIPTION_SETTER(D3D11_COMPARISON_FUNC, ComparisonFunc)
		SamplerDescription &BorderColor_(UINT index, float value)
		{
			assert(index <= 3);
			BorderColor[index] = value;
			return *this;
		}
		SAMPLER_DESCRIPTION_SETTER(float, MinLOD)
		SAMPLER_DESCRIPTION_SETTER(float, MaxLOD)

		ID3D11SamplerState *Create(ID3D11Device *device)
		{
			ID3D11SamplerState *result = 0;
			NULL_ON_FAIL(device->CreateSamplerState(this, &result));
			return result;
		}

#undef SAMPLER_DESCRIPTION_SETTER
	};

	struct QueryDescription : public CD3D11_QUERY_DESC
	{
#define QUERY_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(QueryDescription, structMember_##_, valueType_, structMember_)

		explicit QueryDescription(D3D11_QUERY query)
			: CD3D11_QUERY_DESC(query)
		{
		}
		QUERY_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D11Query *Create(ID3D11Device *device)
		{
			ID3D11Query *result = 0;
			NULL_ON_FAIL(device->CreateQuery(this, &result));
			return result;
		}

#undef QUERY_DESCRIPTION_SETTER
	};

	class PredicateDescription : public CD3D11_QUERY_DESC
	{
	public:
#define PREDICATE_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(PredicateDescription, structMember_##_, valueType_, structMember_)

		PredicateDescription(D3D11_QUERY query)
			: CD3D11_QUERY_DESC(query)
		{
			assert((D3D11_QUERY_OCCLUSION_PREDICATE == query)
				|| (D3D11_QUERY_SO_OVERFLOW_PREDICATE == query));
		}
		PREDICATE_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D11Predicate *Create(ID3D11Device *device)
		{
			ID3D11Predicate *result = 0;
			NULL_ON_FAIL(device->CreatePredicate(this, &result));
			return result;
		}

#undef PREDICATE_DESCRIPTION_SETTER
	};

	struct CounterDescription : public CD3D11_COUNTER_DESC
	{
#define COUNTER_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D11_HELPER_STRUCT_SETTER(CounterDescription, structMember_##_, valueType_, structMember_)

		explicit CounterDescription(D3D11_COUNTER counter)
			: CD3D11_COUNTER_DESC(counter)
		{
		}
		COUNTER_DESCRIPTION_SETTER(UINT, MiscFlags)
		ID3D11Counter *Create(ID3D11Device *device)
		{
			ID3D11Counter *result = 0;
			NULL_ON_FAIL(device->CreateCounter(this, &result));
			return result;
		}

#undef COUNTER_DESCRIPTION_SETTER
	};

	struct SubResourceData : public D3D11_SUBRESOURCE_DATA
	{
#define SUB_RESOURCE_DATA_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(SubResourceData, structMember_##_, valueType_, structMember_)

		explicit SubResourceData(void const *data)
		{
			assert(data && "Supplied pointer should not be zero.");
			pSysMem = data;
			SysMemPitch = 0;
			SysMemSlicePitch = 0;
		}
		SUB_RESOURCE_DATA_SETTER(UINT, SysMemPitch)
		SUB_RESOURCE_DATA_SETTER(UINT, SysMemSlicePitch)

#undef SUB_RESOURCE_DATA_SETTER
	};

	struct StreamOutDeclaration : public D3D11_SO_DECLARATION_ENTRY
	{
#define STREAM_OUT_DECLARATION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(StreamOutDeclaration, structMember_##_, valueType_, structMember_)

		explicit StreamOutDeclaration(UINT stream, LPCSTR semanticName,
			UINT semanticIndex = 0, BYTE startComponent = 0,
			BYTE componentCount = 1, BYTE outputSlot = 0)
		{
			Stream = stream;
			SemanticName = semanticName;
			SemanticIndex = semanticIndex;
			StartComponent = startComponent;
			ComponentCount = componentCount;
			OutputSlot = outputSlot;
		}
		STREAM_OUT_DECLARATION_SETTER(LPCSTR, SemanticName)
		STREAM_OUT_DECLARATION_SETTER(UINT, SemanticIndex)
		STREAM_OUT_DECLARATION_SETTER(BYTE, StartComponent)
		STREAM_OUT_DECLARATION_SETTER(BYTE, ComponentCount)
		STREAM_OUT_DECLARATION_SETTER(BYTE, OutputSlot)

#undef STREAM_OUT_DECLARATION_SETTER
	};

	struct InputElementDescription : public D3D11_INPUT_ELEMENT_DESC
	{
#define INPUT_ELEMENT_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(InputElementDescription, \
		structMember_##_, valueType_, structMember_)

		explicit InputElementDescription(LPCSTR semanticName)
		{
			assert(semanticName && "Must specify a semantic name");
			SemanticName = semanticName;
			SemanticIndex = 0;
			Format = DXGI_FORMAT_UNKNOWN;
			InputSlot = 0;
			AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		}
		INPUT_ELEMENT_DESCRIPTION_SETTER(LPCSTR, SemanticName)
		INPUT_ELEMENT_DESCRIPTION_SETTER(DXGI_FORMAT, Format)
		INPUT_ELEMENT_DESCRIPTION_SETTER(UINT, SemanticIndex)
		INPUT_ELEMENT_DESCRIPTION_SETTER(UINT, InputSlot)
		INPUT_ELEMENT_DESCRIPTION_SETTER(UINT, AlignedByteOffset)

#undef INPUT_ELEMENT_DESCRIPTION_SETTER
	};

	struct PerVertexInputElementDescription : public InputElementDescription
	{
		explicit PerVertexInputElementDescription(LPCSTR semanticName)
			: InputElementDescription(semanticName)
		{
			InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			InstanceDataStepRate = 0;
		}
	};

	struct PerInstanceInputElementDescription : public InputElementDescription
	{
#define PER_INSTANCE_INPUT_ELEMENT_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D11_HELPER_STRUCT_SETTER(PerInstanceInputElementDescription, \
		structMember_##_, valueType_, structMember_)

		explicit PerInstanceInputElementDescription(LPCSTR semanticName)
			: InputElementDescription(semanticName)
		{
			InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
			InstanceDataStepRate = 1;
		}
		PER_INSTANCE_INPUT_ELEMENT_DESCRIPTION_SETTER(UINT, InstanceDataStepRate)

#undef PER_INSTANCE_INPUT_ELEMENT_DESCRIPTION_SETTER
	};

#undef D3D11_HELPER_STRUCT_SETTER
#undef D3D11_HELPER_SETTER_DESC
} }

#endif
