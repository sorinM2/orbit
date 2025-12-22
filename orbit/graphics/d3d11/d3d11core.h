#pragma once 
#include <d3d11.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wrl/client.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace orbit::graphics::d3d11::core
{
	ID3D11Device* get_device();
	ID3D11DeviceContext* get_device_context();

	bool initialize();
	void update();
	void shutdown();
	void shutdown_device();
}