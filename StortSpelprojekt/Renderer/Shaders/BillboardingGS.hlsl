cbuffer matrixBufferBillboarding : register(b6)
{
	matrix worldMatrix;
	matrix camViewMatrix;
	matrix camProjectionMatrix;
	float4 color;
	float3 campos;
	float pad;
	float3 ambientLight;
};

struct GS_IN
{
	float4 position : SV_POSITION;
};

struct GS_OUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 color : COLOROFFSET;
	float3 ambientLight : AMBIENT;
};

[maxvertexcount(4)]
void main(point GS_IN input[1], inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output = (GS_OUT)0;
	float scale = 0.1f;
	float4 particlepos = mul(input[0].position, worldMatrix);

	float3 particleToCam = campos - particlepos.xyz;

	float3 normal = normalize(particleToCam);
	float4 up = float4(0, 0, 1, 0) * scale;
	float4 right = float4(normalize(cross(particleToCam, up.xyz)).xyz, 0) * scale;
	up = float4(normalize(cross(particleToCam, right.xyz)).xyz, 0) * scale;

	//2------4
	//|		 |
	//|      |
	//1------3

	//First triangle

	//Corner 1
	output.position = mul(input[0].position - up - right, worldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(0, 1);
	output.normal = normal;
	output.color = color;
	output.ambientLight = ambientLight;

	OutputStream.Append(output);

	//Corner 2
	output.position = mul(input[0].position + up - right, worldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(0, 0);
	output.normal = normal;
	output.color = color;
	output.ambientLight = ambientLight;

	OutputStream.Append(output);

	//Corner 3
	output.position = mul(input[0].position - up + right, worldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(1, 1);
	output.normal = normal;
	output.color = color;
	output.ambientLight = ambientLight;

	OutputStream.Append(output);

	//Corner 4
	output.position = mul(input[0].position + up + right, worldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(1, 0);
	output.normal = normal;
	output.color = color;
	output.ambientLight = ambientLight;

	OutputStream.Append(output);

	OutputStream.RestartStrip();
}