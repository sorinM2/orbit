#pragma once 
#include <d3d11.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "d3d11device.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace orbit::graphics::platform
{
	struct render_target_texture;
}

namespace orbit::graphics::d3d11::core
{
	ID3D11Device* get_device();
	ID3D11DeviceContext* get_device_context();

	bool initialize(bool runtime);
	void resize_swap_chain();
	void begin_frame();
	void begin_editor();
	void end_frame();
	void shutdown();
	void shutdown_device();

	platform::render_target_texture get_render_target_texture();

	enum class rtv_binding
	{
		swap_chain,
		texture
	};

	rendering_device* get_rendering_device();
	rendering_device_context* get_rendering_device_context();
}