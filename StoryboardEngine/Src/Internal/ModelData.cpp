#include "pch.h"
#include "Internal/ModelData.h"
#include <d3d11.h>
#include "Internal/ColourShader.h"

StoryboardEngine::ModelData::ModelData()
{
	m_vertexBuffer = nullptr;
	m_indexBuffer = nullptr;
	m_vertexCount = 0;
	m_indexCount = 0;
}

StoryboardEngine::ModelData::~ModelData()
{
	// Shutdown the vertex and index buffers.
	ShutdownBuffers();

	for (int i = 0; i < m_subModels.size(); ++i)
	{
		if (m_subModels[i])
		{
			delete m_subModels[i];
			m_subModels[i] = nullptr;
		}
	}

	return;
}

bool StoryboardEngine::ModelData::Initialize(ID3D11Device* device, std::vector<VertexData> vertices, std::vector<unsigned long> indices)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	if (vertices.empty() || indices.empty())
	{
		Logger::LogError("Error: Cannot initialize model, data appears absent.");
		return false;
	}

	// if buffers already exist, release them
	if (m_vertexBuffer)
	{
		ShutdownBuffers();
	}

	m_vertexCount = static_cast<int>(vertices.size());
	m_indexCount = static_cast<int>(indices.size());

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexData) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Create the vertex buffer
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		ShutdownBuffers();
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		ShutdownBuffers();
		return false;
	}

	return true;
}

void StoryboardEngine::ModelData::AddSubModel(ModelData* subModel)
{
	m_subModels.push_back(subModel);
}

void StoryboardEngine::ModelData::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers(deviceContext);

	for (auto* model : m_subModels)
	{
		model->Render(deviceContext);
	}

	return;
}

int StoryboardEngine::ModelData::GetIndexCount() const
{
	return m_indexCount;
}

void StoryboardEngine::ModelData::ShutdownBuffers()
{
	// Release the index buffer.
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = nullptr;
		m_indexCount = 0;
	}

	// Release the vertex buffer.
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = nullptr;
		m_vertexCount = 0;
	}

	return;
}

void StoryboardEngine::ModelData::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Verify that the buffers are valid.
	if (!m_vertexBuffer || !m_indexBuffer)
		return;


	// Set vertex buffer stride and offset.
	stride = sizeof(VertexData);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ColourShader::Render(deviceContext, m_indexCount);

	return;
}
