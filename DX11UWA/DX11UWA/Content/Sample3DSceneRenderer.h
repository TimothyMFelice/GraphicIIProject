#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "Model.h"


namespace DX11UWA
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources(void);
		void CreateWindowSizeDependentResources(void);
		void ReleaseDeviceDependentResources(void);
		void Update(DX::StepTimer const& timer);
		void Render(unsigned int viewport);
		void StartTracking(void);
		void TrackingUpdate(float positionX);
		void StopTracking(void);
		inline bool IsTracking(void) { return m_tracking; }

		// Helper functions for keyboard and mouse input
		void SetKeyboardButtons(const char* list);
		void SetMousePosition(const Windows::UI::Input::PointerPoint^ pos);
		void SetInputDeviceData(const char* kb, const Windows::UI::Input::PointerPoint^ pos);


	private:
		void Rotate(float radians);
		void UpdateCamera(DX::StepTimer const& timer, float const moveSpd, float const rotSpd);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// SamplerState
		Microsoft::WRL::ComPtr<ID3D11SamplerState>			m_samplerState;
		Microsoft::WRL::ComPtr<ID3D11SamplerState>			m_skyboxSamplerSate;

		// DIR Lighting Stuff
		Microsoft::WRL::ComPtr<ID3D11Buffer>			m_DIR_LightConstantBuffer;
		DIR_LIGHT										m_DIR_LightConstantBufferData;

		Microsoft::WRL::ComPtr<ID3D11Buffer>			m_POINT_LightConstantBuffer;
		POINT_LIGHT										m_POINT_LightConstantBufferData;

		Microsoft::WRL::ComPtr<ID3D11Buffer>			m_SPOT_LightConstantBuffer;
		SPOT_LIGHT										m_SPOT_LightConstantBufferData;

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		bool	m_PloadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		// Data members for keyboard and mouse input
		char	m_kbuttons[256];
		Windows::UI::Input::PointerPoint^ m_currMousePos;
		Windows::UI::Input::PointerPoint^ m_prevMousePos;

		// Matrix data member for the camera
		DirectX::XMFLOAT4X4 m_camera;

		//Model Infomation Goes HERE!
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_ModelinputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_ModelvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_ModelpixelShader;

		void DrawModel(ID3D11DeviceContext1* context, Model* model);
		void DrawInstanceModel(ID3D11DeviceContext1* context, Model * model);

		Model*										m_Plane;
		Model*										m_Pyramid;
		Model*										m_FuzzyGoomba;
		Model*										m_BattleAmbulance;
		Model*										m_KungFu_Panda;
		Model*										m_KungFu_Panda_Eye;
		Model*										m_Sphere;
			//Lighting Sphere
		Model*										m_Point_Sphere;
		Model*										m_Spot_Sphere;

		//Sky Box
		Model*										m_SkyBox;
		Model*										m_miniMapSkyBox;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_SkyBoxinputLayout;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_SkyBoxvertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_SkyBoxpixelShader;

		//instance Stuff
		Model*										m_instanceFuzzyGoomba;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_instanceConstantBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_instanceVertexShader;

	};
}

