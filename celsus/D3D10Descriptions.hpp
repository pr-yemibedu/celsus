#if !defined(D3D10_DESCRIPTIONS_H)
#define D3D10_DESCRIPTIONS_H

// from http://legalizeadulthood.wordpress.com/2009/07/12/description-helpers-for-direct3d-10-10-1-and-11/

#include <cassert>
#include <d3d10.h>
#include <exception>
#include <tchar.h>
#include <winerror.h>
#include <wtypes.h>

#define THR(expr_) rt::COMException::ThrowHResult(expr_, _T(#expr_), _T(__FILE__), __LINE__)

namespace rt { 

	class Exception : public std::exception
	{
	public:
		Exception(char const *message) : exception(message)
		{
		}
	};
	
		class COMException : public Exception
	{
	public:
		explicit COMException(HRESULT status, LPCTSTR expr, LPCTSTR file, UINT line)
			: Exception("COM error"),
			m_status(status),
			m_expr(expr),
			m_file(file),
			m_line(line)
		{
		}
		HRESULT Status() const { return m_status; }
		LPCTSTR File() const { return m_file; }
		UINT Line() const { return m_line; }

		static HRESULT ThrowHResult(HRESULT status, LPCTSTR expr, LPCTSTR file, UINT line)
		{
			if (FAILED(status))
			{
				throw COMException(status, expr, file, line);
			}
			return status;
		}

	private:
		HRESULT m_status;
		LPCTSTR m_expr;
		LPCTSTR m_file;
		UINT m_line;
	};

	namespace D3D10
{
#define D3D10_HELPER_STRUCT_SETTER(structType_, methodName_, valueType_, structMember_)	\
	structType_ &methodName_(valueType_ value)											\
	{																					\
		this->structMember_ = value;													\
		return *this;																	\
	}
#define D3D10_HELPER_SETTER_DESC(structType_, methodName_, structMember_, interfaceType_, descType_, descMember_) \
	structType_ &methodName_(interfaceType_ *texture)	\
	{													\
		descType_ desc;									\
		texture->GetDesc(&desc);						\
		structMember_ = desc.descMember_;				\
		return *this;									\
	}

	struct InputElementDescription : public D3D10_INPUT_ELEMENT_DESC
	{
#define INPUT_ELEMENT_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(InputElementDescription, \
		structMember_##_, valueType_, structMember_)

		explicit InputElementDescription(LPCSTR semanticName)
		{
			assert(semanticName && "Must specify a semantic name");
			SemanticName = semanticName;
			SemanticIndex = 0;
			Format = DXGI_FORMAT_UNKNOWN;
			InputSlot = 0;
			AlignedByteOffset = D3D10_APPEND_ALIGNED_ELEMENT;
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
			InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
			InstanceDataStepRate = 0;
		}
	};

	struct PerInstanceInputElementDescription : public InputElementDescription
	{
#define PER_INSTANCE_INPUT_ELEMENT_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(PerInstanceInputElementDescription, \
		structMember_##_, valueType_, structMember_)

		explicit PerInstanceInputElementDescription(LPCSTR semanticName)
			: InputElementDescription(semanticName)
		{
			InputSlotClass = D3D10_INPUT_PER_INSTANCE_DATA;
			InstanceDataStepRate = 1;
		}
		PER_INSTANCE_INPUT_ELEMENT_DESCRIPTION_SETTER(UINT, InstanceDataStepRate)

#undef PER_INSTANCE_INPUT_ELEMENT_DESCRIPTION_SETTER
	};

	struct StreamOutDeclaration : public D3D10_SO_DECLARATION_ENTRY
	{
#define STREAM_OUT_DECLARATION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(StreamOutDeclaration, structMember_##_, valueType_, structMember_)

