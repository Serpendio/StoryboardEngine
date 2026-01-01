#include "pch.h"
#include "Core/ResourceManager.h"
#include <filesystem>

#include <d3d11.h>
#include <directxtk/WICTextureLoader.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Internal/ModelFactory.h"
#include "Internal/ModelData.h"

std::unordered_map<size_t, ID3D11ShaderResourceView*> StoryboardEngine::ResourceManager::m_textureResources;
std::unordered_map<size_t, StoryboardEngine::ModelData*> StoryboardEngine::ResourceManager::m_modelResources;
std::unordered_map<std::string, size_t> StoryboardEngine::ResourceManager::m_textureIDs;
std::unordered_map<std::string, size_t> StoryboardEngine::ResourceManager::m_modelIDs;
ID3D11Device* StoryboardEngine::ResourceManager::m_device = nullptr;
ID3D11DeviceContext* StoryboardEngine::ResourceManager::m_deviceContext = nullptr;
Assimp::Importer* StoryboardEngine::ResourceManager::m_modelImporter = nullptr;
ID3D11Texture2D* StoryboardEngine::ResourceManager::pTexture = nullptr;

size_t StoryboardEngine::ResourceManager::GetTextureID(const std::string& resourceName)
{
	auto it = m_textureIDs.find(resourceName);

	if (it == m_textureIDs.end())
	{
		Logger::LogError("Resource name not found: ", resourceName);
		return -1;
	}

	return it->second;
}

const std::string& StoryboardEngine::ResourceManager::GetTextureName(size_t resourceID)
{
	for (const auto& [name, id] : m_textureIDs)
	{
		if (id == resourceID)
		{
			return name;
		}
	}

	static std::string emptyString;
	return emptyString; // Return an empty string if not found
}

std::vector<std::string> StoryboardEngine::ResourceManager::GetAllTextureNames()
{
	std::vector<std::string> textureNames;
	textureNames.reserve(m_textureIDs.size());

	for (const auto& [name, id] : m_textureIDs)
	{
		textureNames.push_back(name);
	}

	return textureNames;
}

size_t StoryboardEngine::ResourceManager::GetModelID(const std::string& resourceName)
{
	auto it = m_modelIDs.find(resourceName);

	if (it == m_modelIDs.end())
	{
		Logger::LogError("Resource name not found: ", resourceName);
		return -1;
	}

	return it->second;
}

const std::string& StoryboardEngine::ResourceManager::GetModelName(size_t resourceID)
{
	for (const auto& [name, id] : m_modelIDs)
	{
		if (id == resourceID)
		{
			return name;
		}
	}

	static std::string emptyString;
	return emptyString; // Return an empty string if not found
}

std::vector<std::string> StoryboardEngine::ResourceManager::GetAllModelNames()
{
	std::vector<std::string> modelNames;
	modelNames.reserve(m_modelIDs.size());

	for (const auto& [name, id] : m_modelIDs)
	{
		modelNames.push_back(name);
	}

	return modelNames;
}

ID3D11ShaderResourceView* StoryboardEngine::ResourceManager::GetTextureResource(size_t resourceID)
{
	auto it = m_textureResources.find(resourceID);

	if (it == m_textureResources.end())
	{
		Logger::LogWarning("Texture resource ID not found: ", resourceID);
		return m_textureResources.find(0)->second; // Return a default texture (ID 0) if not found
	}

	return it->second;
}

StoryboardEngine::ModelData* StoryboardEngine::ResourceManager::GetModelResource(size_t resourceID)
{
	auto it = m_modelResources.find(resourceID);

	if (it == m_modelResources.end())
	{
		Logger::LogWarning("Model resource ID not found: ", resourceID);
		return m_modelResources.find(0)->second; // Return a default model (ID 0) if not found
	}

	return it->second;
}

size_t StoryboardEngine::ResourceManager::LoadTexture(const std::string& filePath, const std::string& textureName)
{
	// Generate a simple hash-based ID from the filepath
	size_t id = std::hash<std::string>{}(filePath);
	return LoadTexture(filePath, textureName, id);
}

