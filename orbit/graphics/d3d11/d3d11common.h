#pragma once 
#include <d3d11.h>
#include <dxgi1_6.h>
#include <format>

namespace orbit::graphics::d3d11::util
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

	template<typename T>
	void name_com_object(T* resource, const char* name)
	{
		if (resource)
			resource->SetPrivateData(WKPDID_D3DDebugObjectName, strlen(name), name);
	}
}

#if defined(DEBUG) | defined(_DEBUG)
#define DXCALL(x)																											\
{																															\
	HRESULT hr = x;																											\
	if (FAILED(hr))																											\
	{																														\
		OutputDebugString(std::format("DXCALL error in file: {} Line: {} Code: {}", __FILE__, __LINE__, #x).c_str());		\
		__debugbreak();																										\
	}																														\
}
#else
#define DXCALL(X) x;
#endif