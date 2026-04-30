#include "d3d11shader_resource.h"


#include "d3d11device.h"
#include "util.h"

namespace orbit::graphics
{
    d3d11_shader_resource::d3d11_shader_resource(rendering_device* device, rendering_device_context* context, const shader_resource_desc& desc, device_resource* resource)
        : device_resource(device, context), shader_resource(desc, resource)
    {
        d3d11_rendering_device* d3d11_device = static_cast<d3d11_rendering_device*>(device);

        D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
        ZeroMemory(&srv_desc, sizeof(srv_desc));
        if ( desc.type == shader_resource_type::texture2D )
        {
            d3d11_texture2D* texture = dynamic_cast<d3d11_texture2D*>(resource);
            texture2D_desc texture_desc = texture->get_desc();
            srv_desc.Format = d3d11::convert_format(desc.format);
            srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srv_desc.Texture2D.MipLevels = texture_desc.mips == 0 ? -1 : texture_desc.mips;
            srv_desc.Texture2D.MostDetailedMip = 0;

            DXCALL(d3d11_device->_internal_device->CreateShaderResourceView(texture->get_internal_texture(), &srv_desc, &_internal_shader_resource_view));
            if ( srv_desc.Texture2D.MipLevels != 1)
                static_cast<d3d11_rendering_device_context*>(context)->_internal_device_context->GenerateMips(_internal_shader_resource_view);
        }
        else __debugbreak();
    }
}