size_t StoryboardEngine::ResourceManager::LoadModel(const std::string& filepath, const std::string& modelName)
{
	// Generate a simple hash-based ID from the filepath
	size_t id = std::hash<std::string>{}(filepath);

	return LoadModel(filepath, modelName, id);
}

size_t StoryboardEngine::ResourceManager::LoadTexture(const std::string& filepath, const std::string& textureName, size_t id)
{
	std::filesystem::path pathObj(filepath);

	if (!std::filesystem::exists(pathObj))
	{
		Logger::LogError("Texture file does not exist: ", filepath);
		return -1;
	}

	if (pathObj.extension() != ".png" && pathObj.extension() != ".jpg" && pathObj.extension() != ".jpeg" && pathObj.extension() != ".bmp")
	{
		Logger::LogError("Unsupported texture file format: ", pathObj.extension().wstring());
		return -1;
	}

	ID3D11ShaderResourceView* textureView = nullptr;

	HRESULT result = DirectX::CreateWICTextureFromFile(
		m_device,
		m_deviceContext,
		pathObj.wstring().c_str(),
		nullptr,
		&textureView
	);

	if (FAILED(result))
	{
		Logger::LogError("Failed to load texture from file: ", filepath);
		return -1;
	}

	// Clear any existing texture with the same name or ID
	if (!UnloadTexture(id))
		UnloadTexture(textureName);

	m_textureResources[id] = textureView;
	m_textureIDs[textureName] = id;
	return id;
}

size_t StoryboardEngine::ResourceManager::LoadModel(const std::string& filepath, const std::string& modelName, size_t id)
{
	if (!m_modelImporter)
	{
		m_modelImporter = new Assimp::Importer();
		m_modelImporter->SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS,
			aiComponent_CAMERAS |
			aiComponent_LIGHTS |
			aiComponent_TEXTURES |
			aiComponent_ANIMATIONS);
		m_modelImporter->SetPropertyInteger(AI_CONFIG_PP_FD_REMOVE,
			aiPrimitiveType_POINT |
			aiPrimitiveType_LINE);
	}

	// Import with a bunch of flags
	const aiScene* importedModel = m_modelImporter->ReadFile(filepath,
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded |
		aiProcessPreset_TargetRealtime_Fast |
		aiProcess_TransformUVCoords |
		aiProcess_OptimizeGraph |
		aiProcess_OptimizeMeshes |
		aiProcess_PreTransformVertices |
		aiProcess_RemoveRedundantMaterials |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType);

	// If the import failed, report it
	if (!importedModel)
	{
		Logger::LogError(m_modelImporter->GetErrorString());
		return -1;
	}

	ModelData* model = ModelFactory::ConstructModel(m_device, importedModel);

	return LoadModel(modelName, model, id);
}

size_t StoryboardEngine::ResourceManager::LoadModel(const std::string& modelName, ModelData* model, size_t id)
{
	if (!model)
	{
		Logger::LogError("Cannot load null model for resource name: ", modelName);
		return -1;
	}

	// Clear any existing model with the same name or ID
	if (!UnloadModel(id))
		UnloadModel(modelName);

	m_modelResources[id] = model;
	m_modelIDs[modelName] = id;
	return id;
}

bool StoryboardEngine::ResourceManager::UnloadModel(const std::string& modelName)
{
	// Try to find the resource ID for the given resource name
	// If exists remove it from both maps and delete the model

	// Get the name:id map entry
	if (auto modelIt = m_modelIDs.find(modelName); modelIt != m_modelIDs.end())
	{

		// Get the id:model map entry and remove it
		if (auto resourceIt = m_modelResources.find(modelIt->second); resourceIt != m_modelResources.end())
		{
			delete resourceIt->second;
			m_modelResources.erase(resourceIt);
		}

		// Remove the name:id map entry
		m_modelIDs.erase(modelIt);
		return true;
	}

	return false;
}

