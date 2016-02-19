cbuffer matrixBufferBillboarding : register(b6)
{
	matrix worldMatrix;
	matrix camViewMatrix;
	matrix camProjectionMatrix;
	float3 campos;
	float scale;
	int textureCount;
	int isIcon;
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
	float textureNumber : TEXNUM;
};

//Gives random number in the range [0, 0.99999...]
float random(float2 p)
{
	const float2 randomSeed = float2(23.1406926327792690, 2.6651441426902251);
	return (frac(cos(123456789.0f % 1e-7 + 256.0f * dot(p, randomSeed))));
}

[maxvertexcount(4)]
void main(point GS_IN input[1], inout TriangleStream<GS_OUT> OutputStream)
{
	GS_OUT output = (GS_OUT)0;

	float3 particleToCam = campos - mul(input[0].position, worldMatrix).xyz;
	float3 normal;
	float4 up;
	float4 right;
	float textureNumber;

	if (isIcon > 0)
	{
		up = float4(0, 1, 0, 0) * scale;
		right = float4(normalize(cross(particleToCam, up.xyz)).xyz, 0) * scale;
		normal = cross(up, right).xyz;
		normal = normalize(normal.xyz);
		textureNumber = 0;
	}
	else
	{
		normal = normalize(particleToCam);
		up = float4(0, 0, 1, 0) * scale;
		right = float4(normalize(cross(particleToCam, up.xyz)).xyz, 0) * scale;
		up = float4(normalize(cross(particleToCam, right.xyz)).xyz, 0) * scale;

		textureNumber = textureCount * random(input[0].position.xz);
	}

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
	output.textureNumber = textureNumber;

	OutputStream.Append(output);

	//Corner 2
	output.position = mul(input[0].position + up - right, worldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(0, 0);
	output.normal = normal;
	output.textureNumber = textureNumber;

	OutputStream.Append(output);

	//Corner 3
	output.position = mul(input[0].position - up + right, worldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(1, 1);
	output.normal = normal;
	output.textureNumber = textureNumber;

	OutputStream.Append(output);

	//Corner 4
	output.position = mul(input[0].position + up + right, worldMatrix);
	output.position = mul(output.position, camViewMatrix);
	output.position = mul(output.position, camProjectionMatrix);
	output.tex = float2(1, 0);
	output.normal = normal;
	output.textureNumber = textureNumber;

	OutputStream.Append(output);

	OutputStream.RestartStrip();
}