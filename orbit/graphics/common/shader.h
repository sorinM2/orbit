#pragma once
#include "device.h"

namespace orbit::graphics
{
    class shader : public device_resource
    {
    public:
        virtual ~shader() override {};
    protected:
        std::filesystem::path shader_path;
    };

    class vertex_shader : public shader
    {
    public:
        virtual ~vertex_shader() override {};    };

    class pixel_shader : public shader
    {
    public:
        virtual ~pixel_shader() override {};
    };

    enum class input_classification
    {
        per_vertex_data,
        per_instance_data
    };

    struct input_element
    {
        std::string semantic_name;
        unsigned int semantic_index;
        unsigned int input_slot;
        format format;

        input_classification classification;
        unsigned int instance_step_rate;

    };

    class input_layout : public device_resource
    {
    public:
        virtual ~input_layout() override {};
    };
}