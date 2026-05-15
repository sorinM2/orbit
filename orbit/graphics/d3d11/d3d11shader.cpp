#pragma once
#include "d3d11shader.h"

#include <d3dcompiler.h>

#include <fstream>
#include "d3d11device.h"
#include "shaders/shaders_path.h"
#include "util.h"

namespace orbit::graphics
{

	namespace
	{
		bool output_error_blob(ID3D10Blob* error, const std::filesystem::path& shader_relative_path)
		{
			if (!error) return true;

			std::filesystem::path output_path = d3d11::shaders::get_shader_output_path(shader_relative_path);

			OutputDebugString("Error compiling shader:\n");
			OutputDebugString(output_path.string().c_str());
			OutputDebugString("\n Check output folder for compile message.");


			std::ofstream output_file(output_path.string());

			for (unsigned int i = 0; i < error->GetBufferSize(); ++i)
				output_file << *((char*)error->GetBufferPointer() + i);

			output_file.close();

			return false;
		}
	}

    d3d11_shader::d3d11_shader(const shader_desc& desc, rendering_device* device, rendering_device_context* context)
        : device_resource(device, context), shader(desc)
    {
    	ID3D10Blob* shader_bytecode = nullptr, *error_blob = nullptr;
    	std::string target;
    	if ( desc.type == shader_type::vertex )
    		target = "vs_5_0";
    	if ( desc.type == shader_type::pixel )
    		target = "ps_5_0";

		std::wstring s(d3d11::shaders::get_shader_path(_desc.path).wstring().c_str());

    	D3DCompileFromFile(d3d11::shaders::get_shader_path(_desc.path).wstring().c_str(), nullptr, nullptr, _desc.entry_point.c_str(), target.c_str(),
    		0, 0, &shader_bytecode, &error_blob);

		output_error_blob(error_blob, _desc.path);

		d3d11_rendering_device* d3d11_device = static_cast<d3d11_rendering_device*>(device);
		if ( desc.type == shader_type::vertex )
			DXCALL(d3d11_device->_internal_device->CreateVertexShader(shader_bytecode->GetBufferPointer(), shader_bytecode->GetBufferSize(), NULL, &_internal_vertex_shader));
		if ( desc.type == shader_type::pixel )
			DXCALL(d3d11_device->_internal_device->CreatePixelShader(shader_bytecode->GetBufferPointer(), shader_bytecode->GetBufferSize(), NULL, &_internal_pixel_shader));

		if ( desc.type == shader_type::vertex )
		{
			utl::vector<D3D11_INPUT_ELEMENT_DESC> input_layout_desc;
			for ( unsigned int i = 0; i < _desc.vertex_shader.element_count; ++i )
			{
				const input_element& hl_desc = _desc.vertex_shader.elements[i];
				D3D11_INPUT_ELEMENT_DESC input_desc;
				input_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				input_desc.Format = d3d11::convert_format(hl_desc.format);
				input_desc.InputSlot = hl_desc.input_slot;
				input_desc.InputSlotClass = d3d11::convert_input_classification(hl_desc.classification);
				input_desc.SemanticName = hl_desc.semantic_name.c_str();
				input_desc.SemanticIndex = hl_desc.semantic_index;
				input_desc.InstanceDataStepRate = hl_desc.classification == input_classification::per_instance_data ? hl_desc.instance_step_rate : 0;

				input_layout_desc.emplace_back(input_desc);
			}

			DXCALL(d3d11_device->_internal_device->CreateInputLayout(input_layout_desc.data(), input_layout_desc.size(),
				shader_bytecode->GetBufferPointer(), shader_bytecode->GetBufferSize(), &_internal_layout));
		}

		util::safe_release(shader_bytecode);
		util::safe_release(error_blob);
    }

	d3d11_shader::~d3d11_shader()
	{
		util::safe_release(_internal_vertex_shader);
		util::safe_release(_internal_pixel_shader);
		util::safe_release(_internal_layout);
	}

}