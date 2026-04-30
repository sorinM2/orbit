#pragma once
#include "device.h"

namespace orbit::graphics
{
    struct mesh_data
    {
        buffer* _vertex_buffer;
        buffer* _index_buffer;

        buffer* _transforms;

        unsigned int instances;
    };

    class render_pass_data
    {
        mesh_data** meshes = nullptr;
    };
}