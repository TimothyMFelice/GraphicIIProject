#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include "Common\DDSTextureLoader.h"

using namespace DX11UWA;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	memset(m_kbuttons, 0, sizeof(m_kbuttons));
	m_currMousePos = nullptr;
	m_prevMousePos = nullptr;
	memset(&m_camera, 0, sizeof(XMFLOAT4X4));

	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources(void)
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100.0f);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(&m_constantBufferData.projection, XMMatrixTranspose(perspectiveMatrix * orientationMatrix));

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, 0.7f, -1.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, -0.1f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_camera, XMMatrixInverse(nullptr, XMMatrixLookAtLH(eye, at, up)));
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtLH(eye, at, up)));
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	// Convert degrees to radians, then convert seconds to rotation angle
	float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
	double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
	float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

	if (!m_tracking)
	{
		Rotate(radians);
	}

	// Update or move camera here
	UpdateCamera(timer, 3.0f, 0.75f);

//////////////////////////////////////////////////////////////
//Update Models
//////////////////////////////////////////////////////////////
	//Plane
	ModelViewProjectionConstantBuffer planeData;
	XMStoreFloat4x4(&planeData.model, XMMatrixTranspose(XMMatrixTranslation(0.0f, -1.0f, 0.0f)));
	m_Plane->SetConstBuffer(planeData);

	//Pyramid
	ModelViewProjectionConstantBuffer pyramidData;
	XMStoreFloat4x4(&pyramidData.model, XMMatrixTranspose(XMMatrixTranslation(3.0f, -0.2f, 0.0f)));
	m_Pyramid->SetConstBuffer(pyramidData);

	//Fuzzy Goomba
	ModelViewProjectionConstantBuffer FuzzyData;
	XMStoreFloat4x4(&FuzzyData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixRotationY(radians), XMMatrixTranslation(-3.0f, -0.6f, -1.5f))));
	m_FuzzyGoomba->SetConstBuffer(FuzzyData);

	//Battle Ambulance
	ModelViewProjectionConstantBuffer BattleData;
	XMStoreFloat4x4(&BattleData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(0.5f, 0.5f, 0.5f), XMMatrixMultiply(XMMatrixRotationY(radians), XMMatrixTranslation(5.0f, -0.6f, 10.0f)))));
	m_BattleAmbulance->SetConstBuffer(BattleData);

	//KungFu Panda
	ModelViewProjectionConstantBuffer PandaData;
	XMStoreFloat4x4(&PandaData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(1.5f, 1.5f, 1.5f), XMMatrixMultiply(XMMatrixRotationY(180.0f), XMMatrixTranslation(0.0f, -0.6f, 10.0f)))));
	m_KungFu_Panda->SetConstBuffer(PandaData);

	//KungFu Panda Eye
	ModelViewProjectionConstantBuffer PandaEyeData;
	XMStoreFloat4x4(&PandaEyeData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(1.5f, 1.5f, 1.5f), XMMatrixMultiply(XMMatrixRotationY(180.0f), XMMatrixTranslation(0.0f, -0.6f, 10.0f)))));
	m_KungFu_Panda_Eye->SetConstBuffer(PandaEyeData);

	//Sphere
	ModelViewProjectionConstantBuffer sphereData;
	XMStoreFloat4x4(&sphereData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(1.5f, 1.5f, 1.5f), XMMatrixTranslation(-5.0f, 1.0f, 5.0f))));
	m_Sphere->SetConstBuffer(sphereData);

	//Point Light Sphere
	ModelViewProjectionConstantBuffer pointLightData;
	XMStoreFloat4x4(&pointLightData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(0.4f, 0.4f, 0.4f), XMMatrixTranslation(7.0f, cos(radians) * 2.0f, 0.0f))));
	m_Point_Sphere->SetConstBuffer(pointLightData);

	//Spot Light Sphere
	ModelViewProjectionConstantBuffer spotLightData;
	XMStoreFloat4x4(&spotLightData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(0.4f, 0.4f, 0.4f), XMMatrixMultiply(XMMatrixRotationZ(radians), XMMatrixTranslation(-7.0f, cos(radians) * 2.0f, 0.0f)))));
	m_Spot_Sphere->SetConstBuffer(spotLightData);

	// Sky Box
	ModelViewProjectionConstantBuffer skyBoxData;
	XMStoreFloat4x4(&skyBoxData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(25.0f, 25.0f, 25.0f), XMMatrixTranslation(m_camera._41, m_camera._42, m_camera._43))));
	m_SkyBox->SetConstBuffer(skyBoxData);

	// Minimap Sky Box
	ModelViewProjectionConstantBuffer minimapSkyBoxData;
	XMStoreFloat4x4(&minimapSkyBoxData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(25.0f, 25.0f, 25.0f), XMMatrixTranslation(0.0f, 0.0f, 0.0f))));
	m_miniMapSkyBox->SetConstBuffer(minimapSkyBoxData);

	// Instance Model
	InstanceModelViewProjectionConstantBuffer instanceModelData;
	XMStoreFloat4x4(&instanceModelData.model[0], XMMatrixTranspose(XMMatrixMultiply(XMMatrixRotationY(radians), XMMatrixTranslation(-4.0f, -0.6f, -1.50f))));
	XMStoreFloat4x4(&instanceModelData.model[1], XMMatrixTranspose(XMMatrixMultiply(XMMatrixRotationY(radians), XMMatrixTranslation(-5.0f, -0.6f, -1.50f))));
	XMStoreFloat4x4(&instanceModelData.model[2], XMMatrixTranspose(XMMatrixMultiply(XMMatrixRotationY(radians), XMMatrixTranslation(-6.0f, -0.6f, -1.50f))));
	XMStoreFloat4x4(&instanceModelData.model[3], XMMatrixTranspose(XMMatrixMultiply(XMMatrixRotationY(radians), XMMatrixTranslation(-7.0f, -0.6f, -1.50f))));
	XMStoreFloat4x4(&instanceModelData.model[4], XMMatrixTranspose(XMMatrixMultiply(XMMatrixRotationY(radians), XMMatrixTranslation(-8.0f, -0.6f, -1.50f))));
	m_instanceFuzzyGoomba->SetInstanceConstBuffer(instanceModelData);

	// Render To Texture Plane
	ModelViewProjectionConstantBuffer RTTPlaneData;
	XMStoreFloat4x4(&RTTPlaneData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(0.2f, 0.2f, 0.2f), XMMatrixMultiply(XMMatrixRotationZ(3.14159f), XMMatrixMultiply(XMMatrixRotationX(1.5708f), XMMatrixTranslation(5.0f, 5.0f, 10.0f))))));
	m_RTTPlane->SetConstBuffer(RTTPlaneData);

	// Post Plane
	ModelViewProjectionConstantBuffer PostPlaneData;
	XMStoreFloat4x4(&PostPlaneData.model, XMMatrixTranspose(XMMatrixMultiply(XMMatrixScaling(0.2f, 0.2f, 0.2f), XMMatrixMultiply(XMMatrixRotationZ(3.14159f), XMMatrixMultiply(XMMatrixRotationX(1.5708f), XMMatrixTranslation(-5.0f, 5.0f, 10.0f))))));
	m_PostPlane->SetConstBuffer(PostPlaneData);

	//UPDATE LIGHT
		//Directional Light
	m_DIR_LightConstantBufferData.dir_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_DIR_LightConstantBufferData.dir = XMFLOAT4(cos(radians), sin(radians), 1.0f, 0.0f);
		// Point Light
	m_POINT_LightConstantBufferData.point_color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	m_POINT_LightConstantBufferData.point_pos = XMFLOAT4(7.0f, cos(radians) * 2.0f, 0.0f, 1.0f);
	m_POINT_LightConstantBufferData.point_radius = XMFLOAT4(10.0f, 10.0f, 10.0f, 1.0f);
		// Spot Light
	m_SPOT_LightConstantBufferData.spot_color		= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	m_SPOT_LightConstantBufferData.spot_pos			= XMFLOAT4(-7.0f, cos(radians) * 2.0f, 0.0f, 1.0f);
	m_SPOT_LightConstantBufferData.spot_coneDir		= XMFLOAT4(sin(radians), -1.0f, 0.0f, 1.0f);
	m_SPOT_LightConstantBufferData.spot_coneRatio	= XMFLOAT4(0.7f, 0.0f, 0.0f, 0.0f);
	m_SPOT_LightConstantBufferData.spot_InnerRatio  = XMFLOAT4(0.7f, 0.0f, 0.0f, 0.0f);
	m_SPOT_LightConstantBufferData.spot_OuterRatio  = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);

}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::UpdateCamera(DX::StepTimer const& timer, float const moveSpd, float const rotSpd)
{
	const float delta_time = (float)timer.GetElapsedSeconds();

	if (m_kbuttons['W'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['S'])
	{
		XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, -moveSpd * delta_time);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['A'])
	{
		XMMATRIX translation = XMMatrixTranslation(-moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['D'])
	{
		XMMATRIX translation = XMMatrixTranslation(moveSpd * delta_time, 0.0f, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons['X'])
	{
		XMMATRIX translation = XMMatrixTranslation( 0.0f, -moveSpd * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}
	if (m_kbuttons[VK_SPACE])
	{
		XMMATRIX translation = XMMatrixTranslation( 0.0f, moveSpd * delta_time, 0.0f);
		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
		XMStoreFloat4x4(&m_camera, result);
	}

	if (m_currMousePos) 
	{
		if (m_currMousePos->Properties->IsRightButtonPressed && m_prevMousePos)
		{
			float dx = m_currMousePos->Position.X - m_prevMousePos->Position.X;
			float dy = m_currMousePos->Position.Y - m_prevMousePos->Position.Y;

			XMFLOAT4 pos = XMFLOAT4(m_camera._41, m_camera._42, m_camera._43, m_camera._44);

			m_camera._41 = 0;
			m_camera._42 = 0;
			m_camera._43 = 0;

			XMMATRIX rotX = XMMatrixRotationX(dy * rotSpd * delta_time);
			XMMATRIX rotY = XMMatrixRotationY(dx * rotSpd * delta_time);

			XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
			temp_camera = XMMatrixMultiply(rotX, temp_camera);
			temp_camera = XMMatrixMultiply(temp_camera, rotY);

			XMStoreFloat4x4(&m_camera, temp_camera);

			m_camera._41 = pos.x;
			m_camera._42 = pos.y;
			m_camera._43 = pos.z;
		}
		m_prevMousePos = m_currMousePos;
	}


}

void Sample3DSceneRenderer::DrawModel(ID3D11DeviceContext1* context, Model * model)
{
	if (!model)
		return;

	ModelViewProjectionConstantBuffer newData = m_constantBufferData;
	newData.model = model->GetConstBufferData().model;
	XMVECTOR currCamPos = XMVectorSet(m_camera._41, m_camera._42, m_camera._43, 0.0f);
	XMStoreFloat4(&newData.camPos, currCamPos);

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &newData, 0, 0, 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionUVNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, model->GetAddressOfVertexBuffer(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(model->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(model->GetInputLayout());
	// Attach our vertex shader.
	context->VSSetShader(model->GetVertexShader(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(model->GetPixelShader(), nullptr, 0);
	// Attach the srv to the pixel shader
	context->PSSetShaderResources(0, 1, model->GetAddressOfShaderResourceView());
	context->PSSetShaderResources(1, 1, model->GetAddressOfShaderResourceViewNormal());
	context->PSSetShaderResources(2, 1, model->GetAddressOfShaderResourceViewSpecular());


	//DIR LIGHT
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_DIR_LightConstantBuffer.Get(), 0, NULL, &m_DIR_LightConstantBufferData, 0, 0, 0);
	// Send the constant buffer for lighting
	context->PSSetConstantBuffers1(0, 1, m_DIR_LightConstantBuffer.GetAddressOf(), nullptr, nullptr);
	//POINT LIGHT
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_POINT_LightConstantBuffer.Get(), 0, NULL, &m_POINT_LightConstantBufferData, 0, 0, 0);
	// Send the constant buffer for lighting
	context->PSSetConstantBuffers1(1, 1, m_POINT_LightConstantBuffer.GetAddressOf(), nullptr, nullptr);
	//SPOT LIGHT
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_SPOT_LightConstantBuffer.Get(), 0, NULL, &m_SPOT_LightConstantBufferData, 0, 0, 0);
	// Send the constant buffer for lighting
	context->PSSetConstantBuffers1(2, 1, m_SPOT_LightConstantBuffer.GetAddressOf(), nullptr, nullptr);


	// Draw the objects.
	context->DrawIndexed(model->GetNumIndicies(), 0, 0);
}

void Sample3DSceneRenderer::DrawInstanceModel(ID3D11DeviceContext1* context, Model * model)
{
	if (!model)
		return;

	InstanceModelViewProjectionConstantBuffer newData = model->GetInstanceConstBufferData();
	newData.view = m_constantBufferData.view;
	newData.projection = m_constantBufferData.projection;
	XMVECTOR currCamPos = XMVectorSet(m_camera._41, m_camera._42, m_camera._43, 0.0f);
	XMStoreFloat4(&newData.camPos, currCamPos);

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_instanceConstantBuffer.Get(), 0, NULL, &newData, 0, 0, 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionUVNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, model->GetAddressOfVertexBuffer(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(model->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(model->GetInputLayout());
	// Attach our vertex shader.
	context->VSSetShader(model->GetVertexShader(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_instanceConstantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(model->GetPixelShader(), nullptr, 0);
	// Attach the srv to the pixel shader
	context->PSSetShaderResources(0, 1, model->GetAddressOfShaderResourceView());
	context->PSSetShaderResources(1, 1, model->GetAddressOfShaderResourceViewNormal());
	context->PSSetShaderResources(2, 1, model->GetAddressOfShaderResourceViewSpecular());


	//DIR LIGHT
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_DIR_LightConstantBuffer.Get(), 0, NULL, &m_DIR_LightConstantBufferData, 0, 0, 0);
	// Send the constant buffer for lighting
	context->PSSetConstantBuffers1(0, 1, m_DIR_LightConstantBuffer.GetAddressOf(), nullptr, nullptr);
	//POINT LIGHT
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_POINT_LightConstantBuffer.Get(), 0, NULL, &m_POINT_LightConstantBufferData, 0, 0, 0);
	// Send the constant buffer for lighting
	context->PSSetConstantBuffers1(1, 1, m_POINT_LightConstantBuffer.GetAddressOf(), nullptr, nullptr);
	//SPOT LIGHT
	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_SPOT_LightConstantBuffer.Get(), 0, NULL, &m_SPOT_LightConstantBufferData, 0, 0, 0);
	// Send the constant buffer for lighting
	context->PSSetConstantBuffers1(2, 1, m_SPOT_LightConstantBuffer.GetAddressOf(), nullptr, nullptr);


	// Draw the objects.
	context->DrawIndexedInstanced(model->GetNumIndicies(), 5, 0, 0, 0);
}

void Sample3DSceneRenderer::DrawGeoShaderObject(ID3D11DeviceContext1 * context)
{
	InstanceModelViewProjectionConstantBuffer newData;
	newData.projection = m_constantBufferData.projection;
	newData.view = m_constantBufferData.view;
	// Model Matrix
	XMStoreFloat4x4(&newData.model[0], XMMatrixTranspose(XMMatrixTranslation(-3.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&newData.model[1], XMMatrixTranspose(XMMatrixTranslation(-4.0f, 0.0f, 0.0f)));
	XMStoreFloat4x4(&newData.model[2], XMMatrixTranspose(XMMatrixTranslation(-4.0f, 0.0f, 1.0f)));
	XMStoreFloat4x4(&newData.model[3], XMMatrixTranspose(XMMatrixTranslation(-3.0f, 0.0f, 1.0f)));
	XMStoreFloat4x4(&newData.model[4], XMMatrixTranspose(XMMatrixTranslation(-3.5f, 0.5f, 0.5f)));

	context->UpdateSubresource1(m_instanceConstantBuffer.Get(), 0, NULL, &newData, 0, 0, 0);
	UINT stride = sizeof(VertexPositionUVNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_GeoVertexBuffer, &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->IASetInputLayout(m_ModelinputLayout.Get());
	context->VSSetShader(m_GeoVertexShader.Get(), nullptr, 0);
	context->GSSetShader(m_GeoGeometryShader.Get(), nullptr, 0);
	context->GSSetConstantBuffers1(0, 1, m_instanceConstantBuffer.GetAddressOf(), nullptr, nullptr);
	context->PSSetShader(m_ModelpixelShader.Get(), nullptr, 0);

	//Geo Texture
	ID3D11ShaderResourceView* shaderResource;
	CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Assets/Textures/Wallbrick_Diffuse.dds", NULL, &shaderResource);
	context->PSSetShaderResources(0, 1, &shaderResource);

	context->DrawIndexedInstanced(1, 5, 0, 0, 0);

	ID3D11GeometryShader* nullShader = nullptr;
	context->GSSetShader(nullShader, nullptr, NULL);
}

void Sample3DSceneRenderer::DrawRTT(ID3D11DeviceContext1* context, Model * model)
{
	if (!model)
		return;

	ModelViewProjectionConstantBuffer newData;
	newData.projection = m_constantBufferData.projection;
	//XMStoreFloat4x4(&newData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMMatrixMultiply(XMMatrixRotationX(1.5708f), XMMatrixTranslation(5.0f, 5.f, 5.0f)))));
	newData.view = m_constantBufferData.view;
	newData.model = model->GetConstBufferData().model;
	XMVECTOR currCamPos = XMVectorSet(m_camera._41, m_camera._42, m_camera._43, 0.0f);
	XMStoreFloat4(&newData.camPos, currCamPos);

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_RTTConstantBuffer.Get(), 0, NULL, &newData, 0, 0, 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionUVNormal);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, model->GetAddressOfVertexBuffer(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(model->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(model->GetInputLayout());
	// Attach our vertex shader.
	context->VSSetShader(model->GetVertexShader(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_RTTConstantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(model->GetPixelShader(), nullptr, 0);
	// Attach the srv to the pixel shader
	context->PSSetShaderResources(0, 1, model->GetAddressOfShaderResourceView());

	// Draw the objects.
	context->DrawIndexed(model->GetNumIndicies(), 0, 0);
}

void Sample3DSceneRenderer::SetKeyboardButtons(const char* list)
{
	memcpy_s(m_kbuttons, sizeof(m_kbuttons), list, sizeof(m_kbuttons));
}

void Sample3DSceneRenderer::SetMousePosition(const Windows::UI::Input::PointerPoint^ pos)
{
	m_currMousePos = const_cast<Windows::UI::Input::PointerPoint^>(pos);
}

void Sample3DSceneRenderer::SetInputDeviceData(const char* kb, const Windows::UI::Input::PointerPoint^ pos)
{
	SetKeyboardButtons(kb);
	SetMousePosition(pos);
}

void DX11UWA::Sample3DSceneRenderer::StartTracking(void)
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking(void)
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	//SkyBox
	DrawModel(context, m_SkyBox);
	context->ClearDepthStencilView(m_deviceResources->GetRTTDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Prepare the constant buffer to send it to the graphics device.
	context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());
	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);
	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	// Set Sampler State
	context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	context->PSSetSamplers(1, 1, m_skyboxSamplerSate.GetAddressOf());
	context->PSSetSamplers(2, 1, m_SamplerSate2.GetAddressOf());
	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);
	
	//////////////////////////////////////////////////////////////
	//MODELS
	//////////////////////////////////////////////////////////////
	DrawModel(context, m_Plane);
	DrawModel(context, m_Pyramid);
	DrawModel(context, m_FuzzyGoomba);
	DrawModel(context, m_BattleAmbulance);
	DrawModel(context, m_KungFu_Panda);
	DrawModel(context, m_KungFu_Panda_Eye);
	DrawModel(context, m_Sphere);
		// Lighting Spheres
	DrawModel(context, m_Point_Sphere);
	DrawModel(context, m_Spot_Sphere);

	DrawInstanceModel(context, m_instanceFuzzyGoomba);

	//////////////////////////////////////////////////////////////
	//Geometry
	//////////////////////////////////////////////////////////////
	DrawGeoShaderObject(context);
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render2()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	//SkyBox
	DrawModel(context, m_SkyBox);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	context->PSSetSamplers(1, 1, m_skyboxSamplerSate.GetAddressOf());
	context->PSSetSamplers(2, 1, m_SamplerSate2.GetAddressOf());

	//////////////////////////////////////////////////////////////
	//MODELS
	//////////////////////////////////////////////////////////////
	DrawModel(context, m_Plane);
	DrawModel(context, m_Pyramid);
	DrawModel(context, m_FuzzyGoomba);
	DrawModel(context, m_BattleAmbulance);
	DrawModel(context, m_KungFu_Panda);
	DrawModel(context, m_KungFu_Panda_Eye);
	DrawModel(context, m_Sphere);
	// Lighting Spheres
	DrawModel(context, m_Point_Sphere);
	DrawModel(context, m_Spot_Sphere);

	DrawInstanceModel(context, m_instanceFuzzyGoomba);

	//////////////////////////////////////////////////////////////
	//Geometry
	//////////////////////////////////////////////////////////////
	DrawGeoShaderObject(context);

	//////////////////////////////////////////////////////////////
	//Render To Texture
	//////////////////////////////////////////////////////////////
	m_RTTPlane->SetShaderResourceView(m_deviceResources->GetRTTShaderResourceView());
	DrawRTT(context, m_RTTPlane);

	m_PostPlane->SetShaderResourceView(m_deviceResources->GetRTTShaderResourceView());
	DrawRTT(context, m_PostPlane);

	ID3D11ShaderResourceView *nullShader = { NULL };
	context->PSSetShaderResources(0, 1, &nullShader);
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::RenderMini()
{// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_camera))));
	//SkyBox
	DrawModel(context, m_SkyBox);
	context->ClearDepthStencilView(m_deviceResources->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	context->PSSetSamplers(1, 1, m_skyboxSamplerSate.GetAddressOf());
	context->PSSetSamplers(2, 1, m_SamplerSate2.GetAddressOf());

	//////////////////////////////////////////////////////////////
	//MODELS
	//////////////////////////////////////////////////////////////
	DrawModel(context, m_Plane);
	DrawModel(context, m_Pyramid);
	DrawModel(context, m_FuzzyGoomba);
	DrawModel(context, m_BattleAmbulance);
	DrawModel(context, m_KungFu_Panda);
	DrawModel(context, m_KungFu_Panda_Eye);
	DrawModel(context, m_Sphere);
	// Lighting Spheres
	DrawModel(context, m_Point_Sphere);
	DrawModel(context, m_Spot_Sphere);

	DrawInstanceModel(context, m_instanceFuzzyGoomba);

	//////////////////////////////////////////////////////////////
	//Geometry
	//////////////////////////////////////////////////////////////
	DrawGeoShaderObject(context);

	//////////////////////////////////////////////////////////////
	//Render To Texture
	//////////////////////////////////////////////////////////////
	m_RTTPlane->SetShaderResourceView(m_deviceResources->GetRTTShaderResourceView());
	DrawRTT(context, m_RTTPlane);

	ID3D11ShaderResourceView *nullShader = { NULL };
	context->PSSetShaderResources(0, 1, &nullShader);
}

void Sample3DSceneRenderer::CreateDeviceDependentResources(void)
{
	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_vertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_inputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_pixelShader));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer));
	});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this]()
	{
		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor cubeVertices[] =
		{
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
			{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer));

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices[] =
		{
			0,1,2, // -x
			1,3,2,

			4,6,5, // +x
			5,6,7,

			0,5,1, // -y
			0,4,5,

			2,7,6, // +y
			2,3,7,

			0,6,4, // -z
			0,2,6,

			1,7,3, // +z
			1,5,7,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = { 0 };
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer));
	});

//////////////////////////////////////////////////////////////
//Model Loader
//////////////////////////////////////////////////////////////
	// Load shaders asynchronously.
	auto loadModelVSTask = DX::ReadDataAsync(L"VS_Model_Shader.cso");
	auto loadModelPSTask = DX::ReadDataAsync(L"PS_Model_Shader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createModelVSTask = loadModelVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_ModelvertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_ModelinputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createModelPSTask = loadModelPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_ModelpixelShader));
	});

	auto modelLoading = (createModelVSTask && createModelPSTask).then([this]()
	{
		m_Pyramid = new Model();
		m_Pyramid->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/Pyramid.obj", L"Assets/Textures/Concrete_Diffuse.dds", L"Assets/Textures/Concrete_Normal.dds", L"Assets/Textures/Concrete_Specular.dds");
		m_Pyramid->SetInputLayout(m_ModelinputLayout.Get());
		m_Pyramid->SetVertexShader(m_ModelvertexShader.Get());
		m_Pyramid->SetPixelShader(m_ModelpixelShader.Get());

		m_FuzzyGoomba = new Model();
		m_FuzzyGoomba->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/FuzzyGoomba.obj", L"Assets/Textures/Fuzzy_Diffuse.dds", L"Assets/Textures/Fuzzy_Normal.dds", L"Assets/Textures/Fuzzy_Specular.dds");
		m_FuzzyGoomba->SetInputLayout(m_ModelinputLayout.Get());
		m_FuzzyGoomba->SetVertexShader(m_ModelvertexShader.Get());
		m_FuzzyGoomba->SetPixelShader(m_ModelpixelShader.Get());

		m_BattleAmbulance = new Model();
		m_BattleAmbulance->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/BattleAmbulance.obj", L"Assets/Textures/Ambulance_Diffuse.dds", L"Assets/Textures/Ambulance_Normal.dds", L"Assets/Textures/Ambulance_Specular.dds");
		m_BattleAmbulance->SetInputLayout(m_ModelinputLayout.Get());
		m_BattleAmbulance->SetVertexShader(m_ModelvertexShader.Get());
		m_BattleAmbulance->SetPixelShader(m_ModelpixelShader.Get());

		m_KungFu_Panda = new Model();
		m_KungFu_Panda->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/KungFu_Panda.obj", L"Assets/Textures/Panda_Diffuse.dds", L"Assets/Textures/Panda_Normal.dds", L"Assets/Textures/Panda_Specular.dds");
		m_KungFu_Panda->SetInputLayout(m_ModelinputLayout.Get());
		m_KungFu_Panda->SetVertexShader(m_ModelvertexShader.Get());
		m_KungFu_Panda->SetPixelShader(m_ModelpixelShader.Get());

		m_KungFu_Panda_Eye = new Model();
		m_KungFu_Panda_Eye->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/KungFu_Panda_Eyes.obj", L"Assets/Textures/Panda_Eye_Diffuse.dds", nullptr, nullptr);
		m_KungFu_Panda_Eye->SetInputLayout(m_ModelinputLayout.Get());
		m_KungFu_Panda_Eye->SetVertexShader(m_ModelvertexShader.Get());
		m_KungFu_Panda_Eye->SetPixelShader(m_ModelpixelShader.Get());

		m_Sphere = new Model();
		m_Sphere->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/Sphere.obj", L"Assets/Textures/Concrete_Diffuse.dds", L"Assets/Textures/Concrete_Normal.dds", L"Assets/Textures/Concrete_Specular.dds");
		m_Sphere->SetInputLayout(m_ModelinputLayout.Get());
		m_Sphere->SetVertexShader(m_ModelvertexShader.Get());
		m_Sphere->SetPixelShader(m_ModelpixelShader.Get());

		// Light Spheres
		m_Point_Sphere = new Model();
		m_Point_Sphere->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/Sphere.obj", L"Assets/Textures/Concrete_Diffuse.dds", L"Assets/Textures/Concrete_Normal.dds", L"Assets/Textures/Concrete_Specular.dds");
		m_Point_Sphere->SetInputLayout(m_ModelinputLayout.Get());
		m_Point_Sphere->SetVertexShader(m_ModelvertexShader.Get());
		m_Point_Sphere->SetPixelShader(m_ModelpixelShader.Get());

		m_Spot_Sphere = new Model();
		m_Spot_Sphere->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/Sphere.obj", L"Assets/Textures/Concrete_Diffuse.dds", L"Assets/Textures/Concrete_Normal.dds", L"Assets/Textures/Concrete_Specular.dds");
		m_Spot_Sphere->SetInputLayout(m_ModelinputLayout.Get());
		m_Spot_Sphere->SetVertexShader(m_ModelvertexShader.Get());
		m_Spot_Sphere->SetPixelShader(m_ModelpixelShader.Get());

		// PLANE
		m_Plane = new Model();
		m_Plane->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/Plane.obj", L"Assets/Textures/MidBoss_Floor_Diffuse.dds", L"Assets/Textures/MidBoss_Floor_Normal.dds", L"Assets/Textures/MidBoss_Floor_Specular.dds");
		m_Plane->SetInputLayout(m_ModelinputLayout.Get());
		m_Plane->SetVertexShader(m_ModelvertexShader.Get());
		m_Plane->SetPixelShader(m_ModelpixelShader.Get());

		//Sampler
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_deviceResources->GetD3DDevice()->CreateSamplerState(&samplerDesc, &m_samplerState);
	});

//////////////////////////////////////////////////////////////
//Instance
//////////////////////////////////////////////////////////////
	// Load shaders asynchronously.
	auto loadInstanceVSTask = DX::ReadDataAsync(L"VS_Instance.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createInstanceVSTask = loadInstanceVSTask.then([this](const std::vector<byte>& fileData)
	{
		CD3D11_BUFFER_DESC intanceConstantBufferDesc(sizeof(InstanceModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&intanceConstantBufferDesc, nullptr, &m_instanceConstantBuffer));

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_instanceVertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_ModelinputLayout));
	});

	auto instanceLoading = (createInstanceVSTask).then([this]()
	{
		m_instanceFuzzyGoomba = new Model();
		m_instanceFuzzyGoomba->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/FuzzyGoomba.obj", L"Assets/Textures/Fuzzy_Diffuse.dds", L"Assets/Textures/Fuzzy_Normal.dds", L"Assets/Textures/Fuzzy_Specular.dds");
		m_instanceFuzzyGoomba->SetInputLayout(m_ModelinputLayout.Get());
		m_instanceFuzzyGoomba->SetVertexShader(m_instanceVertexShader.Get());
		m_instanceFuzzyGoomba->SetPixelShader(m_ModelpixelShader.Get());
	});

