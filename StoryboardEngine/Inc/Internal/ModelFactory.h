#pragma once
#include "VertexData.h"

struct ID3D11Device;
struct aiScene;
struct aiNode;

namespace StoryboardEngine
{
	class ModelData;
	class PrimativeModel;

	class ModelFactory final
	{
	private:
		static ModelData* ConstructModel(ID3D11Device* device, const aiScene* modelScene, const aiNode* node);

	protected:
		static ModelData* ConstructModel(ID3D11Device* device, const aiScene* modelScene);

		friend class ResourceManager;
	private:
		ModelFactory() = delete;
	};
}