		explicit StreamOutDeclaration(LPCSTR semanticName,
			UINT semanticIndex = 0, BYTE startComponent = 0,
			BYTE componentCount = 1, BYTE outputSlot = 0)
		{
			assert(semanticName && "Must specify a semantic");
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

	struct DepthStencilOpDescription : public D3D10_DEPTH_STENCILOP_DESC
	{
#define DEPTH_STENCIL_OP_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(DepthStencilOpDescription, structMember_##_, valueType_, structMember_)

		explicit DepthStencilOpDescription()
			: D3D10_DEPTH_STENCILOP_DESC()
		{
			StencilFailOp = D3D10_STENCIL_OP_KEEP;
			StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
			StencilPassOp = D3D10_STENCIL_OP_KEEP;
			StencilFunc = D3D10_COMPARISON_ALWAYS;
		}
		DEPTH_STENCIL_OP_DESCRIPTION_SETTER(D3D10_STENCIL_OP, StencilFailOp)
		DEPTH_STENCIL_OP_DESCRIPTION_SETTER(D3D10_STENCIL_OP, StencilDepthFailOp)
		DEPTH_STENCIL_OP_DESCRIPTION_SETTER(D3D10_STENCIL_OP, StencilPassOp)
		DEPTH_STENCIL_OP_DESCRIPTION_SETTER(D3D10_COMPARISON_FUNC, StencilFunc)

#undef DEPTH_STENCIL_OP_DESCRIPTION_SETTER
	};

	struct DepthStencilDescription : public D3D10_DEPTH_STENCIL_DESC
	{
#define DEPTH_STENCIL_DESCRIPTION_SETTER(methodName_, valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(DepthStencilDescription, methodName_, valueType_, structMember_)

		explicit DepthStencilDescription() : D3D10_DEPTH_STENCIL_DESC()
		{
			DepthEnable = TRUE;
			DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
			DepthFunc = D3D10_COMPARISON_LESS;
			StencilEnable = FALSE;
			StencilReadMask = D3D10_DEFAULT_STENCIL_READ_MASK;
			StencilWriteMask = D3D10_DEFAULT_STENCIL_WRITE_MASK;
			FrontFace = DepthStencilOpDescription();
			BackFace = DepthStencilOpDescription();
		}
		DEPTH_STENCIL_DESCRIPTION_SETTER(DepthEnable_, BOOL, DepthEnable)
		DEPTH_STENCIL_DESCRIPTION_SETTER(DepthWriteMask_, D3D10_DEPTH_WRITE_MASK, DepthWriteMask)
		DEPTH_STENCIL_DESCRIPTION_SETTER(DepthFunc_, D3D10_COMPARISON_FUNC, DepthFunc)
		DEPTH_STENCIL_DESCRIPTION_SETTER(StencilEnable_, BOOL, StencilEnable)
		DEPTH_STENCIL_DESCRIPTION_SETTER(StencilReadMask_, UINT8, StencilReadMask)
		DEPTH_STENCIL_DESCRIPTION_SETTER(StencilWriteMask_, UINT8, StencilWriteMask)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontFace_, D3D10_DEPTH_STENCILOP_DESC const &, FrontFace)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontStencilFailOp_, D3D10_STENCIL_OP, FrontFace.StencilFailOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontStencilDepthFailOp_, D3D10_STENCIL_OP, FrontFace.StencilDepthFailOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontStencilPassOp_, D3D10_STENCIL_OP, FrontFace.StencilPassOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(FrontStencilFunc_, D3D10_COMPARISON_FUNC, FrontFace.StencilFunc)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackFace_, D3D10_DEPTH_STENCILOP_DESC const &, BackFace)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackStencilFailOp_, D3D10_STENCIL_OP, BackFace.StencilFailOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackStencilDepthFailOp_, D3D10_STENCIL_OP, BackFace.StencilDepthFailOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackStencilPassOp_, D3D10_STENCIL_OP, BackFace.StencilPassOp)
		DEPTH_STENCIL_DESCRIPTION_SETTER(BackStencilFunc_, D3D10_COMPARISON_FUNC, BackFace.StencilFunc)

		ID3D10DepthStencilState *Create(ID3D10Device *device)
		{
			ID3D10DepthStencilState *result = 0;
			THR(device->CreateDepthStencilState(this, &result));
			return result;
		}

#undef DEPTH_STENCIL_DESCRIPTION_SETTER
	};

	struct BlendDescription : public D3D10_BLEND_DESC
	{
#define BLEND_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(BlendDescription, structMember_##_, valueType_, structMember_)

		explicit BlendDescription() : D3D10_BLEND_DESC()
		{
			AlphaToCoverageEnable = FALSE;
			SrcBlend = D3D10_BLEND_ONE;
			DestBlend = D3D10_BLEND_ZERO;
			BlendOp = D3D10_BLEND_OP_ADD;
			SrcBlendAlpha = D3D10_BLEND_ONE;
			DestBlendAlpha = D3D10_BLEND_ZERO;
			BlendOpAlpha = D3D10_BLEND_OP_ADD;
			for (UINT i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
			{
				BlendEnable[i] = FALSE;
				RenderTargetWriteMask[i] = D3D10_COLOR_WRITE_ENABLE_ALL;
			}
		}
		BLEND_DESCRIPTION_SETTER(BOOL, AlphaToCoverageEnable)
		BLEND_DESCRIPTION_SETTER(D3D10_BLEND, SrcBlend)
		BLEND_DESCRIPTION_SETTER(D3D10_BLEND, DestBlend)
		BLEND_DESCRIPTION_SETTER(D3D10_BLEND_OP, BlendOp)
		BLEND_DESCRIPTION_SETTER(D3D10_BLEND, SrcBlendAlpha)
		BLEND_DESCRIPTION_SETTER(D3D10_BLEND, DestBlendAlpha)
		BLEND_DESCRIPTION_SETTER(D3D10_BLEND_OP, BlendOpAlpha)
		BlendDescription &BlendEnable_(UINT index, BOOL value)
		{
			assert(index < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT);
			BlendEnable[index] = value;
			return *this;
		}
		BlendDescription &RenderTargetWriteMask_(UINT index, UINT8 value)
		{
			assert(index < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT);
			RenderTargetWriteMask[index] = value;
			return *this;
		}

		ID3D10BlendState *Create(ID3D10Device *device)
		{
			ID3D10BlendState *result = 0;
			THR(device->CreateBlendState(this, &result));
			return result;
		}

#undef BLEND_DESCRIPTION_SETTER
	};

	struct RasterizerDescription : public D3D10_RASTERIZER_DESC
	{
#define RASTERIZER_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(RasterizerDescription, structMember_##_, valueType_, structMember_)

