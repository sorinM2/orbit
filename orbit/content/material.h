#pragma once
#include "orbit/graphics/common/texture.h"
#include "orbit/graphics/common/buffer.h"

#include <filesystem>

#include "glm/vec3.hpp"
#include "orbit/utility/freelist.h"
#include <unordered_map>

#include "orbit/graphics/common/shader_resource.h"

namespace orbit::content::material
{
    enum class material_texture_type
    {
        diffuse,
        specular,
        ambient,
        count
    };

    struct material_texture_desc
    {
        material_texture_type type;
    };

    struct texture_buffer
    {
        float blend;
    };

    struct material_buffer
    {
        glm::vec3 color_diffuse{};
        glm::vec3 color_specular{};
        glm::vec3 color_ambient{};

        float shininess = 0.f;
        float opacity = 1.f;

        int no_diffuse_textures = 0;
        int no_specular_textures = 0;
        int no_ambient_textures = 0;

    private:
        friend class material;
        unsigned int first_diffuse_texture_slot = 0;
        unsigned int first_specular_texture_slot = 0;
        unsigned int first_ambient_texture_slot = 0;
    };

    struct material_desc
    {
        unsigned int no_textures = 0;

        utl::vector<texture_buffer> diffuse_textures_data;
        utl::vector<texture_buffer> spectral_textures_data;
        utl::vector<texture_buffer> ambient_textures_data;

        utl::vector<graphics::shader_resource*> diffuse_textures_srs;
        utl::vector<graphics::shader_resource*> spectral_textures_srs;
        utl::vector<graphics::shader_resource*> ambient_textures_srs;

        material_buffer material_buffer;
    };

    class material
    {
    public:
        explicit material(const material_desc& desc);
        void bind();

        material(const material& other) = delete;

        material(material&& other) noexcept
        {
            _material_cbuffer = other._material_cbuffer;
            _textures_buffer = other._textures_buffer;
            _textures_desc_sr = other._textures_desc_sr;
            _texture_srs = other._texture_srs;
            _desc = other._desc;

            other._texture_srs.clear();
            other._material_cbuffer = nullptr;
            other._textures_buffer = nullptr;
            other._textures_desc_sr = nullptr;
        }

        ~material();
    private:

        material_desc _desc{};

        utl::vector<graphics::shader_resource*> _texture_srs;
        graphics::buffer* _material_cbuffer = nullptr;
        graphics::buffer* _textures_buffer = nullptr;
        graphics::shader_resource* _textures_desc_sr = nullptr;
    };

    DEFINE_LIST_TYPE(material)

    handle_type add_material(const material_desc& desc);
    material& get_material(const handle_type& handle);
    void remove_material(const handle_type& handle);
    void bind_material(const handle_type& handle);
}
