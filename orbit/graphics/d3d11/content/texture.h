#pragma once
#include "orbit/content/texture.h"
#include <d3d11.h>

namespace orbit::graphics::d3d11::content::texture
{
    namespace hl
    {
        using texture_type = ::orbit::content::texture::texture;
        DEFINE_LIST_TYPE(::orbit::content::texture::texture)
    }

    class texture
    {
    public:
        void initialize(const hl::handle_type& hl_handle);
        void bind();
        void release();
    private:
        ID3D11Texture2D* _texture;
        ID3D11ShaderResourceView* _texture_srv;
    };

    DEFINE_LIST_TYPE(texture)

    void add(const hl::handle_type& hl_handle);
    void bind(const hl::handle_type& hl_handle);
    void release(const hl::handle_type& hl_handle);

}