		explicit RasterizerDescription() : D3D10_RASTERIZER_DESC()
		{
			FillMode = D3D10_FILL_SOLID;
			CullMode = D3D10_CULL_BACK;
			FrontCounterClockwise = FALSE;
			DepthBias = 0;
			DepthBiasClamp = 0.0f;
			SlopeScaledDepthBias = 0.0f;
			DepthClipEnable = TRUE;
			ScissorEnable = FALSE;
			MultisampleEnable = FALSE;
			AntialiasedLineEnable = FALSE;
		}
		RASTERIZER_DESCRIPTION_SETTER(D3D10_FILL_MODE, FillMode)
		RASTERIZER_DESCRIPTION_SETTER(D3D10_CULL_MODE, CullMode)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, FrontCounterClockwise)
		RASTERIZER_DESCRIPTION_SETTER(int, DepthBias)
		RASTERIZER_DESCRIPTION_SETTER(float, DepthBiasClamp)
		RASTERIZER_DESCRIPTION_SETTER(float, SlopeScaledDepthBias)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, DepthClipEnable)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, ScissorEnable)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, MultisampleEnable)
		RASTERIZER_DESCRIPTION_SETTER(BOOL, AntialiasedLineEnable)

		ID3D10RasterizerState *Create(ID3D10Device *device)
		{
			ID3D10RasterizerState *result = 0;
			THR(device->CreateRasterizerState(this, &result));
			return result;
		}

#undef RASTERIZER_DESCRIPTION_SETTER
	};

	struct SubResourceData : public D3D10_SUBRESOURCE_DATA
	{
#define SUB_RESOURCE_DATA_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(SubResourceData, structMember_##_, valueType_, structMember_)

		explicit SubResourceData(void const *data)
		{
			assert(data && "Supplied pointer should not be zero");
			pSysMem = data;
			SysMemPitch = 0;
			SysMemSlicePitch = 0;
		}
		SUB_RESOURCE_DATA_SETTER(UINT, SysMemPitch)
		SUB_RESOURCE_DATA_SETTER(UINT, SysMemSlicePitch)

#undef SUB_RESOURCE_DATA_SETTER
	};

	struct BufferDescription : public CD3D10_BUFFER_DESC
	{
#define BUFFER_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(BufferDescription, structMember_##_, valueType_, structMember_)

		explicit BufferDescription(UINT byteWidth, UINT bindFlags)
			: CD3D10_BUFFER_DESC(byteWidth, bindFlags)
		{
		}
		BUFFER_DESCRIPTION_SETTER(D3D10_USAGE, Usage)
		BUFFER_DESCRIPTION_SETTER(UINT, CPUAccessFlags)
		BUFFER_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D10Buffer *Create(ID3D10Device *device, D3D10_SUBRESOURCE_DATA *data = 0)
		{
			ID3D10Buffer *result = 0;
			THR(device->CreateBuffer(this, data, &result));
			return result;
		}

#undef BUFFER_DESCRIPTION_SETTER
	};

	struct Texture1DDescription : public CD3D10_TEXTURE1D_DESC
	{
#define TEXTURE1D_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(Texture1DDescription, structMember_##_, valueType_, structMember_)

		explicit Texture1DDescription(DXGI_FORMAT format, UINT width)
			: CD3D10_TEXTURE1D_DESC(format, width)
		{
		}
		TEXTURE1D_DESCRIPTION_SETTER(UINT, ArraySize)
		TEXTURE1D_DESCRIPTION_SETTER(UINT, MipLevels)
		TEXTURE1D_DESCRIPTION_SETTER(UINT, BindFlags)
		TEXTURE1D_DESCRIPTION_SETTER(D3D10_USAGE, Usage)
		TEXTURE1D_DESCRIPTION_SETTER(UINT, CPUAccessFlags)
		TEXTURE1D_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D10Texture1D *Create(ID3D10Device *device, D3D10_SUBRESOURCE_DATA const *data = 0)
		{
			ID3D10Texture1D *result = 0;
			THR(device->CreateTexture1D(this, data, &result));
			return result;
		}

#undef TEXTURE1D_DESCRIPTION_SETTER
	};

	struct Texture2DDescription : public CD3D10_TEXTURE2D_DESC
	{
#define TEXTURE2D_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(Texture2DDescription, structMember_##_, valueType_, structMember_)

		explicit Texture2DDescription(DXGI_FORMAT format, UINT width, UINT height)
			: CD3D10_TEXTURE2D_DESC(format, width, height)
		{
		}
		TEXTURE2D_DESCRIPTION_SETTER(UINT, ArraySize)
		TEXTURE2D_DESCRIPTION_SETTER(UINT, MipLevels)
		TEXTURE2D_DESCRIPTION_SETTER(UINT, BindFlags)
		TEXTURE2D_DESCRIPTION_SETTER(D3D10_USAGE, Usage)
		TEXTURE2D_DESCRIPTION_SETTER(UINT, CPUAccessFlags)
		D3D10_HELPER_STRUCT_SETTER(Texture2DDescription, SampleCount_, UINT, SampleDesc.Count)
		D3D10_HELPER_STRUCT_SETTER(Texture2DDescription, SampleQuality_, UINT, SampleDesc.Quality)
		TEXTURE2D_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D10Texture2D *Create(ID3D10Device *device, D3D10_SUBRESOURCE_DATA const *data = 0)
		{
			ID3D10Texture2D *result = 0;
			THR(device->CreateTexture2D(this, data, &result));
			return result;
		}

#undef TEXTURE2D_DESCRIPTION_SETTER
	};

	struct Texture3DDescription : public CD3D10_TEXTURE3D_DESC
	{
#define TEXTURE3D_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(Texture3DDescription, structMember_##_, valueType_, structMember_)

		explicit Texture3DDescription(DXGI_FORMAT format, UINT width, UINT height, UINT depth)
			: CD3D10_TEXTURE3D_DESC(format, width, height, depth)
		{
		}
		TEXTURE3D_DESCRIPTION_SETTER(UINT, MipLevels)
		TEXTURE3D_DESCRIPTION_SETTER(UINT, BindFlags)
		TEXTURE3D_DESCRIPTION_SETTER(D3D10_USAGE, Usage)
		TEXTURE3D_DESCRIPTION_SETTER(UINT, CPUAccessFlags)
		TEXTURE3D_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D10Texture3D *Create(ID3D10Device *device, D3D10_SUBRESOURCE_DATA const *data = 0)
		{
			ID3D10Texture3D *result = 0;
			THR(device->CreateTexture3D(this, data, &result));
			return result;
		}

#undef TEXTURE3D_DESCRIPTION_SETTER
	};

	struct ShaderResourceViewDescription : public D3D10_SHADER_RESOURCE_VIEW_DESC
	{
#define SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(ShaderResourceViewDescription, \
		structMember_##_, valueType_, structMember_)
#define SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_) \
	D3D10_HELPER_SETTER_DESC(ShaderResourceViewDescription, \
		structMember_##_, structMember_, interfaceType_, descType_, structMember_)

		explicit ShaderResourceViewDescription(D3D10_SRV_DIMENSION viewDimension)
			: D3D10_SHADER_RESOURCE_VIEW_DESC()
		{
			ViewDimension = viewDimension;
			Format = DXGI_FORMAT_UNKNOWN;
		}
		SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER(DXGI_FORMAT, Format)
		SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D10Texture1D, D3D10_TEXTURE1D_DESC)
		SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D10Texture2D, D3D10_TEXTURE2D_DESC)
		SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D10Texture3D, D3D10_TEXTURE3D_DESC)

		ID3D10ShaderResourceView *Create(ID3D10Device *device, ID3D10Resource *resource)
		{
			ID3D10ShaderResourceView *result = 0;
			THR(device->CreateShaderResourceView(resource, this, &result));
			return result;
		}

