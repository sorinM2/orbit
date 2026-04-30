#include "d3d11texture.h"
#include "util.h"
#include "d3d11device.h"

namespace orbit::graphics
{
    d3d11_texture2D::d3d11_texture2D(rendering_device* device, rendering_device_context* context, const texture2D_desc& desc) : device_resource(device, context), texture2D(desc)
    {
        D3D11_TEXTURE2D_DESC d3d11_desc;
        ZeroMemory(&d3d11_desc, sizeof(d3d11_desc));
        d3d11_desc.Width = desc.width;
        d3d11_desc.Height = desc.height;
        d3d11_desc.Format = d3d11::convert_format(desc.format);
        d3d11_desc.MipLevels = desc.mips;
        d3d11_desc.ArraySize = 1;
        d3d11_desc.SampleDesc.Count = desc.sample_desc.count;
        d3d11_desc.SampleDesc.Quality = desc.sample_desc.quality;
        d3d11_desc.Usage = d3d11::convert_resource_usage_flag(desc.usage);
        d3d11_desc.BindFlags = d3d11::convert_bind_flags(desc.bind_flags);
        d3d11_desc.CPUAccessFlags = d3d11::convert_cpu_access_flags(desc.cpu_access_flags);

        D3D11_SUBRESOURCE_DATA data;
        ZeroMemory(&data, sizeof(data));
        data.SysMemPitch = desc.width * d3d11::get_format_size(desc.format);
        data.pSysMem = desc.initial_data;

        d3d11_rendering_device* d3d11_device = static_cast<d3d11_rendering_device*>(device);
        d3d11_rendering_device_context* d3d11_context = static_cast<d3d11_rendering_device_context*>(context);

        if ( desc.mips != 1)
        {
            d3d11_desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
            DXCALL(d3d11_device->_internal_device->CreateTexture2D(&d3d11_desc, nullptr, &_internal_texture));
            if ( desc.initial_data )
                d3d11_context->_internal_device_context->UpdateSubresource(_internal_texture, 0, NULL, data.pSysMem, desc.width * 4 * sizeof(unsigned char), 0);
        }
        else if ( desc.initial_data )
        {
            DXCALL(d3d11_device->_internal_device->CreateTexture2D(&d3d11_desc, &data, &_internal_texture));
        }
        else DXCALL(d3d11_device->_internal_device->CreateTexture2D(&d3d11_desc, nullptr, &_internal_texture));
    }

    d3d11_texture2D::d3d11_texture2D(rendering_device* device, rendering_device_context* context, const texture2D_desc& desc, ID3D11Texture2D* texture)
    : device_resource(device, context), texture2D(desc)
    {
        _internal_texture = texture;
        _internal_texture->AddRef();
    }

}