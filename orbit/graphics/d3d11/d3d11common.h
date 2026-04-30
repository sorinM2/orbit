#pragma once 
#include <d3d11.h>
#include <dxgi1_6.h>
#include <format>
#include <cassert>
#include "orbit/graphics/common/util.h"

namespace orbit::graphics::d3d11
{
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
#define DXCALL(x) x;
#endif