#undef SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER
#undef SHADER_RESOURCE_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderBufferViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_BUFFER_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(ShaderBufferViewDescription, structMember_##_, valueType_, Buffer.structMember_)

		explicit ShaderBufferViewDescription()
			: ShaderResourceViewDescription(D3D10_SRV_DIMENSION_BUFFER)
		{
			Buffer.FirstElement = 0;
			Buffer.NumElements = ~0U;
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
	D3D10_HELPER_STRUCT_SETTER(ShaderTexture1DViewDescription, \
		structMember_##_, valueType_, Texture1D.structMember_)
#define SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(ShaderTexture1DViewDescription, \
		structMember_##_, Texture1D.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture1DViewDescription()
			: ShaderResourceViewDescription(D3D10_SRV_DIMENSION_TEXTURE1D)
		{
			Texture1D.MostDetailedMip = 0;
			Texture1D.MipLevels = ~0U;
		}
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, MipLevels - Texture1D.MostDetailedMip)
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, MipLevels - Texture1D.MostDetailedMip)
		SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture3D, D3D10_TEXTURE3D_DESC, MipLevels - Texture1D.MostDetailedMip)

#undef SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE1D_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture1DArrayViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(ShaderTexture1DArrayViewDescription,					\
		structMember_##_, valueType_, Texture1DArray.structMember_)
#define SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(ShaderTexture1DArrayViewDescription, \
		structMember_##_, Texture1DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture1DArrayViewDescription()
			: ShaderResourceViewDescription(D3D10_SRV_DIMENSION_TEXTURE1DARRAY)
		{
			Texture1DArray.MostDetailedMip = 0;
			Texture1DArray.MipLevels = ~0U;
			Texture1DArray.FirstArraySlice = 0;
			Texture1DArray.ArraySize = ~0U;
		}
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, MipLevels - Texture1DArray.MostDetailedMip)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, MipLevels - Texture1DArray.MostDetailedMip)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture3D, D3D10_TEXTURE3D_DESC, MipLevels - Texture1DArray.MostDetailedMip)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, ArraySize - Texture1DArray.FirstArraySlice)
		SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, ArraySize - Texture1DArray.FirstArraySlice)

#undef SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture2DViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(ShaderTexture2DViewDescription, \
		structMember_##_, valueType_, Texture2D.structMember_)
