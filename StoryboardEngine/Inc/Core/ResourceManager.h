#pragma once
#include <unordered_map>
#include <string>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

namespace Assimp
{
	class Importer;
}

namespace StoryboardEngine
{
	class ModelData;

	class ResourceManager final
	{
	public:
		// Gets a texture id for the resource with the given name. Should be cached during runtime.
		static size_t GetTextureID(const std::string& textureName);
		// Gets a texture name for the resource with the given id. Should be cached during runtime.
		static const std::string& GetTextureName(size_t resourceID);
		// Gets a list of all loaded texture names.
		static std::vector<std::string> GetAllTextureNames();
		// Gets a model id for the resource with the given name. Should be cached during runtime.
		static size_t GetModelID(const std::string& modelName);
		// Gets a model name for the resource with the given id. Should be cached during runtime.
		static const std::string& GetModelName(size_t resourceID);
		// Gets a list of all loaded model names.
		static std::vector<std::string> GetAllModelNames();
		// Gets the texture resource for the given resource ID.
		static ID3D11ShaderResourceView* GetTextureResource(size_t resourceID);
		// Gets the model resource for the given resource ID.
		static ModelData* GetModelResource(size_t resourceID);
		// Loads a texture from file and assigns it the given texture name. Returns the resource ID of the loaded texture.
		static size_t LoadTexture(const std::string& filePath, const std::string& textureName);
		// Loads a model from file and assigns it the given model name. Returns the resource ID of the loaded model.
		static size_t LoadModel(const std::string& filePath, const std::string& modelName);

		// Attempts to unload the model with the given resource name. Returns true if successful.
		static bool UnloadModel(const std::string& resourceName);
		// Attempts to unload the model with the given resource ID. Returns true if successful.
		static bool UnloadModel(size_t resourceID);
		// Attempts to unload the texture with the given resource name. Returns true if successful.
		static bool UnloadTexture(const std::string& resourceName);
		// Attempts to unload the texture with the given resource ID. Returns true if successful.
		static bool UnloadTexture(size_t resourceID);

		// Releases the model importer. Should be called once importing is complete.
		static void ReleaseImporter();
	protected:
		// Loads a texture from file and assigns it the given texture name and given id. Returns the resource ID of the loaded texture.
		static size_t LoadTexture(const std::string& filepath, const std::string& textureName, size_t id);
		// Loads a model from file and assigns it the given model name and given id. Returns the resource ID of the loaded model.
		static size_t LoadModel(const std::string& filepath, const std::string& modelName, size_t id);
		// Loads a model and assigns it the given model name and given id. Claims ownership of the model pointer. Returns the resource ID of the loaded model.
		static size_t LoadModel(const std::string& modelName, ModelData* model, size_t id);

		static void Initialize(ID3D11Device*, ID3D11DeviceContext*);
		static void UpdateDevice(ID3D11Device*, ID3D11DeviceContext*);
		static void Shutdown();

		friend class ApplicationCore;
	private:
		ResourceManager() = delete;

		static void CreateWhiteTexture();

		static std::unordered_map<size_t, ID3D11ShaderResourceView*> m_textureResources;
		static std::unordered_map<size_t, ModelData*> m_modelResources;
		static std::unordered_map<std::string, size_t> m_textureIDs;
		static std::unordered_map<std::string, size_t> m_modelIDs;

		static ID3D11Device* m_device;
		static ID3D11DeviceContext* m_deviceContext;

		static Assimp::Importer* m_modelImporter;
		static ID3D11Texture2D* pTexture;
	};
}
