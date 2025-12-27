#include "pch.h"
#include "Internal/ModelFactory.h"
#include <d3d11.h>
#include <assimp/scene.h>
#include "Internal/ModelData.h"
#include "Internal/PrimativeModel.h"

StoryboardEngine::ModelData* StoryboardEngine::ModelFactory::ConstructModel(ID3D11Device* device, const aiScene* modelScene, const aiNode* node)
{
	ModelData* model = new ModelData();
    
    if (node->mNumMeshes <= 0)
    {
        Logger::LogError("Error: Cannot construct model, aiNode contains no meshes.");
        delete model;
        return nullptr;
	}

    std::vector<VertexData> vertices;
    std::vector<unsigned long> indices;
    for (UINT i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = modelScene->mMeshes[node->mMeshes[i]];
        ULONG vertexOffset = static_cast<ULONG>(vertices.size());
        for (UINT v = 0; v < mesh->mNumVertices; ++v)
        {
            VertexData vertex;
            vertex.position = DirectX::XMFLOAT3(mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z);
            vertex.uv = mesh->HasTextureCoords(0) ? DirectX::XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y) : DirectX::XMFLOAT2(0.0f, 0.0f);
            vertex.normal = mesh->HasNormals() ? DirectX::XMFLOAT3(mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z) : DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
            vertices.push_back(vertex);
        }
        for (UINT f = 0; f < mesh->mNumFaces; ++f)
        {
            aiFace& face = mesh->mFaces[f];
            for (UINT j = 0; j < face.mNumIndices; ++j)
            {
                indices.push_back(static_cast<ULONG>(face.mIndices[j]) + vertexOffset);
            }
        }
    }
    if (!model->Initialize(device, vertices, indices))
    {
        Logger::LogError("Error: Failed to construct model from aiScene data.");
        delete model;
        return nullptr;
    }
    
    for (UINT i = 0; i < node->mNumChildren; ++i)
    {
        ModelData* childModel = ConstructModel(device, modelScene, node->mChildren[i]);
        if (childModel)
        {
            model->AddSubModel(childModel);
        }
	}

	return model;
}

StoryboardEngine::ModelData* StoryboardEngine::ModelFactory::ConstructModel(ID3D11Device* device, const aiScene* modelScene)
{
    if (!modelScene || !modelScene->HasMeshes())
    {
        Logger::LogError("Error: Cannot construct model, aiScene appears invalid or contains no meshes.");
        return nullptr;
    }

	return ConstructModel(device, modelScene, modelScene->mRootNode);
}