//////////////////////////////////////////////////////////////
//Sky Box
//////////////////////////////////////////////////////////////
	// Load shaders asynchronously.
	auto loadSkyBoxVSTask = DX::ReadDataAsync(L"VS_Skybox.cso");
	auto loadSkyBoxPSTask = DX::ReadDataAsync(L"PS_Skybox.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createSkyboxVSTask = loadSkyBoxVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_SkyBoxvertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_SkyBoxinputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createSkyboxPSTask = loadSkyBoxPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_SkyBoxpixelShader));
	});

	auto skyboxLoading = (createSkyboxVSTask && createSkyboxPSTask).then([this]()
	{
		// Sky Box
		m_SkyBox = new Model();
		m_SkyBox->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/Cube.obj", L"Assets/Textures/Skybox_Alps.dds", nullptr, nullptr);
		m_SkyBox->SetInputLayout(m_SkyBoxinputLayout.Get());
		m_SkyBox->SetVertexShader(m_SkyBoxvertexShader.Get());
		m_SkyBox->SetPixelShader(m_SkyBoxpixelShader.Get());

		// Minimap Sky Box
		m_miniMapSkyBox = new Model();
		m_miniMapSkyBox->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/Cube.obj", L"Assets/Textures/Skybox_Alps.dds", nullptr, nullptr);
		m_miniMapSkyBox->SetInputLayout(m_SkyBoxinputLayout.Get());
		m_miniMapSkyBox->SetVertexShader(m_SkyBoxvertexShader.Get());
		m_miniMapSkyBox->SetPixelShader(m_SkyBoxpixelShader.Get());

		//Sampler
		D3D11_SAMPLER_DESC skyboxSamplerDesc;
		skyboxSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		skyboxSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		skyboxSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		skyboxSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		skyboxSamplerDesc.MipLODBias = 0.0f;
		skyboxSamplerDesc.MaxAnisotropy = 1;
		skyboxSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		skyboxSamplerDesc.BorderColor[0] = 0.0f;
		skyboxSamplerDesc.BorderColor[1] = 0.0f;
		skyboxSamplerDesc.BorderColor[2] = 0.0f;
		skyboxSamplerDesc.BorderColor[3] = 0.0f;
		skyboxSamplerDesc.MinLOD = 0;
		skyboxSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_deviceResources->GetD3DDevice()->CreateSamplerState(&skyboxSamplerDesc, &m_skyboxSamplerSate);
	});

	//Lighting
	CD3D11_BUFFER_DESC DIR_LightConstantBufferDesc(sizeof(DIR_LIGHT), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&DIR_LightConstantBufferDesc, nullptr, &m_DIR_LightConstantBuffer));

	CD3D11_BUFFER_DESC POINT_LightConstantBufferDesc(sizeof(POINT_LIGHT), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&POINT_LightConstantBufferDesc, nullptr, &m_POINT_LightConstantBuffer));

	CD3D11_BUFFER_DESC SPOT_LightConstantBufferDesc(sizeof(SPOT_LIGHT), D3D11_BIND_CONSTANT_BUFFER);
	DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&SPOT_LightConstantBufferDesc, nullptr, &m_SPOT_LightConstantBuffer));

