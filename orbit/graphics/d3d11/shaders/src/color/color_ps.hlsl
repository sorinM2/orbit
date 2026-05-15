Texture2D textures[8] : register(t0);

cbuffer material_buffer : register(b0)
{
    float3 color_diffuse;
    float3 color_specular;
    float3 color_ambient;

    float shininess;
    float opacity;

    uint no_diffuse_textures;
    uint no_specular_textures;
    uint no_ambient_textures;

    uint first_diffuse_texture_slot;
    uint first_specular_texture_slot;
    uint first_ambient_texture_slot;
};

struct texture_desc
{
    float blend;
};

StructuredBuffer<texture_desc> _texture_descs : register(t9);

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
    texture_color = textures[0].Sample(sampler_type, float2(input.uv.x, 1.f - input.uv.y));
    return texture_color;
}