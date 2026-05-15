#include "d3d11buffer.h"
#include "util.h"
#include "d3d11device.h"

namespace orbit::graphics
{
    d3d11_buffer::d3d11_buffer(rendering_device* device, rendering_device_context* context, const buffer_desc& desc)
    : device_resource(device, context), buffer(desc)
    {
        D3D11_BUFFER_DESC buffer_desc;
        ZeroMemory(&buffer_desc, sizeof(buffer_desc));

        _desc.width = desc.width + (16 - desc.width % 16);
        buffer_desc.Usage = d3d11::convert_resource_usage_flag(_desc.usage);
        buffer_desc.BindFlags = d3d11::convert_bind_flags(_desc.bind_flags);
        buffer_desc.CPUAccessFlags = d3d11::convert_cpu_access_flags(_desc.cpu_access_flags);
        buffer_desc.ByteWidth = _desc.width;

        if ( desc.structured )
        {
            buffer_desc.StructureByteStride = _desc.structure_width;
            buffer_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        }

        D3D11_SUBRESOURCE_DATA init_data;
        ZeroMemory(&init_data, sizeof(init_data));
        init_data.pSysMem = _desc.initial_data;

        D3D11_SUBRESOURCE_DATA* init_data_ptr = init_data.pSysMem ? &init_data : nullptr;


        d3d11_rendering_device* d3d11_device = static_cast<d3d11_rendering_device*>(device);
        DXCALL(d3d11_device->_internal_device->CreateBuffer(&buffer_desc, init_data_ptr, &_buffer));
    }

    mapped_resource d3d11_buffer::map(map_type map_type)
    {
        assert(!_mapped);
        _mapped = true;

        d3d11_rendering_device_context* d3d11_device_context = static_cast<d3d11_rendering_device_context*>(device_resource::get_context());

        D3D11_MAP map = d3d11::convert_map_type_flag(map_type);

        D3D11_MAPPED_SUBRESOURCE mapped_subresource;
        DXCALL(d3d11_device_context->_internal_device_context->Map(_buffer, 0, map, 0, &mapped_subresource));

        mapped_resource result;

        result.width = mapped_subresource.RowPitch;
        result._data = mapped_subresource.pData;

        return result;
    }

    void d3d11_buffer::unmap()
    {
        assert(_mapped);
        _mapped = false;

        d3d11_rendering_device_context* d3d11_device_context = static_cast<d3d11_rendering_device_context*>(device_resource::get_context());
        d3d11_device_context->_internal_device_context->Unmap(_buffer, 0);
    }

    void d3d11_buffer::update_buffer(unsigned int offset, unsigned int size, void* data)
    {
        d3d11_rendering_device_context* d3d11_device_context = static_cast<d3d11_rendering_device_context*>(device_resource::get_context());

        D3D11_BOX dst_box;
        ZeroMemory(&dst_box, sizeof(dst_box));

        dst_box.left = offset;
        dst_box.right = offset + size;
        dst_box.top = 0;
        dst_box.bottom = 1;
        dst_box.front = 0;
        dst_box.back = 1;

        d3d11_device_context->_internal_device_context->UpdateSubresource(_buffer, 0, &dst_box, data, 0, 0);
    }
}