bool StoryboardEngine::ResourceManager::UnloadModel(size_t modelId)
{
	// Try to find the model and name associated with the given resource ID, then remove both

	// Get the id:model map entry
	if (auto resourceIt = m_modelResources.find(modelId); resourceIt != m_modelResources.end())
	{
		// Get the name:id map entry and remove it
		for (auto it = m_modelIDs.begin(); it != m_modelIDs.end(); ++it)
		{
			if (it->second == modelId)
			{
				m_modelIDs.erase(it);
				break;
			}
		}

		// Remove the model from the id:model map and delete it
		delete resourceIt->second;
		m_modelResources.erase(resourceIt);
		return true;
	}

	return false;
}

bool StoryboardEngine::ResourceManager::UnloadTexture(const std::string& textureName)
{
	// Try to find the resource ID for the given resource name
	// If exists remove it from both maps and release the texture
	
	// Get the name:id map entry
	if (auto textureIt = m_textureIDs.find(textureName); textureIt != m_textureIDs.end())
	{
		// Get the id:texture map entry and remove it
		if (auto resourceIt = m_textureResources.find(textureIt->second); resourceIt != m_textureResources.end())
		{
			if (resourceIt->second)
			{
				resourceIt->second->Release();
			}
			m_textureResources.erase(resourceIt);
		}

		// Remove the name:id map entry
		m_textureIDs.erase(textureIt);
		return true;
	}
	return false;
}

bool StoryboardEngine::ResourceManager::UnloadTexture(size_t textureId)
{
	// Try to find the texture and name associated with the given resource ID, then remove both
	
	// Get the id:texture map entry
	if (auto resourceIt = m_textureResources.find(textureId); resourceIt != m_textureResources.end())
	{
		// Get the name:id map entry and remove it
		for (auto it = m_textureIDs.begin(); it != m_textureIDs.end(); ++it)
		{
			if (it->second == textureId)
			{
				m_textureIDs.erase(it);
				break;
			}
		}

		// Remove the texture from the id:texture map and release it
		if (resourceIt->second)
		{
			resourceIt->second->Release();
		}
		m_textureResources.erase(resourceIt);
		return true;
	}
	return false;
}

void StoryboardEngine::ResourceManager::ReleaseImporter()
{
	if (m_modelImporter)
	{
		delete m_modelImporter;
		m_modelImporter = nullptr;
	}
}

void StoryboardEngine::ResourceManager::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_device = device;
	m_deviceContext = deviceContext;

	CreateWhiteTexture();
}

void StoryboardEngine::ResourceManager::UpdateDevice(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_device = device;
	m_deviceContext = deviceContext;
}

void StoryboardEngine::ResourceManager::Shutdown()
{
	for (auto& [id, resource] : m_textureResources)
	{
		if (resource)
		{
			resource->Release();
		}
	}
	m_textureResources.clear();
	m_textureIDs.clear();

	for (auto& [id, model] : m_modelResources)
	{
		delete model;
	}
	m_modelResources.clear();
	m_modelIDs.clear();

	m_device = nullptr;
	m_deviceContext = nullptr;

	if (pTexture)
	{
		pTexture->Release();
	}

	ReleaseImporter();
}

void StoryboardEngine::ResourceManager::CreateWhiteTexture()
{
	// Credit to the DXFramework from CMP301 course at Abertay University for this function

	constexpr uint32_t s_pixel = 0xffffffff;
	ID3D11ShaderResourceView* texture = nullptr;

	D3D11_SUBRESOURCE_DATA initData = { &s_pixel, sizeof(uint32_t), 0 };

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = desc.Height = desc.MipLevels = desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	HRESULT hr = m_device->CreateTexture2D(&desc, &initData, &pTexture);

	if (SUCCEEDED(hr))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
		SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = 1;

		hr = m_device->CreateShaderResourceView(pTexture, &SRVDesc, &texture);
		m_textureResources[0] = texture;
		m_textureIDs["__default_texture__"] = 0;

		//pTexture->Release();
	}
}
