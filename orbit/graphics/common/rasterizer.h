#pragma once
#include "common.h"
namespace orbit::graphics
{
    struct viewport
    {
        int width;
        int height;
        int x;
        int y;
    };

    struct rasterizer_desc
    {
        fill_mode fill_mode;
        cull_mode cull_mode;
        bool front_counter_clockwise;
        bool depth_clip_enabled;
        bool scissor_enabled;
        bool multisample_enabled;
    };

    class rasterizer : public virtual device_resource
    {
    public:
        virtual ~rasterizer() override {};
        rasterizer_desc get_desc() const { return _desc;}
    protected:
        rasterizer(const rasterizer_desc& desc) : device_resource(nullptr, nullptr), _desc{desc} {}
    private:
        rasterizer_desc _desc;
    };
}