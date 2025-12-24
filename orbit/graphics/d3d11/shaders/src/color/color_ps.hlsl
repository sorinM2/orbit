struct pixel_input_type
{
    float4 position : SV_Position;
    float4 color : COLOR;
};

float4 ps_main(pixel_input_type input) : SV_TARGET 
{
    return input.color;
}