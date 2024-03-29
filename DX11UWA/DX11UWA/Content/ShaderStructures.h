﻿#pragma once

namespace DX11UWA
{
	// Constant buffer used to send MVP matrices to the vertex shader.
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 camPos;
	};

	// Constant buffer used to send MVP matrices to the vertex shader.
	struct InstanceModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model[5];
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4 camPos;
	};

	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	struct VertexPositionUVNormal
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 uv;
		DirectX::XMFLOAT3 normal;
		//ADDED TANGENT
		DirectX::XMFLOAT4 tangent;
	};

	struct VertexPositionUVNormalTagent
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 uv;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT4 tangent;
	};

	struct DIR_LIGHT
	{
		DirectX::XMFLOAT4 dir_color;
		DirectX::XMFLOAT4 dir;
	};

	struct POINT_LIGHT
	{
		DirectX::XMFLOAT4 point_color;
		DirectX::XMFLOAT4 point_pos;
		DirectX::XMFLOAT4 point_radius;
	};

	struct SPOT_LIGHT
	{
		DirectX::XMFLOAT4 spot_color;
		DirectX::XMFLOAT4 spot_pos;
		DirectX::XMFLOAT4 spot_coneDir;
		DirectX::XMFLOAT4 spot_coneRatio;
		DirectX::XMFLOAT4 spot_InnerRatio;
		DirectX::XMFLOAT4 spot_OuterRatio;
	};
}