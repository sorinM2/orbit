#include "texture.h"

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/sdbi.h"
#include "unordered_map"

#include "../d3d11core.h"
#include "../d3d11common.h"

namespace orbit::graphics::d3d11::content::texture
{

    namespace
    {
        list_type _textures;
        std::unordered_map<hl::handle_type, handle_type, hl::hash_type> hl_to_handle;
    }

    void texture::initialize(const hl::handle_type& hl_handle)
    {
        hl::texture_type& hl_texture = ::orbit::content::texture::get_texture(hl_handle);
        assert( hl_texture.get_data() or !hl_texture.get_path().empty() );

        ID3D11DeviceContext* device_context = core::get_device_context();
        ID3D11Device* device = core::get_device();

        int width, height, channels;
        unsigned char* data = stbi_load(hl_texture.get_path().string().c_str(), &width, &height, &channels, 4);

        // D3D11_TEXTURE2D_DESC texture_desc;
        // D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
        // D3D11_SUBRESOURCE_DATA subresource_data;
        //
        // texture_desc.Width = width;
        // texture_desc.Height = height;
        // texture_desc.MipLevels = 0;
        // texture_desc.ArraySize = 1;
        // texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        // texture_desc.SampleDesc.Count = 1;
        // texture_desc.SampleDesc.Quality = 0;
        // texture_desc.Usage = D3D11_USAGE_DEFAULT;
        // texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        // texture_desc.CPUAccessFlags = 0;
        // texture_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        //
        // device->CreateTexture2D(&texture_desc, nullptr, &_texture);
        // device_context->UpdateSubresource(_texture, 0, NULL, data, width * 4 * sizeof(unsigned char), 0);
        // srv_desc.Format = texture_desc.Format;
        // srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        // srv_desc.Texture2D.MipLevels = -1;
        // srv_desc.Texture2D.MostDetailedMip = 0;
        //
        // device->CreateShaderResourceView(_texture, &srv_desc, &_texture_srv);
        //
        // device_context->GenerateMips(_texture_srv);

        texture2D_desc texture_desc;
        shader_resource_desc sr_desc;

        texture_desc.bind_flags = bind_flag_shader_resource;// | bind_flag_render_target;
        texture_desc.cpu_access_flags = 0;
        texture_desc.width = width;
        texture_desc.height = height;
        texture_desc.format = format::FORMAT_R8G8B8A8_UNORM;
        texture_desc.initial_data = data;
        texture_desc.sample_desc.count = 1;
        texture_desc.sample_desc.quality = 0;
        texture_desc.usage = resource_usage::resource_constant_usage;
        texture_desc.mips = 1;

        rendering_device* rendering_device = d3d11::core::get_rendering_device();
        rendering_device_context* rendering_context = d3d11::core::get_rendering_device_context();

        rendering_device->create_texture2D(texture_desc, &_texture);

        sr_desc.format = format::FORMAT_R8G8B8A8_UNORM;
        sr_desc.type = shader_resource_type::texture2D;

        rendering_device->create_shader_resource(sr_desc, _texture, &_shader_resource);

        stbi_image_free(data);
    }

    void texture::Release()
    {
        util::safe_release(_texture);
        util::safe_release(_shader_resource);
    }

    void texture::bind()
    {
        // ID3D11DeviceContext* device_context = core::get_device_context();
        // device_context->PSSetShaderResources(0, 1, &_texture_srv);

        rendering_device_context* rendering_device_context = d3d11::core::get_rendering_device_context();
        rendering_device_context->ps_set_shader_resources(&_shader_resource, 1, 0);
    }


    void add(const hl::handle_type& hl_handle)
    {
        assert(!hl_to_handle.contains(hl_handle));
        handle_type handle = _textures.emplace();
        hl_to_handle[hl_handle] = handle;

        texture& texture = _textures.get(handle);
        texture.initialize(hl_handle);
    }

    void bind(const hl::handle_type& hl_handle)
    {
        assert(hl_to_handle.contains(hl_handle));
        handle_type handle = hl_to_handle[hl_handle];

        assert(_textures.is_alive(handle));
        texture& texture = _textures.get(handle);

        texture.bind();
    }

    void release(const hl::handle_type& hl_handle)
    {
        assert(hl_to_handle.contains(hl_handle));
        handle_type handle = hl_to_handle[hl_handle];

        assert(_textures.is_alive(handle));
        texture& texture = _textures.get(handle);

        texture.Release();

        _textures.erase(handle);
        hl_to_handle.erase(hl_handle);
    }
}