#define SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(ShaderTexture2DViewDescription, \
		structMember_##_, Texture2D.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture2DViewDescription()
			: ShaderResourceViewDescription(D3D10_SRV_DIMENSION_TEXTURE2D)
		{
			Texture2D.MostDetailedMip = 0;
			Texture2D.MipLevels = ~0U;
		}
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, MipLevels - Texture2D.MostDetailedMip)
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, MipLevels - Texture2D.MostDetailedMip)
		SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture3D, D3D10_TEXTURE3D_DESC, MipLevels - Texture2D.MostDetailedMip)

#undef SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE2D_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture2DArrayViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(ShaderTexture2DArrayViewDescription,					\
		structMember_##_, valueType_, Texture2DArray.structMember_)
#define SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(ShaderTexture2DArrayViewDescription, \
		structMember_##_, Texture2DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture2DArrayViewDescription()
			: ShaderResourceViewDescription(D3D10_SRV_DIMENSION_TEXTURE2DARRAY)
		{
			Texture2DArray.MostDetailedMip = 0;
			Texture2DArray.MipLevels = ~0U;
			Texture2DArray.FirstArraySlice = 0;
			Texture2DArray.ArraySize = ~0U;
		}
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, MipLevels - Texture2DArray.MostDetailedMip)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, MipLevels - Texture2DArray.MostDetailedMip)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture3D, D3D10_TEXTURE3D_DESC, MipLevels - Texture2DArray.MostDetailedMip)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, ArraySize - Texture2DArray.FirstArraySlice)
		SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, ArraySize - Texture2DArray.FirstArraySlice)

#undef SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture2DMultiSampleViewDescription : public ShaderResourceViewDescription
	{
		explicit ShaderTexture2DMultiSampleViewDescription()
			: ShaderResourceViewDescription(D3D10_SRV_DIMENSION_TEXTURE2DMS)
		{
		}
	};

	struct ShaderTexture2DMultiSampleArrayViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(ShaderTexture2DMultiSampleArrayViewDescription,					\
		structMember_##_, valueType_, Texture2DMSArray.structMember_)
#define SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(ShaderTexture2DMultiSampleArrayViewDescription, \
		structMember_##_, Texture2DMSArray.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture2DMultiSampleArrayViewDescription()
			: ShaderResourceViewDescription(D3D10_SRV_DIMENSION_TEXTURE2DMSARRAY)
		{
			Texture2DMSArray.FirstArraySlice = 0;
			Texture2DMSArray.ArraySize = ~0U;
		}
		SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)
		SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)

#undef SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTexture3DViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(ShaderTexture3DViewDescription, \
		structMember_##_, valueType_, Texture3D.structMember_)
#define SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(ShaderTexture3DViewDescription, \
		structMember_##_, Texture3D.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTexture3DViewDescription()
			: ShaderResourceViewDescription(D3D10_SRV_DIMENSION_TEXTURE3D)
		{
			Texture3D.MostDetailedMip = 0;
			Texture3D.MipLevels = ~0U;
		}
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, MipLevels - Texture3D.MostDetailedMip)
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, MipLevels - Texture3D.MostDetailedMip)
		SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture3D, D3D10_TEXTURE3D_DESC, MipLevels - Texture3D.MostDetailedMip)

#undef SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE3D_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct ShaderTextureCubeViewDescription : public ShaderResourceViewDescription
	{
#define SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(ShaderTextureCubeViewDescription, \
		structMember_##_, valueType_, TextureCube.structMember_)
#define SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(ShaderTextureCubeViewDescription, \
		structMember_##_, TextureCube.structMember_, interfaceType_, descType_, descMember_)

		explicit ShaderTextureCubeViewDescription()
			: ShaderResourceViewDescription(D3D10_SRV_DIMENSION_TEXTURECUBE)
		{
			TextureCube.MostDetailedMip = 0;
			TextureCube.MipLevels = ~0U;
		}
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER(UINT, MipLevels)
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER(UINT, MostDetailedMip)
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, MipLevels - TextureCube.MostDetailedMip)
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, MipLevels - TextureCube.MostDetailedMip)
		SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC(MipLevels, ID3D10Texture3D, D3D10_TEXTURE3D_DESC, MipLevels - TextureCube.MostDetailedMip)

#undef SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER
#undef SHADER_TEXTURE_CUBE_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct RenderTargetViewDescription : public D3D10_RENDER_TARGET_VIEW_DESC
	{
#define RENDER_TARGET_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(RenderTargetViewDescription, \
		structMember_##_, valueType_, structMember_)
#define RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_) \
	D3D10_HELPER_SETTER_DESC(RenderTargetViewDescription, \
		structMember_##_, structMember_, interfaceType_, descType_, structMember_)

		explicit RenderTargetViewDescription(D3D10_RTV_DIMENSION viewDimension)
			: D3D10_RENDER_TARGET_VIEW_DESC()
		{
			ViewDimension = viewDimension;
			Format = DXGI_FORMAT_UNKNOWN;
		}
		RENDER_TARGET_VIEW_DESCRIPTION_SETTER(DXGI_FORMAT, Format)
		RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D10Texture1D, D3D10_TEXTURE1D_DESC)
		RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D10Texture2D, D3D10_TEXTURE2D_DESC)
		RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D10Texture3D, D3D10_TEXTURE3D_DESC)

		ID3D10RenderTargetView *Create(ID3D10Device *device, ID3D10Resource *resource)
		{
			ID3D10RenderTargetView *result = 0;
			THR(device->CreateRenderTargetView(resource, this, &result));
			return result;
		}

