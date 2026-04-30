#pragma once
#include "common.h"

namespace orbit::graphics
{
    struct texture2D_desc
    {
        unsigned int width, height;
        sample_desc sample_desc;
        resource_usage usage;
        unsigned int bind_flags;
        unsigned int cpu_access_flags;
        format format;
        unsigned int mips;

        void* initial_data;
    };

    class texture2D : public virtual device_resource
    {
    public:
        explicit texture2D(texture2D_desc desc) : device_resource(nullptr, nullptr), _texture_desc{desc} {}
        ~texture2D() override {};

        texture2D_desc get_desc() const { return _texture_desc;}
    protected:
        texture2D_desc _texture_desc {};
    };
}