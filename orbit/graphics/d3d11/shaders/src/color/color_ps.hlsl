Texture2D shader_texture : register(t0);
SamplerState sampler_type : register(s0);

struct pixel_input_type
{
    float4 position : SV_Position;
    float4 normal : NORMAL;
    float2 uv : TEXCOORD;
};

float4 ps_main(pixel_input_type input) : SV_TARGET
{
    float4 texture_color;
    texture_color = shader_texture.Sample(sampler_type, float2(input.uv.x, 1.f - input.uv.y));
    return texture_color;
}