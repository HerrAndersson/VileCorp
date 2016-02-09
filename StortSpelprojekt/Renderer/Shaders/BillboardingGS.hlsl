cbuffer matrixBufferBillboarding : register(b6)
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

[maxvertexcount(6)] //TODO: Use triangle strip instead of triangle list /Jonas
void main(point GS_IN input[1], inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output = (GS_OUT)0;
	float scale = 1.0f;

	//TODO: Replace all world*view*projection with a single mul using this matrix /Jonas
	matrix wvp = mul(mul(emitterWorldMatrix, camViewMatrix), camProjectionMatrix);

	float4 up = float4(0, 1, 0, 0) * scale;
	float4 particlepos = mul(input[0].position, emitterWorldMatrix);
	float4 right = float4(normalize(cross(campos - particlepos.xyz, up.xyz)).xyz, 0) * scale;
	float4 temp;

	float3 normal = normalize(cross(up.xyz, right.xyz));

	//First triangle
	temp = input[0].position + up - right;
	output.position = mul(temp, emitterWorldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(0, 0);
	output.normal = normal;
	output.worldPos = mul(temp, emitterWorldMatrix);
	OutputStream.Append(output);

	temp = input[0].position + up + right;
	output.position = mul(temp, emitterWorldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(1, 0);
	output.normal = normal;
	output.worldPos = mul(temp, emitterWorldMatrix);
	OutputStream.Append(output);

	temp = input[0].position - up + right;
	output.position = mul(temp, emitterWorldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(1, 1);
	output.normal = normal;
	output.worldPos = mul(temp, emitterWorldMatrix);
	OutputStream.Append(output);

	//Second triangle
	temp = input[0].position + up - right;
	output.position = mul(temp, emitterWorldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(0, 0);
	output.normal = normal;
	output.worldPos = mul(temp, emitterWorldMatrix);
	OutputStream.Append(output);

	temp = input[0].position - up - right;
	output.position = mul(temp, emitterWorldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(0, 1);
	output.normal = normal;
	output.worldPos = mul(temp, emitterWorldMatrix);
	OutputStream.Append(output);

	temp = input[0].position - up + right;
	output.position = mul(temp, emitterWorldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(1, 1);
	output.normal = normal;
	output.worldPos = mul(temp, emitterWorldMatrix);
	OutputStream.Append(output);

	OutputStream.RestartStrip();
}