#undef RENDER_TARGET_VIEW_DESCRIPTION_SETTER
#undef RENDER_TARGET_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct RenderTargetBufferViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_BUFFER_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(RenderTargetBufferViewDescription, structMember_##_, valueType_, Buffer.structMember_)

		explicit RenderTargetBufferViewDescription()
			: RenderTargetViewDescription(D3D10_RTV_DIMENSION_BUFFER)
		{
			Buffer.FirstElement = 0;
			Buffer.NumElements = ~0U;
		}
		RENDER_TARGET_BUFFER_VIEW_DESCRIPTION_SETTER(UINT, FirstElement)
		RENDER_TARGET_BUFFER_VIEW_DESCRIPTION_SETTER(UINT, NumElements)

#undef RENDER_TARGET_BUFFER_VIEW_DESCRIPTION_SETTER
	};

	struct RenderTargetTexture1DViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE1D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(RenderTargetTexture1DViewDescription,				\
		structMember_##_, valueType_, Texture1D.structMember_)

		explicit RenderTargetTexture1DViewDescription()
			: RenderTargetViewDescription(D3D10_RTV_DIMENSION_TEXTURE1D)
		{
			Texture1D.MipSlice = 0;
		}
		RENDER_TARGET_TEXTURE1D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)

#undef RENDER_TARGET_TEXTURE1D_VIEW_DESCRIPTION_SETTER
	};

	struct RenderTargetTexture1DArrayViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(RenderTargetTexture1DArrayViewDescription,					\
		structMember_##_, valueType_, Texture1DArray.structMember_)
#define RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(RenderTargetViewDescription, \
		structMember_##_, Texture1DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit RenderTargetTexture1DArrayViewDescription()
			: RenderTargetViewDescription(D3D10_RTV_DIMENSION_TEXTURE1DARRAY)
		{
			Texture1DArray.MipSlice = 0;
			Texture1DArray.FirstArraySlice = 0;
			Texture1DArray.ArraySize = ~0U;
		}
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, ArraySize - Texture1DArray.FirstArraySlice)
		RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, ArraySize - Texture1DArray.FirstArraySlice)

#undef RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef RENDER_TARGET_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct RenderTargetTexture2DViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE2D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(RenderTargetTexture2DViewDescription,				\
		structMember_##_, valueType_, Texture2D.structMember_)

		explicit RenderTargetTexture2DViewDescription()
			: RenderTargetViewDescription(D3D10_RTV_DIMENSION_TEXTURE2D)
		{
			Texture2D.MipSlice = 0;
		}
		RENDER_TARGET_TEXTURE2D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)

#undef RENDER_TARGET_TEXTURE2D_VIEW_DESCRIPTION_SETTER
	};

	struct RenderTargetTexture2DArrayViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(RenderTargetTexture2DArrayViewDescription,					\
		structMember_##_, valueType_, Texture2DArray.structMember_)
#define RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(RenderTargetTexture2DArrayViewDescription, \
		structMember_##_, Texture2DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit RenderTargetTexture2DArrayViewDescription()
			: RenderTargetViewDescription(D3D10_RTV_DIMENSION_TEXTURE2DARRAY)
		{
			Texture2DArray.MipSlice = 0;
			Texture2DArray.FirstArraySlice = 0;
			Texture2DArray.ArraySize = ~0U;
		}
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, ArraySize - Texture2DArray.FirstArraySlice)
		RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, ArraySize - Texture2DArray.FirstArraySlice)

#undef RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef RENDER_TARGET_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct RenderTargetTexture2DMultiSampleViewDescription : public RenderTargetViewDescription
	{
		explicit RenderTargetTexture2DMultiSampleViewDescription()
			: RenderTargetViewDescription(D3D10_RTV_DIMENSION_TEXTURE2DMS)
		{
		}
	};

	struct RenderTargetTexture2DMultiSampleArrayViewDescription : public RenderTargetViewDescription
	{
#define DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(RenderTargetTexture2DMultiSampleArrayViewDescription, structMember_##_,	\
		valueType_, Texture2DMSArray.structMember_)
#define RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(RenderTargetTexture2DMultiSampleArrayViewDescription, \
		structMember_##_, Texture2DMSArray.structMember_, interfaceType_, descType_, descMember_)

		explicit RenderTargetTexture2DMultiSampleArrayViewDescription()
			: RenderTargetViewDescription(D3D10_RTV_DIMENSION_TEXTURE2DMSARRAY)
		{
			Texture2DMSArray.FirstArraySlice = 0;
			Texture2DMSArray.ArraySize = ~0U;
		}
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)
		RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)

#undef DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER
#undef RENDER_TARGET_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct RenderTargetTexture3DViewDescription : public RenderTargetViewDescription
	{
#define RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)		\
	D3D10_HELPER_STRUCT_SETTER(RenderTargetTexture3DViewDescription, structMember_##_,	\
		valueType_, Texture3D.structMember_)

		explicit RenderTargetTexture3DViewDescription()
			: RenderTargetViewDescription(D3D10_RTV_DIMENSION_TEXTURE3D)
		{
			Texture3D.MipSlice = 0;
			Texture3D.FirstWSlice = 0;
			Texture3D.WSize = ~0U;
		}
		RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, FirstWSlice)
		RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER(UINT, WSize)

