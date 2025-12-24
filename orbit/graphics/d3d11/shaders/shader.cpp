#include "shader.h"
#include "../d3d11core.h"
#include <d3dcompiler.h>
#include <fstream>
#include "shaders_path.h"
#include "utility/vector.h"

namespace orbit::graphics::d3d11::shaders
{
	namespace 
	{
		inline bool output_error_blob(ID3D10Blob* error, const std::filesystem::path& shader_relative_path)
		{
			if (!error) return true;

			std::filesystem::path output_path = get_shader_output_path(shader_relative_path);

			OutputDebugString("Error compiling shader:\n");
			OutputDebugString(output_path.string().c_str());
			OutputDebugString("\n Check output folder for compile message.");


			std::ofstream output_file(output_path.string());

			for (unsigned int i = 0; i < error->GetBufferSize(); ++i)
				output_file << *((char*)error->GetBufferPointer() + i);

			output_file.close();

			return false;
		}

		bool create_input_layout_for_shader_bytecode(ID3D10Blob* shader_byte_code, ID3D11InputLayout** input_layout)
		{
			if (!shader_byte_code) return false;

			ID3D11ShaderReflection* shader_reflection = nullptr;

			DXCALL(D3DReflect(shader_byte_code->GetBufferPointer(), shader_byte_code->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&shader_reflection));

			D3D11_SHADER_DESC shader_desc;
			shader_reflection->GetDesc(&shader_desc);

			utl::vector<D3D11_INPUT_ELEMENT_DESC> input_layout_desc;

			for (unsigned int i = 0; i < shader_desc.InputParameters; ++i)
			{
				D3D11_SIGNATURE_PARAMETER_DESC parameter_desc;
				shader_reflection->GetInputParameterDesc(i, &parameter_desc);

				D3D11_INPUT_ELEMENT_DESC input_desc;
				//TODO make it work with input per instance data
				input_desc.SemanticName = parameter_desc.SemanticName;
				input_desc.SemanticIndex = parameter_desc.SemanticIndex;
				input_desc.InputSlot = 0;
				input_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
				input_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
				input_desc.InstanceDataStepRate = 0;

				if (parameter_desc.Mask == 1)
				{
					if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) input_desc.Format = DXGI_FORMAT_R32_UINT;
					else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) input_desc.Format = DXGI_FORMAT_R32_SINT;
					else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) input_desc.Format = DXGI_FORMAT_R32_FLOAT;
					else __debugbreak();
				}
				else if (parameter_desc.Mask <= 3)
				{
					if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) input_desc.Format = DXGI_FORMAT_R32G32_UINT;
					else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) input_desc.Format = DXGI_FORMAT_R32G32_SINT;
					else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) input_desc.Format = DXGI_FORMAT_R32G32_FLOAT;
					else __debugbreak();
				}
				else if (parameter_desc.Mask <= 7)
				{
					if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) input_desc.Format = DXGI_FORMAT_R32G32B32_UINT;
					else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) input_desc.Format = DXGI_FORMAT_R32G32B32_SINT;
					else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) input_desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
					else __debugbreak();
				}
				else if (parameter_desc.Mask <= 15)
				{
					if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) input_desc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
					else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) input_desc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
					else if (parameter_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) input_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
					else __debugbreak();
				}

				input_layout_desc.emplace_back(input_desc);
			}

			ID3D11Device* device = core::get_device();
			DXCALL(device->CreateInputLayout(input_layout_desc.data(), input_layout_desc.size(), shader_byte_code->GetBufferPointer(), shader_byte_code->GetBufferSize(), input_layout));
		
			util::safe_release(shader_reflection);

			return input_layout != nullptr;
		}
	}

	bool shader::initialize()
	{
		bool result = true;

		unsigned int shader_flags = 0;

#ifdef _DEBUG
		shader_flags |= D3DCOMPILE_DEBUG;
		shader_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ID3D10Blob* vertex_shader_bytecode = nullptr, *pixel_shader_bytecode = nullptr;
		ID3D10Blob* vertex_shader_error_blob = nullptr, *pixel_shader_error_blob = nullptr;

		HRESULT vertex_shader_result = D3DCompileFromFile(get_shader_path(_vertex_shader_path).wstring().c_str(), NULL, NULL, _vertex_shader_entry_point.c_str(), "vs_5_0", shader_flags, 0, &vertex_shader_bytecode, &vertex_shader_error_blob);
		HRESULT pixel_shader_result = D3DCompileFromFile(get_shader_path(_pixel_shader_path).wstring().c_str(), NULL, NULL, _pixel_shader_entry_point.c_str(), "ps_5_0", shader_flags, 0, &pixel_shader_bytecode, &pixel_shader_error_blob);

		output_error_blob(vertex_shader_error_blob, _vertex_shader_path);
		output_error_blob(pixel_shader_error_blob, _pixel_shader_path);

		if (FAILED(vertex_shader_result) or FAILED(pixel_shader_result))
			return false;

		util::safe_release(vertex_shader_error_blob);
		util::safe_release(pixel_shader_error_blob);

		ID3D11Device* device = core::get_device();
		device->CreateVertexShader(vertex_shader_bytecode->GetBufferPointer(), vertex_shader_bytecode->GetBufferSize(), NULL, &_vertex_shader);
		device->CreatePixelShader(pixel_shader_bytecode->GetBufferPointer(), pixel_shader_bytecode->GetBufferSize(), NULL, &_pixel_shader);

		if (!create_input_layout_for_shader_bytecode(vertex_shader_bytecode, &_input_layout))
			result = false;

		util::safe_release(vertex_shader_bytecode);
		util::safe_release(pixel_shader_bytecode);

		return result;
	}

	void shader::bind()
	{
		ID3D11DeviceContext* device_context = core::get_device_context();
		device_context->VSSetShader(_vertex_shader, 0, 0);
		device_context->PSSetShader(_pixel_shader, 0, 0);
		device_context->IASetInputLayout(_input_layout);


	}
}