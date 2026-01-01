#pragma once

#include "pch.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3D10Blob;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
struct ID3D11SamplerState;

namespace StoryboardEngine
{
	class ColourShader
	{
	public:
		ColourShader();
		ColourShader(const ColourShader&) = default;
		~ColourShader();

		bool Initialize(ID3D11Device*, ID3D11DeviceContext*);
		void Shutdown();
		//bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*);

		static void SetWorldMatrix(Matrix& matrix);
		static void SetViewMatrix(Matrix& matrix);
		static void SetProjectionMatrix(Matrix& matrix);
		static void SetColourTint(Vector4& colour);
		static void SetTexture(ID3D11ShaderResourceView* texture);

		static void Render(ID3D11DeviceContext*, int);
	private:
		bool InitializeShader(ID3D11Device*, const WCHAR*, const WCHAR*);
		void OutputShaderErrorMessage(ID3D10Blob*, const WCHAR*) const;

	private:
		static ColourShader* s_instance;

		ID3D11VertexShader* m_vertexShader;
		ID3D11PixelShader* m_pixelShader;
		ID3D11InputLayout* m_layout;
		ID3D11Buffer* m_worldMatrixBuffer;
		ID3D11Buffer* m_viewMatrixBuffer;
		ID3D11Buffer* m_projectionMatrixBuffer;
		ID3D11Buffer* m_colourBuffer;
		ID3D11SamplerState* m_samplerState;

		ID3D11DeviceContext* m_context;
	};
}
