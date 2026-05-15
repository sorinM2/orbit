#include "common.h"
#include "device.h"
namespace orbit::graphics
{
    device_resource::~device_resource()
    {
        if ( _owner ) _owner->remove_resource(this);
    }

}