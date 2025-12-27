#pragma once
#include "pch.h"
#include "Internal/VertexData.h"

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;

namespace StoryboardEngine
{
	class ModelData
	{
	public:
		ModelData();
		ModelData(const ModelData&) = delete;
		ModelData& operator=(const ModelData&) = delete;
		~ModelData();

		// Initializes the model data with the given vertices and indices.
		bool Initialize(ID3D11Device* device, std::vector<VertexData> vertices, std::vector<unsigned long> indices);
		// Adds a sub-model to this model. Ownership of the sub-model is claimed by this model.
		void AddSubModel(ModelData* subModels);

		void Render(ID3D11DeviceContext*);

		int GetIndexCount() const;
	private:
		void ShutdownBuffers();
		void RenderBuffers(ID3D11DeviceContext*);

	private:
		ID3D11Buffer* m_vertexBuffer;
		ID3D11Buffer* m_indexBuffer;
		int m_vertexCount;
		int m_indexCount;
		std::vector<ModelData*> m_subModels;

		DirectX::XMMATRIX m_worldMatrix;
	};
}
