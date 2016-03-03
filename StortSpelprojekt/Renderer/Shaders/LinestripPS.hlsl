/*----------------------------------------------------------------------------------------------------------------------
| Pixel shader used specifically for the grid, or other linestrips. Renders the color to the render target.			   |
----------------------------------------------------------------------------------------------------------------------*/
struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 colorOffset : COLOROFFSET;
};

float4 main(VS_OUT input) : SV_TARGET2
{
	return float4(input.colorOffset, 1.0f);
}