#undef RENDER_TARGET_TEXTURE3D_VIEW_DESCRIPTION_SETTER
	};

	struct DepthStencilViewDescription : public D3D10_DEPTH_STENCIL_VIEW_DESC
	{
#define DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(DepthStencilViewDescription, \
		structMember_##_, valueType_, structMember_)
#define DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_) \
	D3D10_HELPER_SETTER_DESC(DepthStencilViewDescription, \
		structMember_##_, structMember_, interfaceType_, descType_, structMember_)

		explicit DepthStencilViewDescription(D3D10_DSV_DIMENSION viewDimension)
			: D3D10_DEPTH_STENCIL_VIEW_DESC()
		{
			ViewDimension = viewDimension;
			Format = DXGI_FORMAT_UNKNOWN;
		}
		DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER(DXGI_FORMAT, Format)
		DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D10Texture1D, D3D10_TEXTURE1D_DESC)
		DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D10Texture2D, D3D10_TEXTURE2D_DESC)
		DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC(Format, ID3D10Texture3D, D3D10_TEXTURE3D_DESC)

		ID3D10DepthStencilView *Create(ID3D10Device *device, ID3D10Resource *resource)
		{
			ID3D10DepthStencilView *result = 0;
			THR(device->CreateDepthStencilView(resource, this, &result));
			return result;
		}

#undef DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER
#undef DEPTH_STENCIL_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct DepthStencilTexture1DViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE1D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(DepthStencilTexture1DViewDescription, structMember_##_, valueType_, Texture1D.structMember_)

		explicit DepthStencilTexture1DViewDescription()
			: DepthStencilViewDescription(D3D10_DSV_DIMENSION_TEXTURE1D)
		{
		}
		DEPTH_STENCIL_TEXTURE1D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)

#undef DEPTH_STENCIL_TEXTURE1D_VIEW_DESCRIPTION_SETTER
	};

	struct DepthStencilTexture1DArrayViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(DepthStencilTexture1DArrayViewDescription, \
		structMember_##_, valueType_, Texture1DArray.structMember_)
#define DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(DepthStencilTexture1DArrayViewDescription, \
		structMember_##_, Texture1DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit DepthStencilTexture1DArrayViewDescription()
			: DepthStencilViewDescription(D3D10_DSV_DIMENSION_TEXTURE1DARRAY)
		{
			Texture1DArray.MipSlice = 0;
			Texture1DArray.FirstArraySlice = 0;
			Texture1DArray.ArraySize = ~0U;
		}
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, ArraySize - Texture1DArray.FirstArraySlice)
		DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, ArraySize - Texture1DArray.FirstArraySlice)

#undef DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef DEPTH_STENCIL_TEXTURE1D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct DepthStencilTexture2DViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE2D_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(DepthStencilTexture2DViewDescription, structMember_##_, valueType_, Texture2D.structMember_)

		explicit DepthStencilTexture2DViewDescription()
			: DepthStencilViewDescription(D3D10_DSV_DIMENSION_TEXTURE2D)
		{
		}
		DEPTH_STENCIL_TEXTURE2D_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)

#undef DEPTH_STENCIL_TEXTURE2D_VIEW_DESCRIPTION_SETTER
	};

	struct DepthStencilTexture2DArrayViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_) \
	D3D10_HELPER_STRUCT_SETTER(DepthStencilTexture2DArrayViewDescription, \
		structMember_##_, valueType_, Texture2DArray.structMember_)
#define DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(DepthStencilTexture2DArrayViewDescription, \
		structMember_##_, Texture2DArray.structMember_, interfaceType_, descType_, descMember_)

		explicit DepthStencilTexture2DArrayViewDescription()
			: DepthStencilViewDescription(D3D10_DSV_DIMENSION_TEXTURE2DARRAY)
		{
			Texture2DArray.MipSlice = 0;
			Texture2DArray.FirstArraySlice = 0;
			Texture2DArray.ArraySize = ~0U;
		}
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, MipSlice)
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, ArraySize - Texture2DArray.FirstArraySlice)
		DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, ArraySize - Texture2DArray.FirstArraySlice)

#undef DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER
#undef DEPTH_STENCIL_TEXTURE2D_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct DepthStencilTexture2DMultiSampleViewDescription : public DepthStencilViewDescription
	{
		explicit DepthStencilTexture2DMultiSampleViewDescription()
			: DepthStencilViewDescription(D3D10_DSV_DIMENSION_TEXTURE2DMS)
		{
		}
	};

	struct DepthStencilTexture2DMultiSampleArrayViewDescription : public DepthStencilViewDescription
	{
#define DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(DepthStencilTexture2DMultiSampleArrayViewDescription, \
		structMember_##_, valueType_, Texture2DMSArray.structMember_)
#define DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(structMember_, interfaceType_, descType_, descMember_) \
	D3D10_HELPER_SETTER_DESC(DepthStencilTexture2DMultiSampleArrayViewDescription, \
		structMember_##_, Texture2DMSArray.structMember_, interfaceType_, descType_, descMember_)

		explicit DepthStencilTexture2DMultiSampleArrayViewDescription()
			: DepthStencilViewDescription(D3D10_DSV_DIMENSION_TEXTURE2DMSARRAY)
		{
			Texture2DMSArray.FirstArraySlice = 0;
			Texture2DMSArray.ArraySize = ~0U;
		}
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, FirstArraySlice)
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER(UINT, ArraySize)
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture1D, D3D10_TEXTURE1D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)
		DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC(ArraySize, ID3D10Texture2D, D3D10_TEXTURE2D_DESC, ArraySize - Texture2DMSArray.FirstArraySlice)

