#pragma once
#include "common.h"
#include <cassert>

namespace orbit::graphics
{
    enum class shader_resource_type
    {
        texture2D,
    };

    struct shader_resource_desc
    {
        shader_resource_type type;
        format format;
    };

    class shader_resource : public virtual device_resource
    {
    public:
        explicit shader_resource(const shader_resource_desc& desc, device_resource* resource)
        : device_resource(nullptr, nullptr), _desc(desc), _resource( resource)
        {
            assert(resource);
            resource->add_ref();
        }

        ~shader_resource() override { util::safe_release(_resource); };

        shader_resource_desc get_desc() const { return _desc; }
        device_resource* get_resource() const { return _resource; }
    protected:
        shader_resource_desc _desc{};
        device_resource* _resource;
    };

}