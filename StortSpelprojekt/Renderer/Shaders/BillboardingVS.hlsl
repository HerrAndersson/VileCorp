/*----------------------------------------------------------------------------------------------------------------------
| Vertex shader used for the rendering of billboarded objects. Simply passes a world position to the geometry shader   |
----------------------------------------------------------------------------------------------------------------------*/

struct VS_IN
{
	float4 position : POSITION;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.position = input.position;

	return output;
}