#undef DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER
#undef DEPTH_STENCIL_TEXTURE2D_MULTI_SAMPLE_ARRAY_VIEW_DESCRIPTION_SETTER_DESC
	};

	struct SamplerDescription : public D3D10_SAMPLER_DESC
	{
#define SAMPLER_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(SamplerDescription, structMember_##_, valueType_, structMember_)

		explicit SamplerDescription() : D3D10_SAMPLER_DESC()
		{
			Filter = D3D10_FILTER_MIN_MAG_MIP_POINT;
			AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
			AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
			AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;
			MipLODBias = 0.0f;
			MaxAnisotropy = 16;
			ComparisonFunc = D3D10_COMPARISON_NEVER;
			BorderColor[0] = 0.0f;
			BorderColor[1] = 0.0f;
			BorderColor[2] = 0.0f;
			BorderColor[3] = 0.0f;
			MinLOD = -FLT_MAX;
			MaxLOD = FLT_MAX;
		}
		SAMPLER_DESCRIPTION_SETTER(D3D10_FILTER, Filter)
		SAMPLER_DESCRIPTION_SETTER(D3D10_TEXTURE_ADDRESS_MODE, AddressU)
		SAMPLER_DESCRIPTION_SETTER(D3D10_TEXTURE_ADDRESS_MODE, AddressV)
		SAMPLER_DESCRIPTION_SETTER(D3D10_TEXTURE_ADDRESS_MODE, AddressW)
		SAMPLER_DESCRIPTION_SETTER(float, MipLODBias)
		SAMPLER_DESCRIPTION_SETTER(UINT, MaxAnisotropy)
		SAMPLER_DESCRIPTION_SETTER(D3D10_COMPARISON_FUNC, ComparisonFunc)
		SamplerDescription &BorderColor_(UINT index, float value)
		{
			assert(index <= 3);
			BorderColor[index] = value;
			return *this;
		}
		SAMPLER_DESCRIPTION_SETTER(float, MinLOD)
		SAMPLER_DESCRIPTION_SETTER(float, MaxLOD)

		ID3D10SamplerState *Create(ID3D10Device *device)
		{
			ID3D10SamplerState *result = 0;
			THR(device->CreateSamplerState(this, &result));
			return result;
		}

#undef SAMPLER_DESCRIPTION_SETTER
	};

	struct QueryDescription : public D3D10_QUERY_DESC
	{
#define QUERY_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(QueryDescription, structMember_##_, valueType_, structMember_)

		explicit QueryDescription(D3D10_QUERY query) : D3D10_QUERY_DESC()
		{
			Query = query;
			MiscFlags = 0;
		}
		QUERY_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D10Query *Create(ID3D10Device *device)
		{
			ID3D10Query *result = 0;
			THR(device->CreateQuery(this, &result));
			return result;
		}

#undef QUERY_DESCRIPTION_SETTER
	};

	class PredicateDescription : public D3D10_QUERY_DESC
	{
	public:
#define PREDICATE_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(PredicateDescription, structMember_##_, valueType_, structMember_)

		explicit PredicateDescription(D3D10_QUERY query) : D3D10_QUERY_DESC()
		{
			assert((D3D10_QUERY_SO_OVERFLOW_PREDICATE == query)
				|| (D3D10_QUERY_OCCLUSION_PREDICATE == query));
			Query = query;
			MiscFlags = 0;
		}
		PREDICATE_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D10Predicate *Create(ID3D10Device *device)
		{
			ID3D10Predicate *result = 0;
			THR(device->CreatePredicate(this, &result));
			return result;
		}

#undef PREDICATE_DESCRIPTION_SETTER
	};

	struct CounterDescription : public D3D10_COUNTER_DESC
	{
#define COUNTER_DESCRIPTION_SETTER(valueType_, structMember_)	\
	D3D10_HELPER_STRUCT_SETTER(CounterDescription, structMember_##_, valueType_, structMember_)

		explicit CounterDescription(D3D10_COUNTER counter) : D3D10_COUNTER_DESC()
		{
			Counter = counter;
			MiscFlags = 0;
		}
		COUNTER_DESCRIPTION_SETTER(UINT, MiscFlags)

		ID3D10Counter *Create(ID3D10Device *device)
		{
			ID3D10Counter *result = 0;
			THR(device->CreateCounter(this, &result));
			return result;
		}

#undef COUNTER_DESCRIPTION_SETTER
	};

#undef D3D10_HELPER_STRUCT_SETTER
} }

#endif
