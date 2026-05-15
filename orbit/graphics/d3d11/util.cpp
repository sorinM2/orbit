#include "util.h"
#include <unordered_map>


namespace orbit::graphics::d3d11
{
    namespace
    {
        std::unordered_map<format, DXGI_FORMAT> _converisons;
    }
    void initialize_conversions()
    {
        _converisons[format::FORMAT_R8_SINT] = DXGI_FORMAT_R8_SINT;
        _converisons[format::FORMAT_R8_UINT] = DXGI_FORMAT_R8_UINT;
        _converisons[format::FORMAT_R8_UNORM] = DXGI_FORMAT_R8_UNORM;

        _converisons[format::FORMAT_R8G8_SINT] = DXGI_FORMAT_R8G8_SINT;
        _converisons[format::FORMAT_R8G8_UINT] = DXGI_FORMAT_R8G8_UINT;
        _converisons[format::FORMAT_R8G8_UNORM] = DXGI_FORMAT_R8G8_UNORM;

        _converisons[format::FORMAT_R8G8B8A8_SINT] = DXGI_FORMAT_R8G8B8A8_SINT;
        _converisons[format::FORMAT_R8G8B8A8_UINT] = DXGI_FORMAT_R8G8B8A8_UINT;
        _converisons[format::FORMAT_R8G8B8A8_UNORM] = DXGI_FORMAT_R8G8B8A8_UNORM;

        _converisons[format::FORMAT_R32_SINT] = DXGI_FORMAT_R32_SINT;
        _converisons[format::FORMAT_R32_UINT] = DXGI_FORMAT_R32_UINT;
        _converisons[format::FORMAT_R32_FLOAT] = DXGI_FORMAT_R32_FLOAT;

        _converisons[format::FORMAT_R32G32_SINT] = DXGI_FORMAT_R32G32_SINT;
        _converisons[format::FORMAT_R32G32_UINT] = DXGI_FORMAT_R32G32_UINT;
        _converisons[format::FORMAT_R32G32_FLOAT] = DXGI_FORMAT_R32G32_FLOAT;

        _converisons[format::FORMAT_R32G32B32_SINT] = DXGI_FORMAT_R32G32B32_SINT;
        _converisons[format::FORMAT_R32G32B32_UINT] = DXGI_FORMAT_R32G32B32_UINT;
        _converisons[format::FORMAT_R32G32B32_FLOAT] = DXGI_FORMAT_R32G32B32_FLOAT;

        _converisons[format::FORMAT_R32G32B32A32_FLOAT] = DXGI_FORMAT_R32G32B32A32_FLOAT;
        _converisons[format::FORMAT_R32G32B32A32_UINT] = DXGI_FORMAT_R32G32B32A32_UINT;
        _converisons[format::FORMAT_R32G32B32A32_SINT] = DXGI_FORMAT_R32G32B32A32_SINT;

        _converisons[format::FORMAT_D24_UNORM_S8_UINT] = DXGI_FORMAT_D24_UNORM_S8_UINT;

        _converisons[format::FORMAT_UNKNOWN] = DXGI_FORMAT_UNKNOWN;
    }

    DXGI_FORMAT convert_format(format format)
    {
        static bool initialized = false;
        if ( !initialized )
        {
            initialized = true;
            initialize_conversions();
        }

        return _converisons[format];
    }


}