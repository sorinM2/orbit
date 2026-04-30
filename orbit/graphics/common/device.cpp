#include "device.h"
#include "common.h"

namespace orbit::graphics
{
    rendering_device::~rendering_device()
    {
        for ( auto* resource : _resources )
            resource -> Release();
    }

    void rendering_device_context::get_viewports(int* count, viewport* viewports) const
    {
        *count = _viewports.size();
        memcpy (viewports, _viewports.data(), _viewports.size() * sizeof(viewport));
    }

    void rendering_device_context::get_scissors(int* count, rect* rects) const
    {
        *count = _scissors.size();
        memcpy(rects, _scissors.data(), sizeof(rect) * _scissors.size());
    }



}