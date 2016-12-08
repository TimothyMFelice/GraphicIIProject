//////////////////////////////////////////////////////////////
// Model Class was created during
// Dan's offices hours
// 1:00pm - 5:00pm | Nov 30, 2016
//
// During which multiple students work with
// Dan to come up with a model loader
//////////////////////////////////////////////////////////////

#pragma once

#include "Content\ShaderStructures.h"
#include <wrl.h>
#include <wrl/client.h>
#include <vector>
#include <d3d11.h>

class Model
{
private:
	// Data members
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_VertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>				m_IndexBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>			m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>			m_VertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>			m_PixelShader;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_ShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_NormalShaderResourceView;

	std::vector<DX11UWA::VertexPositionUVNormal>		m_InterleavedVertices;
	std::vector<unsigned int>							m_InterleavedIndices;

	unsigned int										m_NumIndices;

	DX11UWA::ModelViewProjectionConstantBuffer			m_ConstBufferData;
	DX11UWA::InstanceModelViewProjectionConstantBuffer	m_instanceConstBufferData;


	// Private functions
	bool LoadBuffers(const ID3D11Device* device);
	bool LoadTexture(const ID3D11Device* device, const wchar_t* szTextureName);
	bool LoadNormalTexture(const ID3D11Device* device, const wchar_t * szNormalTextureName);

	// Tangent Function
	bool CreateTangents(void);

public:
	// Class trilogy
	Model(void);
	~Model(void);
	Model(const Model& copy);
	Model& operator=(const Model& copy);

	// Public Functions
	bool LoadOBJFromFile(const ID3D11Device* device, const char* szFileName, const wchar_t* szTextureName, const wchar_t * szNormalTextureName);

	// Accessors
	inline ID3D11Buffer* GetVertexBuffer(void) { return m_VertexBuffer.Get(); }
	inline ID3D11Buffer* GetIndexBuffer(void) { return m_IndexBuffer.Get(); }
	inline ID3D11InputLayout* GetInputLayout(void) { return m_inputLayout.Get(); }
	inline ID3D11VertexShader* GetVertexShader(void) { return m_VertexShader.Get(); }
	inline ID3D11PixelShader* GetPixelShader(void) { return m_PixelShader.Get(); }
	inline ID3D11ShaderResourceView* GetShaderResourceView(void) { return m_ShaderResourceView.Get(); }
	inline ID3D11ShaderResourceView* GetShaderResourceViewNormal(void) { return m_NormalShaderResourceView.Get(); }

	inline ID3D11Buffer** GetAddressOfVertexBuffer(void) { return m_VertexBuffer.GetAddressOf(); }
	inline ID3D11Buffer** GetAddressOfIndexBuffer(void) { return m_IndexBuffer.GetAddressOf(); }
	inline ID3D11InputLayout** GetAddressOfInputLayout(void) { return m_inputLayout.GetAddressOf(); }
	inline ID3D11VertexShader** GetAddressOfVertexShader(void) { return m_VertexShader.GetAddressOf(); }
	inline ID3D11ShaderResourceView** GetAddressOfShaderResourceView(void) { return m_ShaderResourceView.GetAddressOf(); }
	inline ID3D11ShaderResourceView** GetAddressOfShaderResourceViewNormal(void) { return m_NormalShaderResourceView.GetAddressOf(); }

	inline unsigned int GetNumIndicies(void) { return m_NumIndices; }

	inline DX11UWA::ModelViewProjectionConstantBuffer GetConstBufferData(void) { return m_ConstBufferData; }
	inline DX11UWA::InstanceModelViewProjectionConstantBuffer GetInstanceConstBufferData(void) { return m_instanceConstBufferData; }

	// Mutators
	inline void SetVertexBuffer(const ID3D11Buffer* vertBuffer) { this->m_VertexBuffer = const_cast<ID3D11Buffer*>(vertBuffer); }
	inline void SetIndexBuffer(const ID3D11Buffer* indexBuffer) { this->m_IndexBuffer = const_cast<ID3D11Buffer*>(indexBuffer); }
	inline void SetInputLayout(const ID3D11InputLayout* inputLayout) { this->m_inputLayout = const_cast<ID3D11InputLayout*>(inputLayout); }
	inline void SetVertexShader(const ID3D11VertexShader* shader) { this->m_VertexShader = const_cast<ID3D11VertexShader*>(shader); }
	inline void SetPixelShader(const ID3D11PixelShader* shader) { this->m_PixelShader = const_cast<ID3D11PixelShader*>(shader); }
	inline void SetShaderResourceView(const ID3D11ShaderResourceView* srv) { this->m_ShaderResourceView = const_cast<ID3D11ShaderResourceView*>(srv); }

	inline void SetConstBuffer(DX11UWA::ModelViewProjectionConstantBuffer data) { this->m_ConstBufferData = data; }
	inline void SetInstanceConstBuffer(DX11UWA::InstanceModelViewProjectionConstantBuffer data) { this->m_instanceConstBufferData = data; }

};