#include "material.h"
#include "orbit/graphics/renderer.h"

namespace orbit::content::material
{
    namespace
    {
        list_type _materials;
        std::unordered_set<handle_type, hash_type> _handles;
    }

    material::material(const material_desc& desc) : _desc(desc)
    {

        if ( !_desc.no_textures )
            return;

        _texture_srs.append(_desc.diffuse_textures_srs);
        _texture_srs.append(_desc.spectral_textures_srs);
        _texture_srs.append(_desc.ambient_textures_srs);

        //for ( auto texture_sr : _texture_srs )
        //    texture_sr->add_ref();

        _desc.material_buffer.first_diffuse_texture_slot = 0;
        _desc.material_buffer.first_specular_texture_slot = _desc.material_buffer.first_diffuse_texture_slot + _desc.material_buffer.no_diffuse_textures;
        _desc.material_buffer.first_ambient_texture_slot = _desc.material_buffer.first_specular_texture_slot + _desc.material_buffer.no_specular_textures;

        graphics::rendering_device* device = graphics::renderer::get_device();

        graphics::buffer_desc material_buffer_desc;
        material_buffer_desc.structured = false;
        material_buffer_desc.bind_flags = graphics::bind_flags::bind_flag_constant_buffer;
        material_buffer_desc.usage = graphics::resource_usage::resource_default_usage;
        material_buffer_desc.initial_data = &_desc.material_buffer;
        material_buffer_desc.width = sizeof(material_buffer);

        device->create_buffer(material_buffer_desc, &_material_cbuffer);

        utl::vector<texture_buffer> textures_buffers;
        textures_buffers.append(desc.diffuse_textures_data);
        textures_buffers.append(desc.spectral_textures_data);
        textures_buffers.append(desc.ambient_textures_data);

        assert(textures_buffers.size() <= 8);
        assert(_texture_srs.size() <= 8);
        assert(_texture_srs.size() == textures_buffers.size() );
        assert(_texture_srs.size() == _desc.no_textures );

        graphics::buffer_desc textures_buffer_desc;
        textures_buffer_desc.structured = true;
        textures_buffer_desc.structure_width = sizeof(texture_buffer);
        textures_buffer_desc.bind_flags = graphics::bind_flags::bind_flag_shader_resource;
        textures_buffer_desc.usage = graphics::resource_usage::resource_constant_usage;
        textures_buffer_desc.initial_data = textures_buffers.data();
        textures_buffer_desc.width = sizeof(texture_buffer) * textures_buffers.size();

        device->create_buffer(textures_buffer_desc, &_textures_buffer);

        graphics::shader_resource_desc textures_sr_desc;
        textures_sr_desc.format = graphics::format::FORMAT_UNKNOWN;
        textures_sr_desc.type = graphics::shader_resource_type::buffer;
        textures_sr_desc.buffer.first_element = 0;
        textures_sr_desc.buffer.num_elements = textures_buffers.size();
        device->create_shader_resource(textures_sr_desc, _textures_buffer, &_textures_desc_sr);
    }

    void material::bind()
    {
        if ( !_desc.no_textures )
            return;

        graphics::rendering_device_context* context = graphics::renderer::get_context();
        context->ps_set_constant_buffers(&_material_cbuffer, 1, 0);
        context->ps_set_shader_resources(_texture_srs.data(), _texture_srs.size(), 0);
        context->ps_set_shader_resources(&_textures_desc_sr, 1, 9);
    }

    material::~material()
    {
        for ( auto texture_sr : _texture_srs )
            util::safe_release(texture_sr);

        util::safe_release(_material_cbuffer);
        util::safe_release(_textures_buffer);
        util::safe_release(_textures_desc_sr);
    }

    handle_type add_material(const material_desc& desc)
    {
        handle_type handle = _materials.emplace(desc);
        _handles.insert(handle);
        return handle;
    }

    material& get_material(const handle_type& handle)
    {
        return _materials.get(handle);
    }

    void bind_material(const handle_type& handle)
    {
        _materials.get(handle).bind();
    }

    void remove_material(const handle_type& handle)
    {
        if(!_materials.is_alive(handle))
            return;

        _materials.erase(handle);
        _handles.erase(handle);
    }
}