//////////////////////////////////////////////////////////////
//Geo Shader
//////////////////////////////////////////////////////////////
	auto loadGeoVSTask = DX::ReadDataAsync(L"VS_NDC.cso");
	auto loadGeoGSTask = DX::ReadDataAsync(L"GS_NDC.cso");

	auto createGeoVSTask = loadGeoVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_GeoVertexShader));
	});

	auto createGeoGSTask = loadGeoGSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateGeometryShader(&fileData[0], fileData.size(), nullptr, &m_GeoGeometryShader));
	});

	auto createPointTask = (createGeoVSTask && createGeoGSTask).then([this]()
	{
		VertexPositionUVNormal point;
		point.pos		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		point.uv		= XMFLOAT3(0.0f, 0.0f, 0.0f);
		point.normal	= XMFLOAT3(0.0f, 0.0f, 0.0f);

		D3D11_SUBRESOURCE_DATA vertexBufferData = { 0 };
		vertexBufferData.pSysMem = &point;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(DX11UWA::VertexPositionUVNormal), D3D11_BIND_VERTEX_BUFFER);

		m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_GeoVertexBuffer);
	});

//////////////////////////////////////////////////////////////
//Render To Texture
//////////////////////////////////////////////////////////////
	// Load shaders asynchronously.
	auto loadRTTVSTask = DX::ReadDataAsync(L"VS_RTT.cso");
	auto loadRTTPSTask = DX::ReadDataAsync(L"PS_RTT.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createRTTVSTask = loadRTTVSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateVertexShader(&fileData[0], fileData.size(), nullptr, &m_RTTVertexShader));

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateInputLayout(vertexDesc, ARRAYSIZE(vertexDesc), &fileData[0], fileData.size(), &m_RTTInputLayout));
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createRTTPSTask = loadRTTPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_RTTPixelShader));

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_RTTConstantBuffer));
	});

	auto rttLoading = (createRTTVSTask && createRTTPSTask).then([this]()
	{
		// PLANE
		m_RTTPlane = new Model();
		m_RTTPlane->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/Plane.obj", L"Assets/Textures/Concrete_Diffuse.dds", nullptr, nullptr);
		m_RTTPlane->SetInputLayout(m_RTTInputLayout.Get());
		m_RTTPlane->SetVertexShader(m_RTTVertexShader.Get());
		m_RTTPlane->SetPixelShader(m_RTTPixelShader.Get());

		//Sampler
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_deviceResources->GetD3DDevice()->CreateSamplerState(&samplerDesc, &m_SamplerSate2);
	});

