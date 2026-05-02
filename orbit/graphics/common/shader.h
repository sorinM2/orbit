#pragma once
#include "common.h"
#include <filesystem>

namespace orbit::graphics
{

    struct input_element
    {
        std::string semantic_name;
        unsigned int semantic_index;
        unsigned int input_slot;
        format format;

        input_classification classification;
        unsigned int instance_step_rate;
    };

    struct shader_desc
    {
        std::filesystem::path path;
        shader_type type;
        std::string entry_point;
        struct vertex
        {
            input_element* elements;
            unsigned int element_count;
        } vertex_shader;
    };

    class shader : public virtual device_resource
    {
    public:
        explicit shader(const shader_desc& desc, rendering_device* device = nullptr, rendering_device_context* context = nullptr) : device_resource(device, context), _desc{desc}
        {
            if ( _desc.type == shader_type::vertex )
            {
                _desc.vertex_shader.elements = new input_element[desc.vertex_shader.element_count];
                for ( unsigned int i = 0; i < desc.vertex_shader.element_count; i++ )
                    _desc.vertex_shader.elements[i] = desc.vertex_shader.elements[i];
            }
        }

        ~shader() override {}
    protected:
        shader_desc _desc;
    };

    struct program_desc
    {
        shader* vertex_shader;
        shader* pixel_shader;
    };

    class program : public virtual device_resource
    {
        public:
        program(const program_desc& desc, rendering_device* device = nullptr, rendering_device_context* context = nullptr) : device_resource(device, context), _desc{desc}
        {
            _desc.vertex_shader->add_ref();
            _desc.pixel_shader->add_ref();
        }

        ~program() override
        {
            util::safe_release(_desc.vertex_shader);
            util::safe_release(_desc.pixel_shader);
        }

        program_desc get_desc() const { return _desc; }
    private:
        program_desc _desc;
    };


}