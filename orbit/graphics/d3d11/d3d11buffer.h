#pragma once
#include "orbit/graphics/common/buffer.h"
#include "d3d11common.h"

namespace orbit::graphics
{
    class d3d11_buffer final : public buffer
    {
        friend class d3d11_rendering_device;
        friend class d3d11_rendering_device_context;

    public:

        mapped_resource map(map_type map_type) override;
        void unmap() override;
        void update_buffer(unsigned int offset, unsigned int size, void* data) override;

        ~d3d11_buffer() override { util::safe_release(_buffer); }
    private:
        d3d11_buffer(rendering_device* device, rendering_device_context* context, const buffer_desc& desc);

        ID3D11Buffer* _buffer = nullptr;
    };
}