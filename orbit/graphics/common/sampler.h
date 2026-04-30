#pragma once
#include "common.h"

namespace orbit::graphics
{
    struct sampler_desc
    {
        texture_address_mode address_u;
        texture_address_mode address_v;
        texture_address_mode address_w;
        comparison_func comparison_func;
        float border_color[4];
    };

    class sampler : public virtual device_resource
    {
    public:
        sampler_desc get_desc() const { return _desc;}
        ~sampler() override {}
    protected:
        explicit sampler(const sampler_desc &desc) : device_resource(nullptr, nullptr), _desc(desc) {}
    private:
        sampler_desc _desc;
    };
}