cbuffer mesh_buffer : register(b0)
{
    matrix world_matrix;
    bool instanced;
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

    float4 world0 : WORLD0;
    float4 world1 : WORLD1;
    float4 world2 : WORLD2;
    float4 world3 : WORLD3;
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

    matrix world;
    if ( instanced )
        world = matrix(input.world0, input.world1, input.world2, input.world3);
    else world = world_matrix;

    output.position = mul(float4(input.position, 1.f), world);
    output.position = mul(output.position, view_matrix);
    output.position = mul(output.position, projection_matrix);

    output.uv = input.uv;

    return output;
}