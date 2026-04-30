#include "d3d11core.h"
#include "orbit/core/system.h"
#include <string>
#include <format>
#include "d3d11common.h"
#include "d3d11shader_resource.h"
#include "orbit/graphics/common/camera.h"
#include "../common/platform.h"
#include "shaders/shader.h"
#include "orbit/graphics/d3d11/content/mesh.h"

namespace orbit::graphics::d3d11::core
{



	namespace
	{
		int _render_target_width = 1920;
		int _render_target_height = 1080;
	}

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
		rendering_device* d3d11_device = nullptr;
		rendering_device_context* d3d11_context = nullptr;
		buffer* d3d11_buffer = nullptr;
		sampler* d3d11_sampler = nullptr;
		rasterizer* d3d11_rasterizer = nullptr;
		viewport viewport;

		swap_chain* swap_chain = nullptr;

		texture2D* render_target_texture = nullptr;
		render_target* render_target = nullptr;
		framebuffer* framebuffer = nullptr;
		shader_resource* shader_resource = nullptr;

		texture2D* depth_stencil_texture = nullptr;
		depth_stencil* depth_stencil = nullptr;
		depth_stencil_state* depth_stencil_state = nullptr;
	}
	rendering_device* get_rendering_device() { return d3d11_device; }
	rendering_device_context* get_rendering_device_context() { return d3d11_context; }
	namespace
	{
		//IDXGISwapChain* _swap_chain = nullptr;
		ID3D11Device* _device = nullptr;
		ID3D11DeviceContext* _device_context = nullptr;

		rtv_binding _rtv_binding = rtv_binding::swap_chain;
		// ID3D11Texture2D* _render_target = nullptr;
		// ID3D11ShaderResourceView* _render_target_srv = nullptr;
		// ID3D11RenderTargetView* _render_target_view = nullptr;
		// ID3D11RenderTargetView* _swap_chain_render_target_view = nullptr;
		//
		// ID3D11Texture2D* _depth_stencil_buffer = nullptr;
		// ID3D11DepthStencilState* _depth_stencil_state = nullptr;
		// ID3D11DepthStencilView* _depth_stencil_view = nullptr;

		//ID3D11RasterizerState* _rasterizer_state = nullptr;
		//D3D11_VIEWPORT _viewport{};
		//ID3D11SamplerState* _sampler_state = nullptr;

		//ID3D11Buffer* _camera_buffer;

		const D3D_FEATURE_LEVEL supported_feature_levels[] = {
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		using idxgi_factory = IDXGIFactory7;
		using idxgi_adapter = IDXGIAdapter4;
	}

	namespace 
	{
		shaders::shader color_shader;
	}

	platform::render_target_texture get_render_target_texture()
	{
		platform::render_target_texture result;
		result.width = _render_target_width;
		result.height = _render_target_height;
		result.texture = static_cast<d3d11_shader_resource*>(shader_resource)->get_internal();
		return result;
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

		return true;
	}

	void initialize_depth_stencil(int width, int height)
	{
		//util::safe_release(_depth_stencil_buffer);
		//util::safe_release(_depth_stencil_view);
		util::safe_release(depth_stencil_texture);
		util::safe_release(depth_stencil);

		// D3D11_TEXTURE2D_DESC depth_buffer_desc;
		// depth_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		// depth_buffer_desc.Width = width;
		// depth_buffer_desc.Height = height;
		// depth_buffer_desc.SampleDesc.Count = 1;
		// depth_buffer_desc.SampleDesc.Quality = 0;
		// depth_buffer_desc.ArraySize = 1;
		// depth_buffer_desc.MipLevels = 1;
		// depth_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		// depth_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		// depth_buffer_desc.MiscFlags = 0;
		// depth_buffer_desc.CPUAccessFlags = 0;
		//
		// DXCALL(_device->CreateTexture2D(&depth_buffer_desc, NULL, &_depth_stencil_buffer));

		texture2D_desc depth_buffer_desc;
		ZeroMemory(&depth_buffer_desc, sizeof(depth_buffer_desc));
		depth_buffer_desc.format = format::FORMAT_D24_UNORM_S8_UINT;
		depth_buffer_desc.width = width;
		depth_buffer_desc.height = height;
		depth_buffer_desc.sample_desc.count = 1;
		depth_buffer_desc.sample_desc.quality = 0;
		depth_buffer_desc.mips = 1;
		depth_buffer_desc.usage = resource_usage::resource_default_usage;
		depth_buffer_desc.bind_flags = bind_flag_depth_stencil;
		depth_buffer_desc.cpu_access_flags = 0;

		d3d11_device->create_texture2D(depth_buffer_desc, &depth_stencil_texture);

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
		ZeroMemory(&depth_stencil_view_desc, sizeof(depth_stencil_view_desc));

		// depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		// depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		// depth_stencil_view_desc.Texture2D.MipSlice = 0;
		//
		// DXCALL(_device->CreateDepthStencilView(_depth_stencil_buffer, &depth_stencil_view_desc, &_depth_stencil_view));

		depth_stencil_desc depth_stencil_desc;
		ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
		depth_stencil_desc.type = depth_stencil_type::texture2D;

		d3d11_device->create_depth_stencil(depth_stencil_desc, depth_stencil_texture, &depth_stencil);
	}

	void initialize_depth_stencil_for_swap_chain()
	{
		assert( swap_chain );
		swap_chain_desc sc_desc = swap_chain->get_desc();

		initialize_depth_stencil(sc_desc.buffer_desc.width, sc_desc.buffer_desc.height);
	}

	void initialize_depth_stencil_for_texture()
	{
		assert ( render_target );
		texture2D_desc rt_desc = render_target_texture->get_desc();

		initialize_depth_stencil(rt_desc.width, rt_desc.height);
	}

	void initialize_viewport(int width, int height)
	{

		viewport.x = 0;
		viewport.y = 0;
		viewport.width = width;
		viewport.height = height;

		d3d11_context->set_viewports(&viewport, 1);
	}

	void initialize_viewport_for_swap_chain()
	{
		assert( swap_chain );
		swap_chain_desc sc_desc = swap_chain->get_desc();

		initialize_viewport(sc_desc.buffer_desc.width, sc_desc.buffer_desc.height);
	}

	void initialize_viewport_for_texture()
	{
		initialize_viewport(_render_target_width, _render_target_height);
	}

	void initialize_rtv(texture2D* buffer, ::orbit::graphics::render_target*& rt)
	{
		// util::safe_release(rtv);
		// DXCALL(_device->CreateRenderTargetView(buffer, nullptr, &rtv));

		util::safe_release(rt);
		render_target_desc desc;
		desc.type = render_target_type::texture2D;
		d3d11_device->create_render_target(desc, buffer, &rt);
	}

	void initialize_rtv_for_texture(class render_target*& rt)
	{
		// D3D11_TEXTURE2D_DESC texture_desc;
		// ZeroMemory(&texture_desc, sizeof(texture_desc));
		//
		// texture_desc.Width = _render_target_width;
		// texture_desc.Height = _render_target_height;
		// texture_desc.MipLevels = 1;
		// texture_desc.ArraySize = 1;
		// texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		// texture_desc.SampleDesc.Count = 1;
		// texture_desc.SampleDesc.Quality = 0;
		// texture_desc.Usage = D3D11_USAGE_DEFAULT;
		// texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		// texture_desc.MiscFlags = 0;
		// texture_desc.CPUAccessFlags = 0;
		//
		// util::safe_release(_render_target);
		// _device->CreateTexture2D(&texture_desc, nullptr, &_render_target);

		util::safe_release(rt);
		util::safe_release(render_target_texture);

		texture2D_desc rtv_texture_desc;
		ZeroMemory(&rtv_texture_desc, sizeof(rtv_texture_desc));
		rtv_texture_desc.format = format::FORMAT_R8G8B8A8_UNORM;
		rtv_texture_desc.width = _render_target_width;
		rtv_texture_desc.height = _render_target_height;
		rtv_texture_desc.mips = 1;
		rtv_texture_desc.usage = resource_usage::resource_default_usage;
		rtv_texture_desc.sample_desc.count = 1;
		rtv_texture_desc.sample_desc.quality = 0;
		rtv_texture_desc.bind_flags = bind_flag_render_target | bind_flag_shader_resource;
		rtv_texture_desc.cpu_access_flags = 0;

		d3d11_device->create_texture2D(rtv_texture_desc, &render_target_texture);

		// D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
		// ZeroMemory(&srv_desc, sizeof(srv_desc));
		// srv_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		// srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		// srv_desc.Texture2D.MipLevels = texture_desc.MipLevels;
		// srv_desc.Texture2D.MostDetailedMip = 0;
		//
		// util::safe_release(_render_target_srv);
		// _device->CreateShaderResourceView(_render_target, &srv_desc, &_render_target_srv);

		shader_resource_desc shader_resource_desc;
		ZeroMemory(&shader_resource_desc, sizeof(shader_resource_desc));
		shader_resource_desc.type = shader_resource_type::texture2D;
		shader_resource_desc.format = format::FORMAT_R8G8B8A8_UNORM;
		util::safe_release(shader_resource);
		d3d11_device->create_shader_resource(shader_resource_desc, render_target_texture, &shader_resource);

		// assert(_render_target);
		// initialize_rtv(_render_target, rtv);

		assert(render_target_texture);
		initialize_rtv(render_target_texture, rt);
	}

	void resize_swap_chain()
	{
		// if ( !_swap_chain )
		// 	return;
		//
		// _device_context->OMSetRenderTargets(0, 0, 0);
		//
		// if ( _rtv_binding == rtv_binding::swap_chain)
		// 	util::safe_release(_render_target_view);
		//
		// util::safe_release(_swap_chain_render_target_view);
		//
		// DXCALL(_swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0));
		//
		// if ( _rtv_binding == rtv_binding::swap_chain)
		// {
		// 	initialize_viewport_for_swap_chain();
		// 	initialize_depth_stencil_for_swap_chain();
		// 	initialize_rtv_for_swap_chain(_render_target_view);
		// }
		// initialize_rtv_for_swap_chain(_swap_chain_render_target_view);
		//
		// _device_context->OMSetRenderTargets(1, &_render_target_view, _depth_stencil_view);
	}

	bool initialize(bool runtime)
	{
		if ( runtime )
			_rtv_binding = rtv_binding::swap_chain;
		else _rtv_binding = rtv_binding::texture;

		//main hwnd information
		D3D11_RECT window_rect;
		GetClientRect(system::get_main_hwnd(), &window_rect);
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

		//OutputDebugString(std::format("Creating swapchain:\n Monitor name: {}\n Monitor refresh rate: {} \n Monitor size: {}x{}\n", monitor_info.szDevice, vsync_numerator, window_width, window_height).c_str());

		//dxgi
		idxgi_factory* _dxgi_factory;
		DXCALL(CreateDXGIFactory1(__uuidof(idxgi_factory), (void**)&_dxgi_factory));

		idxgi_adapter* adapter = get_best_adapter(_dxgi_factory);

		DXGI_ADAPTER_DESC3 main_adapter_desc;
		DXCALL(adapter->GetDesc3(&main_adapter_desc));

		_video_card_memory = main_adapter_desc.DedicatedVideoMemory;
		wcsncpy(_video_card_description, main_adapter_desc.Description, 128);
		initialize_device(adapter);
		d3d11_device = new d3d11_rendering_device(_device);
		d3d11_context = new d3d11_rendering_device_context(_device_context);
		d3d11_device->set_context(d3d11_context);

		// DXGI_SWAP_CHAIN_DESC swap_chain_desc;
		// ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
		//
		// swap_chain_desc.BufferDesc.Width = window_width;
		// swap_chain_desc.BufferDesc.Height = window_height;
		//
		// if (!_vsync_enabled)
		// {
		// 	swap_chain_desc.BufferDesc.RefreshRate.Numerator = 0;
		// 	swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		// }
		// else
		// {
		// 	swap_chain_desc.BufferDesc.RefreshRate.Numerator = vsync_numerator;
		// 	swap_chain_desc.BufferDesc.RefreshRate.Denominator = vsync_denominator;
		// }
		//
		// swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		// swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		// swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		//
		// swap_chain_desc.SampleDesc.Count = 1;
		// swap_chain_desc.SampleDesc.Quality = 0;
		//
		// swap_chain_desc.BufferCount = 1;
		//
		// swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		// swap_chain_desc.OutputWindow = system::get_main_hwnd();
		// swap_chain_desc.Windowed = true;
		// swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		// swap_chain_desc.Flags = 0;


		//DXCALL(_dxgi_factory->CreateSwapChain(_device, &swap_chain_desc, &_swap_chain));

		swap_chain_desc swap_chain_desc;
		ZeroMemory(&swap_chain_desc, sizeof(swap_chain_desc));
		swap_chain_desc.buffer_desc.width = window_width;
		swap_chain_desc.buffer_desc.height = window_height;
		swap_chain_desc.buffer_desc.refresh_rate.numerator = vsync_numerator;
		swap_chain_desc.buffer_desc.refresh_rate.denominator = vsync_denominator;
		swap_chain_desc.buffer_desc.format = format::FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.buffer_desc.scaling = scaling_mode::unspecified;
		swap_chain_desc.vsync_enabled = true;

		swap_chain_desc.sample_desc.count = 1;
		swap_chain_desc.sample_desc.quality = 0;

		swap_chain_desc.buffer_count = 1;

		swap_chain_desc.buffer_usage = render_target_output | shader_input;
		swap_chain_desc.windowed = true;

		swap_chain = new d3d11_swap_chain(d3d11_device, d3d11_context, swap_chain_desc, _dxgi_factory);


		util::safe_release(_dxgi_factory);
		util::safe_release(adapter);


		if ( _rtv_binding == rtv_binding::swap_chain )
		{
			//initialize_rtv_for_swap_chain(render_target);
		}
		else initialize_rtv_for_texture(render_target);
		framebuffer_desc fb_desc;
		ZeroMemory(&fb_desc, sizeof(fb_desc));
		fb_desc.count = 1;
		d3d11_device->create_framebuffer(fb_desc, &render_target, &framebuffer);

		//initialize_rtv_for_swap_chain(_swap_chain_render_target_view);

		// D3D11_DEPTH_STENCIL_DESC depth_stencil_desc;
		// ZeroMemory(&depth_stencil_desc, sizeof(depth_stencil_desc));
		//
		// depth_stencil_desc.DepthEnable = true;
		// depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		// depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
		// depth_stencil_desc.StencilEnable = false;
		//
		// DXCALL(_device->CreateDepthStencilState(&depth_stencil_desc, &_depth_stencil_state));

		depth_stencil_state_desc ds_desc;
		ZeroMemory(&ds_desc, sizeof(ds_desc));
		ds_desc.depth_enabled = true;
		ds_desc.depth_func = comparison_func::comparison_func_less;
		ds_desc.stencil_enabled = false;

		d3d11_device->create_depth_stencil_state(ds_desc, &depth_stencil_state);
		d3d11_context->set_depth_stencil_state(depth_stencil_state);

		if ( _rtv_binding == rtv_binding::swap_chain )
			initialize_depth_stencil_for_swap_chain();
		else initialize_depth_stencil_for_texture();

		// D3D11_RASTERIZER_DESC rasterizer_desc;
		// rasterizer_desc.CullMode = D3D11_CULL_NONE;
		// rasterizer_desc.FillMode = D3D11_FILL_SOLID;
		// rasterizer_desc.FrontCounterClockwise = true;
		// rasterizer_desc.AntialiasedLineEnable = false;
		// rasterizer_desc.DepthBias = 0;
		// rasterizer_desc.DepthBiasClamp = 0.f;
		// rasterizer_desc.SlopeScaledDepthBias = 0.f;
		// rasterizer_desc.DepthClipEnable = true;
		// rasterizer_desc.ScissorEnable = false;
		// rasterizer_desc.MultisampleEnable = false;
		//
		// DXCALL(_device->CreateRasterizerState(&rasterizer_desc, &_rasterizer_state));
		// util::name_com_object(_rasterizer_state, "raster");
		// _device_context->RSSetState(_rasterizer_state);

		rasterizer_desc raster_desc;
		ZeroMemory(&raster_desc, sizeof(raster_desc));
		raster_desc.cull_mode = cull_mode::none;
		raster_desc.fill_mode = fill_mode::solid;
		raster_desc.front_counter_clockwise = true;
		raster_desc.multisample_enabled = false;
		raster_desc.scissor_enabled = false;
		raster_desc.depth_clip_enabled = true;

		d3d11_device->create_rasterizer(raster_desc, &d3d11_rasterizer);
		d3d11_context->set_rasterizer(d3d11_rasterizer);

		// D3D11_SAMPLER_DESC sampler_desc;
		// sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		// sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		// sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		// sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		// sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		// sampler_desc.BorderColor[0] = 0.0f;
		// sampler_desc.BorderColor[1] = 0.0f;
		// sampler_desc.BorderColor[2] = 0.0f;
		// sampler_desc.BorderColor[3] = 0.0f;
		// sampler_desc.MinLOD = 0;
		// sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		// sampler_desc.MaxAnisotropy = 1;
		// sampler_desc.MipLODBias = 0.0f;
		// _device_context->PSSetSamplers(0, 1, &_sampler_state);
		// DXCALL(_device->CreateSamplerState(&sampler_desc, &_sampler_state));

		sampler_desc sampler_desc;
		ZeroMemory(&sampler_desc, sizeof(sampler_desc));
		sampler_desc.address_u = texture_address_mode::texture_address_clamp;
		sampler_desc.address_v = texture_address_mode::texture_address_clamp;
		sampler_desc.address_w = texture_address_mode::texture_address_clamp;
		sampler_desc.comparison_func = comparison_func::comparison_func_always;

		d3d11_device->create_sampler(sampler_desc, &d3d11_sampler);
		d3d11_context->set_samplers(&d3d11_sampler, 1, 0);


		if ( _rtv_binding == rtv_binding::swap_chain )
			initialize_viewport_for_swap_chain();
		else initialize_viewport_for_texture();

		//name_com_object(_depth_stencil_state, "deoth stencil");
		//name_com_object(_device, "device 1");

		color_shader._pixel_shader_path = std::filesystem::path("src/color/color_ps.hlsl");
		color_shader._vertex_shader_path = std::filesystem::path("src/color/color_vs.hlsl");

		color_shader._vertex_shader_entry_point = "vs_main";
		color_shader._pixel_shader_entry_point = "ps_main";
		
		color_shader.initialize();

		// D3D11_BUFFER_DESC camera_buffer_desc;
		// camera_buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		// camera_buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		// camera_buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		// camera_buffer_desc.MiscFlags = 0;
		// camera_buffer_desc.StructureByteStride = 0;
		// camera_buffer_desc.ByteWidth = sizeof(camera::camera_buffer);
		//
		// D3D11_SUBRESOURCE_DATA camera_buffer_data;
		// camera_buffer_data.pSysMem = &camera::get_camera_buffer();
		// camera_buffer_data.SysMemPitch = 0;
		// camera_buffer_data.SysMemSlicePitch = 0;
		//
		// DXCALL(_device->CreateBuffer(&camera_buffer_desc, &camera_buffer_data, &_camera_buffer));
		//
		// _device_context->VSSetConstantBuffers(1, 1, &_camera_buffer);

		buffer_desc camera_buffer_desc;
		camera_buffer_desc.bind_flags = bind_flag_constant_buffer;
		camera_buffer_desc.cpu_access_flags = cpu_access_flags_write;
		camera_buffer_desc.initial_data = &camera::get_camera_buffer();
		camera_buffer_desc.usage = resource_usage::resource_dynamic_usage;
		camera_buffer_desc.width = sizeof(camera::camera_buffer);

		d3d11_device->create_buffer(camera_buffer_desc, &d3d11_buffer);
		d3d11_context->vs_set_constant_buffers(&d3d11_buffer, 1, 1);

		content::mesh::initialize();

		return true;
	}

	void begin_frame()
	{
		float color[] = {(float)glm::sin(glfwGetTime()), 0.5f, 0.8f, 1.f};

		d3d11_context->clear_render_target(render_target);
		d3d11_context->clear_depth_stencil(depth_stencil);

		//_device_context->ClearRenderTargetView(_render_target_view, color);
		//_device_context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

		color_shader.bind();

		//D3D11_MAPPED_SUBRESOURCE msr;
		//_device_context->Map(_camera_buffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr);
		//memcpy(msr.pData, &camera::get_camera_buffer(), sizeof(camera::get_camera_buffer()));
		//s_device_context->Unmap(_camera_buffer, 0u);

		mapped_resource mapped_resource = d3d11_buffer->map(map_type::map_type_write);
		memcpy(mapped_resource._data, &camera::get_camera_buffer(), sizeof(camera::camera_buffer));
		d3d11_buffer->unmap();

		//_device_context->OMSetRenderTargets(1, &_render_target_view, _depth_stencil_view);
		framebuffer->set_depth_stencil(depth_stencil);
		d3d11_context->set_frame_buffer(framebuffer);
	}

	void begin_editor()		//TODO remove this
	{
	//	_device_context->OMSetRenderTargets(1, &_swap_chain_render_target_view, nullptr);
		d3d11_context->set_frame_buffer(swap_chain->get_framebuffer());
		float color[] = {0.f, 0.f, 0.f, 1.f};
	//	_device_context->ClearRenderTargetView(_swap_chain_render_target_view, color);
		d3d11_context->clear_render_target(swap_chain->get_framebuffer()->get_targets()[0]);
	}

	void end_frame()
	{
		d3d11_swap_chain* d3d11_sc = static_cast<d3d11_swap_chain*>(swap_chain);
		if (_vsync_enabled)
			d3d11_sc->get_internal()->Present(1, 0);
		else d3d11_sc->get_internal()->Present(0, 0);
	}

	void shutdown()
	{

		util::safe_release(d3d11_buffer);
		util::safe_release(d3d11_sampler);

		util::safe_release(swap_chain);

		util::safe_release(render_target);
		util::safe_release(render_target);
		util::safe_release(shader_resource);

		util::safe_release(depth_stencil_texture);
		util::safe_release(depth_stencil);
		util::safe_release(depth_stencil_state);


		// util::safe_release(_swap_chain);
		//
		// util::safe_release(_render_target_view);
		// util::safe_release(_swap_chain_render_target_view);
		// util::safe_release(_render_target);
		// util::safe_release(_render_target_srv);
		//
		// util::safe_release(_depth_stencil_buffer);
		// util::safe_release(_depth_stencil_view);
		// util::safe_release(_depth_stencil_state);
		// util::safe_release(_rasterizer_state);
		// util::safe_release(_sampler_state);
		// util::safe_release(_camera_buffer);

		util::safe_release(d3d11_device);
		util::safe_release(d3d11_context);
		color_shader.Release();
		content::mesh::shutdown();
	}

	void shutdown_device()
	{
		shutdown();
		_device_context->ClearState();
		_device_context->Flush();
		util::safe_release(_device_context);

#ifdef _DEBUG
		ID3D11Debug* debug_controller;
		_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug_controller);
		debug_controller->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL | D3D11_RLDO_IGNORE_INTERNAL);
		util::safe_release(debug_controller);

#endif // _DEBUG

		util::safe_release(_device);
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