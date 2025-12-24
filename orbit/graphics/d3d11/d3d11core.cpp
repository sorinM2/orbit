#include "d3d11core.h"
#include "core/system.h"
#include <string>
#include <format>
#include "d3d11common.h"
#include "shaders/shader.h"
#include "orbit/graphics/d3d11/content/mesh.h"

namespace orbit::graphics::d3d11::core
{

	namespace
	{
		bool _vsync_enabled = false;
		int _video_card_memory;
		wchar_t _video_card_description[128];
		unsigned int vsync_numerator, vsync_denominator;
		D3D_FEATURE_LEVEL supported_feature_level;
	}

	namespace
	{
		IDXGISwapChain* _swap_chain = nullptr;
		ID3D11Device* _device = nullptr;
		ID3D11DeviceContext* _device_context = nullptr;
		ID3D11RenderTargetView* _render_target_view = nullptr;
		ID3D11Texture2D* _depth_stencil_buffer = nullptr;
		ID3D11DepthStencilState* _depth_stencil_state = nullptr;
		ID3D11DepthStencilView* _depth_stencil_view = nullptr;
		ID3D11RasterizerState* _rasterizer_state = nullptr;
		D3D11_VIEWPORT _viewport{};

		const D3D_FEATURE_LEVEL supported_feature_levels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		using idxgi_factory = IDXGIFactory7;
		using idxgi_adapter = IDXGIAdapter4;
		using idxgi_adapter_output = IDXGIOutput6;

		using idxgi_swap_chain = IDXGISwapChain4;
	}

	namespace 
	{
		shaders::shader color_shader;
		::orbit::graphics::d3d11::content::mesh triangle;
	}

	idxgi_adapter* get_best_adapter(idxgi_factory* dxgi_factory)
	{
		int adapter_index = 0, best_index = 0, best_index_feature_level = -1;
		idxgi_adapter* adapter = nullptr;

		while (dxgi_factory->EnumAdapters1(adapter_index, (IDXGIAdapter1**)&adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC3 adapter_desc;
			DXCALL(adapter->GetDesc3(&adapter_desc));

			D3D_FEATURE_LEVEL maximum_feature_level;

			DXCALL(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, NULL, supported_feature_levels, _countof(supported_feature_levels), D3D11_SDK_VERSION, NULL, &maximum_feature_level, NULL));

			for ( unsigned int i = 0; i < _countof(supported_feature_levels); ++i )
				if (supported_feature_levels[i] == maximum_feature_level and i > best_index_feature_level)
				{
					best_index_feature_level = i;
					best_index = adapter_index;
				}

			adapter->Release();
			adapter = nullptr;

			++adapter_index;
		}

		if (adapter_index == 0)
			return nullptr;

		DXCALL(dxgi_factory->EnumAdapters1(best_index, (IDXGIAdapter1**)&adapter));

		return adapter;
		
	}

	bool initialize_device(idxgi_adapter* adapter)
	{
		if (_device != nullptr)
			return true;

		unsigned int creation_flags = 0;

#ifdef _DEBUG
		creation_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		DXCALL(D3D11CreateDevice(adapter, D3D_DRIVER_TYPE_UNKNOWN, NULL, creation_flags, supported_feature_levels, _countof(supported_feature_levels), D3D11_SDK_VERSION, &_device, &supported_feature_level, &_device_context));
	
#ifdef _DEBUG

		ID3D11InfoQueue* info_queue = nullptr;
		DXCALL(_device->QueryInterface(IID_PPV_ARGS(&info_queue)));
		info_queue->GetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR);

