#include "pch.h"
#include "Internal/PrimativeModel.h"

StoryboardEngine::PrimativeModel* StoryboardEngine::PrimativeModel::MakeTriangle(ID3D11Device* device)
{
    std::vector<VertexData> vertices =
    {
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }, // BL
        { DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }, // TM
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) } // BR
    };

    std::vector<unsigned long> indices = { 0, 1, 2 };

	return MakeCustom(device, vertices, indices);
}

StoryboardEngine::PrimativeModel* StoryboardEngine::PrimativeModel::MakeQuad(ID3D11Device* device)
{
    std::vector<VertexData> vertices =
    {
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }, // BL
        { DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }, // TL
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }, // BR
        { DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) } // TR
    };

    std::vector<unsigned long> indices = { 0, 1, 2, 2, 1, 3 };

	return MakeCustom(device, vertices, indices);
}

StoryboardEngine::PrimativeModel* StoryboardEngine::PrimativeModel::MakePlane(ID3D11Device* device, size_t quadRes)
{
    quadRes = std::max(1ULL, quadRes);
    size_t vertRes = quadRes + 1;

    float perQuad = 1.0f / vertRes;

    std::vector<VertexData> vertices;
    vertices.reserve(vertRes * vertRes);
    std::vector<unsigned long> indices;
    indices.reserve(quadRes * quadRes * 6);

    for (size_t j = 0; j < vertRes; ++j)
    {
        for (size_t i = 0; i < vertRes; ++i)
        {
            float x = -0.5f + j * perQuad;
            float y = 0.0f;
            float z = -0.5f + i * perQuad;
            vertices.push_back({ DirectX::XMFLOAT3(x, y, z), DirectX::XMFLOAT2(j * perQuad, i * perQuad), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) });

            if (i == quadRes || j == quadRes)
                continue;

            ULONG bottomLeft = static_cast<ULONG>(j * vertRes + i);
            ULONG bottomRight = static_cast<ULONG>(bottomLeft + 1);
            ULONG topLeft = static_cast<ULONG>(bottomLeft + vertRes);
            ULONG topRight = static_cast<ULONG>(topLeft + 1);
            indices.push_back(bottomLeft);
            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(bottomRight);
            indices.push_back(topLeft);
            indices.push_back(topRight);
        }
    }

	return MakeCustom(device, vertices, indices);
}

StoryboardEngine::PrimativeModel* StoryboardEngine::PrimativeModel::MakeCube(ID3D11Device* device)
{
    std::vector<VertexData> vertices =
    {
        // Front face
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }, // BL
        { DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }, // TL
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }, // BR
        { DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f) }, // TR
        // Back face
        { DirectX::XMFLOAT3(0.5f, -0.5f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // BL
        { DirectX::XMFLOAT3(0.5f, 0.5f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // TL
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // BR
        { DirectX::XMFLOAT3(-0.5f, 0.5f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) }, // TR
        // Left face
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) }, // BL
        { DirectX::XMFLOAT3(-0.5f, 0.5f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) }, // TL
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) }, // BR
        { DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f) }, // TR
        // Right face
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // BL
        { DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // TL
        { DirectX::XMFLOAT3(0.5f, -0.5f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // BR
        { DirectX::XMFLOAT3(0.5f, 0.5f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) }, // TR
        // Top face
        { DirectX::XMFLOAT3(-0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // BL
        { DirectX::XMFLOAT3(-0.5f, 0.5f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // TL
        { DirectX::XMFLOAT3(0.5f, 0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // BR
        { DirectX::XMFLOAT3(0.5f, 0.5f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) }, // TR
        // Bottom face
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) }, // BL
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) }, // TL
        { DirectX::XMFLOAT3(0.5f, -0.5f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) }, // BR
        { DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f) } // TR
    };

    std::vector<unsigned long> indices =
    {
        // Front face
        0, 1, 2,
        2, 1, 3,
        // Back face
        4, 5, 6,
        6, 5, 7,
        // Left face
        8, 9, 10,
        10, 9, 11,
        // Right face
        12, 13, 14,
        14, 13, 15,
        // Top face
        16, 17, 18,
        18, 17, 19,
        // Bottom face
        20, 21, 22,
        22, 21, 23
    };

	return MakeCustom(device, vertices, indices);
}

StoryboardEngine::PrimativeModel* StoryboardEngine::PrimativeModel::MakeCapsule(ID3D11Device* device, float aspectRatio, size_t latitudeLines, size_t longitudeLines)
{
    std::vector<VertexData> vertices;
    vertices.reserve((latitudeLines + 1) * (longitudeLines + 1));
    std::vector<unsigned long> indices;
    indices.reserve(latitudeLines * longitudeLines * 6);

    float halfHeight = aspectRatio >= 1.0f ? 0.5f : aspectRatio / 2.0f;
    float radius = aspectRatio >= 1.0f ? 1.0f / (2.0f * aspectRatio) : 0.5f;

    for (size_t pitch = 0; pitch <= latitudeLines; ++pitch)
    {
        float theta = pitch / static_cast<float>(latitudeLines) * DirectX::XM_PI;
        float sinTheta = sin(theta);
        float cosTheta = cos(theta);
        for (size_t yaw = 0; yaw <= longitudeLines; ++yaw)
        {
            float phi = yaw / static_cast<float>(longitudeLines) * DirectX::XM_2PI;
            float sinPhi = sin(phi);
            float cosPhi = cos(phi);
            float x = cosPhi * sinTheta * radius;
            float y = cosTheta * halfHeight;
            float z = sinPhi * sinTheta * radius;
            DirectX::XMFLOAT3 position(x, y, z);
            DirectX::XMFLOAT3 normal(x, y / aspectRatio, z);
            DirectX::XMFLOAT2 uv(static_cast<float>(yaw) / longitudeLines, static_cast<float>(pitch) / latitudeLines);
            vertices.push_back({ position, uv, normal });

            if (pitch == latitudeLines || yaw == longitudeLines)
                continue;

            ULONG bottomLeft = static_cast<ULONG>(pitch * (longitudeLines + 1) + yaw);
            ULONG bottomRight = static_cast<ULONG>(bottomLeft + 1);
            ULONG topLeft = static_cast<ULONG>(bottomLeft + (longitudeLines + 1));
            ULONG topRight = static_cast<ULONG>(topLeft + 1);
            indices.push_back(bottomLeft);
            indices.push_back(topLeft);
            indices.push_back(bottomRight);
            indices.push_back(bottomRight);
            indices.push_back(topLeft);
            indices.push_back(topRight);
        }
    }

	return MakeCustom(device, vertices, indices);
}

StoryboardEngine::PrimativeModel* StoryboardEngine::PrimativeModel::MakeCustom(ID3D11Device* device, std::vector<VertexData> vertices, std::vector<unsigned long> indices)
{
    if (vertices.empty() || indices.empty())
    {
        Logger::LogError("Error: Cannot create custom PrimativeModel, data appears absent.");
        return nullptr;
    }

    PrimativeModel* primative = new PrimativeModel();
	primative->Initialize(device, vertices, indices);

    return primative;
}