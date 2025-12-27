#pragma once
#include <DirectXMath.h>

namespace StoryboardEngine
{
	struct VertexData
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT2 uv;
		DirectX::XMFLOAT3 normal;
	};
}