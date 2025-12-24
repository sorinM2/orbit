cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct vertex_input_type
{
    float3 position : POSITION;
    float3 color : COLOR;
};

struct pixel_input_type
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

pixel_input_type vs_main(vertex_input_type input)
{
    pixel_input_type output;
    output.color = float4(input.color, 1.f);
    output.position = float4(input.position, 1.f);
    
    return output;
}