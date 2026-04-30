#pragma once
#include "common.h"

namespace orbit::graphics
{
    struct mapped_resource
    {
        void* _data = nullptr;
        unsigned int width = 0;
    };

    struct buffer_desc
    {
        unsigned int width = 0;
        const void* initial_data = nullptr;

        resource_usage usage;
        unsigned int cpu_access_flags;
        unsigned int bind_flags;
    };

    class buffer : public virtual device_resource
    {
    public:

        virtual mapped_resource map(map_type map_type) = 0;
        virtual void unmap() = 0;
        virtual void update_buffer(unsigned int offset, unsigned int size, void* data) = 0;

        virtual ~buffer() override {};

        buffer_desc get_desc() const { return _desc;}
    protected:

        explicit buffer(const buffer_desc& desc) : _desc{desc} {}

        bool _mapped = false;
        buffer_desc _desc{};
    };
}