#include "texture.h"
#include <unordered_set>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/sdbi.h"

namespace orbit::content::texture
{
    namespace
    {
        list_type _textures;
        std::unordered_set<handle_type, hash_type> _handles;

        platform_descriptor _platform;
    }

    texture::texture(const std::filesystem::path& path) : _path{path}
    {
        int channels;
        stbi_info(path.string().c_str(), &_width, &_height, &channels);
    }

    texture::texture(unsigned char* data, int width, int height) : _data{data}, _width{width}, _height{height} {}

    handle_type add_texture(const std::filesystem::path& path)
    {
        handle_type handle = _textures.emplace(path);
        _handles.insert(handle);
        _platform.add(handle);
        return handle;
    }

    handle_type add_texture(unsigned char* data, int width, int height)
    {
        handle_type handle = _textures.emplace(data, width, height);
        _handles.insert(handle);
        _platform.add(handle);
        return handle;
    }

    void remove_texture(const handle_type& handle)
    {
        assert(_textures.is_alive(handle));
        _platform.release(handle);
        _textures.erase(handle);
        _handles.erase(handle);
    }

    void bind_texture(const handle_type& handle)
    {
        assert(_textures.is_alive(handle));
        _platform.bind(handle);
    }

    void set_platform(const platform_descriptor& platform_descriptor)
    {
        for ( auto& handle : _handles )
            _platform.release(handle);

        _platform = platform_descriptor;

        for ( auto& handle : _handles )
            _platform.add(handle);
    }

    texture& get_texture(const handle_type& handle)
    {
        return _textures.get(handle);
    }

}