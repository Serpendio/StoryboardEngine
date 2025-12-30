#pragma once
#include "Internal/ModelData.h"

namespace StoryboardEngine
{
    class PrimativeModel : public ModelData
    {
	public:
		static PrimativeModel* MakeTriangle(ID3D11Device* device);
		static PrimativeModel* MakeQuad(ID3D11Device* device);
		static PrimativeModel* MakePlane(ID3D11Device* device, size_t resolution);

		static PrimativeModel* MakeCube(ID3D11Device* device);
		static PrimativeModel* MakeCapsule(ID3D11Device* device, float aspectRatio = 1.4f, size_t latitudeLines = 8, size_t longitudeLines = 8);

		static PrimativeModel* MakeCustom(ID3D11Device* device, std::vector<VertexData> vertices, std::vector<unsigned long> indices);

	private:
		PrimativeModel() = default;
    };
}

