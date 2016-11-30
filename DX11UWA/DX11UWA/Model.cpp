#include "pch.h"
#include "Model.h"
#include <vector>
#include <DirectXMath.h>

using namespace DirectX;


Model::Model(void)
{
	m_VertexBuffer = nullptr;
	m_IndexBuffer = nullptr;
	m_inputLayout = nullptr;
	m_VertexShader = nullptr;
	m_PixelShader = nullptr;
	m_ShaderResourceView = nullptr;

	m_InterleavedVertices.clear();
	m_InterleavedIndices.clear();

	m_NumIndices = 0;
}

Model::~Model(void)
{
	m_VertexBuffer.Reset();
	m_IndexBuffer.Reset();
	m_inputLayout.Reset();
	m_VertexShader.Reset();
	m_PixelShader.Reset();
	m_ShaderResourceView.Reset();

	m_InterleavedVertices.clear();
	m_InterleavedIndices.clear();

	m_NumIndices = 0;
}

Model::Model(const Model & copy)
{
	this->m_VertexBuffer = copy.m_VertexBuffer;
	this->m_IndexBuffer = copy.m_IndexBuffer;
	this->m_inputLayout = copy.m_inputLayout;
	this->m_VertexShader = copy.m_VertexShader;
	this->m_PixelShader = copy.m_PixelShader;

	this->m_ShaderResourceView = copy.m_ShaderResourceView;
	this->m_InterleavedVertices = copy.m_InterleavedVertices;
	this->m_InterleavedIndices = copy.m_InterleavedIndices;

	this->m_NumIndices = copy.m_NumIndices;
}

Model & Model::operator=(const Model & copy)
{
	if (this != &copy)
	{

		this->m_VertexBuffer = copy.m_VertexBuffer;
		this->m_IndexBuffer = copy.m_IndexBuffer;
		this->m_inputLayout = copy.m_inputLayout;
		this->m_VertexShader = copy.m_VertexShader;
		this->m_PixelShader = copy.m_PixelShader;

		this->m_ShaderResourceView = copy.m_ShaderResourceView;
		this->m_InterleavedVertices = copy.m_InterleavedVertices;
		this->m_InterleavedIndices = copy.m_InterleavedIndices;

		this->m_NumIndices = copy.m_NumIndices;
	}

	return *this;
}

bool Model::LoadBuffers(const ID3D11Device* device)
{
	HRESULT hr = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
	vertexBufferData.pSysMem = this->m_InterleavedVertices.data();
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(DX11UWA::VertexPositionUVNormal) * this->m_InterleavedVertices.size(), D3D11_BIND_VERTEX_BUFFER);
	hr = const_cast<ID3D11Device*>(device)->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->m_VertexBuffer.GetAddressOf());

	if (FAILED(hr))
		return FALSE;

	D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
	indexBufferData.pSysMem = this->m_InterleavedIndices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(unsigned int) * this->m_InterleavedIndices.size(), D3D11_BIND_INDEX_BUFFER);
	hr = const_cast<ID3D11Device*>(device)->CreateBuffer(&indexBufferDesc, &indexBufferData, this->m_IndexBuffer.GetAddressOf());

	this->m_NumIndices = (unsigned int)this->m_InterleavedIndices.size();

	if (FAILED(hr))
		return FALSE;

	return TRUE;
}

bool Model::LoadTexture(const ID3D11Device* device, const char * szTextureName)
{
	return TRUE;
}

bool Model::LoadOBJFromFile(const ID3D11Device* device, const char * szFileName, const char * szTextureName)
{
	std::vector<XMFLOAT3>		m_vVertices,	 m_vTexcoords,		  m_vNormals;
	std::vector<unsigned int>	m_vVertIndicies, m_vTexcoordIndicies, m_vNormIndicies;

	FILE * file;
	fopen_s(&file, szFileName, "r");

	if (file == NULL)
	{
		printf("Impossible to open the file!\n");
		return FALSE;
	}

	while (TRUE)
	{
		char lineHeader[128];

		int res = fscanf_s(file, "%s", lineHeader, 128);
		if (res == EOF)
			break;

		if (strcmp(lineHeader, "v") == 0)
		{
			XMFLOAT3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			m_vVertices.push_back(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			XMFLOAT3 uv;
			fscanf_s(file, "%f %f\n", &uv.x, &uv.y);
			m_vTexcoords.push_back(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			XMFLOAT3 normal;
			fscanf_s(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			m_vNormals.push_back(normal);
		}
		else if (strcmp(lineHeader, "f") == 0)
		{
			std::string vertex1, vertex2, vertex3;
			unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
			int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&vertexIndex[0], &uvIndex[0], &normalIndex[0],
				&vertexIndex[1], &uvIndex[1], &normalIndex[1],
				&vertexIndex[2], &uvIndex[2], &normalIndex[2]);

			if (matches != 9) {
				printf("File can't be read by our simple parser : ( Try exporting with other options\n");
				return false;
			}
			m_vVertIndicies.push_back(vertexIndex[0]);
			m_vVertIndicies.push_back(vertexIndex[1]);
			m_vVertIndicies.push_back(vertexIndex[2]);
			m_vTexcoordIndicies.push_back(uvIndex[0]);
			m_vTexcoordIndicies.push_back(uvIndex[1]);
			m_vTexcoordIndicies.push_back(uvIndex[2]);
			m_vNormIndicies.push_back(normalIndex[0]);
			m_vNormIndicies.push_back(normalIndex[1]);
			m_vNormIndicies.push_back(normalIndex[2]);
		}
	}

	int res = fclose(file);

	for (unsigned int i = 0; i < m_vVertIndicies.size(); i++)
	{
		DX11UWA::VertexPositionUVNormal tmp;
		tmp.pos = m_vVertices[m_vVertIndicies[i] - 1];
		tmp.uv = m_vTexcoords[m_vTexcoordIndicies[i] - 1];
		tmp.normal = m_vNormals[m_vNormIndicies[i] - 1];

		this->m_InterleavedVertices.push_back(tmp);
		this->m_InterleavedIndices.push_back(i);
	}

	if (!LoadBuffers(device))
		return FALSE;

	return TRUE;
}