//////////////////////////////////////////////////////////////
// Post Process
//////////////////////////////////////////////////////////////
	// Load shaders asynchronously.
	auto loadPostPSTask = DX::ReadDataAsync(L"PS_Post.cso");

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPostPSTask = loadPostPSTask.then([this](const std::vector<byte>& fileData)
	{
		DX::ThrowIfFailed(m_deviceResources->GetD3DDevice()->CreatePixelShader(&fileData[0], fileData.size(), nullptr, &m_PostPixelShader));
	});

	auto postLoading = (createPostPSTask).then([this]()
	{
		// PLANE
		m_PostPlane = new Model();
		m_PostPlane->LoadOBJFromFile(m_deviceResources->GetD3DDevice(), "Assets/Models/Plane.obj", L"Assets/Textures/Concrete_Diffuse.dds", nullptr, nullptr);
		m_PostPlane->SetInputLayout(m_RTTInputLayout.Get());
		m_PostPlane->SetVertexShader(m_RTTVertexShader.Get());
		m_PostPlane->SetPixelShader(m_PostPixelShader.Get());

		//Sampler
		/*D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0.0f;
		samplerDesc.BorderColor[1] = 0.0f;
		samplerDesc.BorderColor[2] = 0.0f;
		samplerDesc.BorderColor[3] = 0.0f;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		m_deviceResources->GetD3DDevice()->CreateSamplerState(&samplerDesc, &m_samplerState);*/
	});


	// Once the cube is loaded, the object is ready to be rendered.
	modelLoading.then([this]()
	{
		m_loadingComplete = true;
	});

}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources(void)
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();

	m_samplerState.Reset();
}