		util::safe_release(info_queue);
#endif
	}

	bool initialize()
	{
		//main hwnd information
		D3D11_RECT window_rect;
		GetWindowRect(system::get_main_hwnd(), &window_rect);
		unsigned int window_width = window_rect.right - window_rect.left;
		unsigned int window_height = window_rect.bottom - window_rect.top;
		
		HWND main_hwnd = system::get_main_hwnd();
		HMONITOR hwnd_monitor = MonitorFromWindow(main_hwnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFOEX monitor_info;
		monitor_info.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hwnd_monitor, &monitor_info);

		DEVMODE dev_mode;
		dev_mode.dmSize = sizeof(dev_mode);
		EnumDisplaySettings(monitor_info.szDevice, ENUM_CURRENT_SETTINGS, &dev_mode);

		vsync_numerator = dev_mode.dmDisplayFrequency;
		vsync_denominator = 1;

		OutputDebugString(std::format("Creating swapchain:\n Monitor name: {}\n Monitor refresh rate: {} \n Monitor size: {}x{}\n", monitor_info.szDevice, vsync_numerator, window_width, window_height).c_str());

		//dxgi
		idxgi_factory* _dxgi_factory;
		DXCALL(CreateDXGIFactory1(__uuidof(idxgi_factory), (void**)&_dxgi_factory));

		idxgi_adapter* adapter = get_best_adapter(_dxgi_factory);

		DXGI_ADAPTER_DESC3 main_adapter_desc;
		DXCALL(adapter->GetDesc3(&main_adapter_desc));

		_video_card_memory = main_adapter_desc.DedicatedVideoMemory;
		wcsncpy(_video_card_description, main_adapter_desc.Description, 128);

		DXGI_SWAP_CHAIN_DESC swap_chain_desc;
		ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));

		swap_chain_desc.BufferDesc.Width = window_width;
		swap_chain_desc.BufferDesc.Height = window_height;
		
		if (!_vsync_enabled)
		{
			swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
			swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		}
		else
		{
			swap_chain_desc.BufferDesc.RefreshRate.Numerator = vsync_numerator;
			swap_chain_desc.BufferDesc.RefreshRate.Denominator = vsync_denominator;
		}

		swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;

		swap_chain_desc.BufferCount = 1;

		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.OutputWindow = system::get_main_hwnd();
		swap_chain_desc.Windowed = true;
		swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swap_chain_desc.Flags = 0;

		initialize_device(adapter);

		DXCALL(_dxgi_factory->CreateSwapChain(_device, &swap_chain_desc, &_swap_chain));
	
		util::safe_release(_dxgi_factory);
		util::safe_release(adapter);

		ID3D11Texture2D* back_buffer_ptr;

		DXCALL(_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer_ptr));
		DXCALL(_device->CreateRenderTargetView(back_buffer_ptr, NULL, &_render_target_view));
		util::safe_release(back_buffer_ptr);

		D3D11_TEXTURE2D_DESC depth_buffer_desc;
		depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_buffer_desc.Width = window_width;
		depth_buffer_desc.Height = window_height;
		depth_buffer_desc.SampleDesc.Count = 1;
		depth_buffer_desc.SampleDesc.Quality = 0;
		depth_buffer_desc.ArraySize = 1;
		depth_buffer_desc.MipLevels = 1;
		depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depth_buffer_desc.MiscFlags = 0;
		depth_buffer_desc.CPUAccessFlags = 0;

		DXCALL(_device->CreateTexture2D(&depth_buffer_desc, NULL, &_depth_stencil_buffer));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
		ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));

		depth_stencil_desc.DepthEnable = true;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		depth_stencil_desc.StencilEnable = false;

		DXCALL(_device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state));

		depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;

		DXCALL(_device->CreateDepthStencilView(_depth_stencil_buffer, &depth_stencil_view_desc, &_depth_stencil_view));

		D3D11_RASTERIZER_DESC rasterizer_desc;
		rasterizer_desc.CullMode = D3D11_CULL_NONE;
		rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		rasterizer_desc.FrontCounterClockwise = true;
		rasterizer_desc.AntialiasedLineEnable = false;
		rasterizer_desc.DepthBias = 0;
		rasterizer_desc.DepthBiasClamp = 0.f;
		rasterizer_desc.SlopeScaledDepthBias = 0.f;
		rasterizer_desc.DepthClipEnable = true;
		rasterizer_desc.ScissorEnable = false;
		rasterizer_desc.MultisampleEnable = false;

		DXCALL(_device->CreateRasterizerState(&rasterizer_desc, &_rasterizer_state));

		_viewport.MinDepth = 0.f;
		_viewport.MaxDepth = 1.f;
		_viewport.TopLeftX = 0;
		_viewport.TopLeftY = 0;
		_viewport.Width = window_width;
		_viewport.Height = window_height;


		_device_context->OMSetRenderTargets(1, &_render_target_view, _depth_stencil_view);
		_device_context->RSSetState(_rasterizer_state);
		_device_context->RSSetViewports(1, &_viewport);

		util::name_com_object(_depth_stencil_state, "deoth stencil");
		util::name_com_object(_device, "device 1");

		color_shader._pixel_shader_path = std::filesystem::path("src/color/color_ps.hlsl");
		color_shader._vertex_shader_path = std::filesystem::path("src/color/color_vs.hlsl");

		color_shader._vertex_shader_entry_point = "vs_main";
		color_shader._pixel_shader_entry_point = "ps_main";
		
		color_shader.initialize();

		triangle._indices.emplace_back(0);
		triangle._indices.emplace_back(1);
		triangle._indices.emplace_back(2);

		::orbit::graphics::d3d11::content::mesh::vertex v1, v2, v3;
		v1 = { glm::vec3(-1.f, -1.f, 0.f), glm::vec3(0.5f, 0.7f, 0.5f) };
		v2 = { glm::vec3(0.f, 1.f, 0.f) ,glm::vec3(1.f, 1.f, 0.1f) };
		v3 = { glm::vec3(1.f, -1.f, 0.f), glm::vec3(0.f, 0.2f, 1.f) };
		triangle._vertices.emplace_back(v1);
		triangle._vertices.emplace_back(v2);
		triangle._vertices.emplace_back(v3);

		triangle.initialize();
		return true;
	}

	void begin_frame(float red, float green, float blue, float alpha)
	{
		float color[4];

		color[0] = red;
		color[1] = green;
		color[2] = blue;
		color[3] = alpha;

		_device_context->ClearRenderTargetView(_render_target_view, color);
		_device_context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	}

	void end_frame()
	{
		if (_vsync_enabled)
			_swap_chain->Present(1, 0);
		else _swap_chain->Present(0, 0);
	}

	void update()
	{
		begin_frame(0.5f,0.2f, 0.2f, 1.f);
		color_shader.bind();
		triangle.bind_buffers();
		_device_context->DrawIndexed(3, 0, 0);
		end_frame();
	}

	void shutdown()
	{
		util::safe_release(_swap_chain);
		util::safe_release(_render_target_view);
		util::safe_release(_depth_stencil_buffer);
		util::safe_release(_depth_stencil_view);
		util::safe_release(_depth_stencil_state);
		util::safe_release(_rasterizer_state);
		_viewport = {};
	}

	void shutdown_device()
	{
		shutdown();

#ifdef _DEBUG
		ID3D11Debug* debug_controller;
		_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug_controller);
		debug_controller->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		util::safe_release(debug_controller);

#endif // _DEBUG

		util::safe_release(_device);
		util::safe_release(_device_context);
	}

	ID3D11Device* get_device()
	{
		return _device;
	}

	ID3D11DeviceContext* get_device_context()
	{
		return _device_context;
	}
}