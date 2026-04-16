cbuffer mesh_buffer : register(b0)
{
    matrix world_matrix;
};

cbuffer camera_buffer : register(b1)
{
    matrix view_matrix;
    matrix projection_matrix;
};

struct vertex_input_type
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
};

struct pixel_input_type
{
    float4 position : SV_Position;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

pixel_input_type vs_main(vertex_input_type input)
{
    pixel_input_type output;
    output.normal = float4(input.normal, 1.f);

    output.position = float4(input.position, 1.f);
    output.position = mul(output.position, view_matrix);
    output.position = mul(output.position, projection_matrix);

    output.uv = input.uv;
    return output;
}