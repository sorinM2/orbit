#pragma once

namespace orbit
{
    class ref_counted
    {
    public:
        virtual void add_ref() { ref_count++; }
        virtual void Release() { if ( --ref_count == 0 ) delete this;}

    protected:
        virtual ~ref_counted() {};
    private:
        int ref_count = 1 ;
    };

    namespace util
    {
        template<typename T>
        void safe_release(T*& resource)
        {
            if (resource)
            {
                resource->Release();
                resource = nullptr;
            }
        }
    }
}