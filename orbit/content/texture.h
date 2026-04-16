#pragma once
#include <filesystem>
#include <functional>
#include "orbit/utility/freelist.h"

namespace orbit::content::texture
{
    class texture
    {
    public:
        explicit texture(const std::filesystem::path& path);
        texture(unsigned char* data, int width, int height);

        int get_width() const { return _width;}
        int get_height() const { return _height;}
        unsigned char* get_data() const { return _data;}
        std::filesystem::path get_path() const { return _path;}

    private:
        int _width = 0, _height = 0;
        unsigned char* _data = nullptr;
        std::filesystem::path _path;
    };

    DEFINE_LIST_TYPE(texture)

    struct platform_descriptor
    {
        std::function<void(const handle_type&)> add;
        std::function<void(const handle_type&)> bind;
        std::function<void(const handle_type&)> release;
    };

    void set_platform(const platform_descriptor& platform_descriptor);

    handle_type add_texture(const std::filesystem::path& path);
    texture& get_texture(const handle_type& handle);
    void remove_texture(const handle_type& handle);
    void bind_texture(const handle_type& handle);
}