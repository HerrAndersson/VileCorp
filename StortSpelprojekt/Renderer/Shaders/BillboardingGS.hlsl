cbuffer MatrixBuffer
{
	matrix emitterWorldMatrix;
	matrix camViewMatrix;
	matrix camProjectionMatrix;
	float3 campos;
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
	float4 worldPos : POS;
};

[maxvertexcount(6)]
void main(point GS_IN input[1], inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output = (GS_OUT)0;
	float scale = 1.0f;

	float4 up = float4(0, 1, 0, 0);
	float4 particlepos = mul(input[0].position, worldMatrix);
	float4 right = float4(normalize(cross(campos - particlepos, up)).xyz, 0);
	float4 temp;

	float3 normal = cross(up, right);
	normal = normalize(normal.xyz);

	up = up*scale;
	right = right*scale;

	//First triangle
	temp = input[0].position + up - right;
	output.position = mul(temp, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = float2(0, 0);
	output.normal = normal;
	output.worldPos = mul(temp, worldMatrix);
	OutputStream.Append(output);

	temp = input[0].position + up + right;
	output.position = mul(temp, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = float2(1, 0);
	output.normal = normal;
	output.worldPos = mul(temp, worldMatrix);
	OutputStream.Append(output);

	temp = input[0].position - up + right;
	output.position = mul(temp, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = float2(1, 1);
	output.normal = normal;
	output.worldPos = mul(temp, worldMatrix);
	OutputStream.Append(output);

	//Second triangle
	temp = input[0].position + up - right;
	output.position = mul(temp, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = float2(0, 0);
	output.normal = normal;
	output.worldPos = mul(temp, worldMatrix);
	OutputStream.Append(output);

	temp = input[0].position - up - right;
	output.position = mul(temp, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = float2(0, 1);
	output.normal = normal;
	output.worldPos = mul(temp, worldMatrix);
	OutputStream.Append(output);

	temp = input[0].position - up + right;
	output.position = mul(temp, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);
	output.tex = float2(1, 1);
	output.normal = normal;
	output.worldPos = mul(temp, worldMatrix);
	OutputStream.Append(output);

	OutputStream.RestartStrip();
}