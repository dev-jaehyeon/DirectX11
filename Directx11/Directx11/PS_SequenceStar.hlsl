Texture2D shaderTexture : register(t0);
SamplerState sampleType : register(s0);


cbuffer SamplingPixelConstantData : register(b0)
{
    float dx;
    float dy;
    float threshold;
    float strength;
    float iTime;
    float dummy[3];
};


struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

float snoise(float3 uv, float res)	// by trisomie21
{
    const float3 s = float3(1e0, 1e2, 1e4);
	
    uv *= res;
	
    float3 uv0 = floor(fmod(uv, res)) * s;
    float3 uv1 = floor(fmod(uv + float3(1, 1, 1), res)) * s;
	
    float3 f = frac(uv);
    f = f * f * (3.0 - 2.0 * f);
	
    float4 v = float4(uv0.x + uv0.y + uv0.z, uv1.x + uv0.y + uv0.z,
		      	  uv0.x + uv1.y + uv0.z, uv1.x + uv1.y + uv0.z);
	
    float4 r = frac(sin(v * 1e-3) * 1e5);
    float r0 = lerp(lerp(r.x, r.y, f.x), lerp(r.z, r.w, f.x), f.y);
	
    r = frac(sin((v + uv1.z - uv0.z) * 1e-3) * 1e5);
    float r1 = lerp(lerp(r.x, r.y, f.x), lerp(r.z, r.w, f.x), f.y);
	
    return lerp(r0, r1, f.z) * 2. - 1.;
}

static float freqs[4];
const static float3 iResolution = float3(800, 600, 0);

float4 main(PixelShaderInput input) : SV_TARGET
{
    float2 fragCoord = input.texcoord / iResolution.xy;
    float4 sampled = shaderTexture.Sample(sampleType, input.texcoord).x;
    freqs[0] = shaderTexture.Sample(sampleType, float2(0.01, 0.25)).x;
    freqs[1] = shaderTexture.Sample(sampleType, float2(0.07, 0.25)).x;
    freqs[2] = shaderTexture.Sample(sampleType, float2(0.15, 0.25)).x;
    freqs[3] = shaderTexture.Sample(sampleType, float2(0.30, 0.25)).x;

    float brightness = freqs[1] * 0.25 + freqs[2] * 0.25;
    float radius = 0.24 + brightness * 0.2;
    float invRadius = 1.0 / radius;
	
    float3 orange = float3(0.8, 0.65, 0.3);
    float3 orangeRed = float3(0.8, 0.35, 0.1);
    float time = iTime * 0.1;
    float aspect = iResolution.x / iResolution.y;
    float2 uv = input.texcoord;
    float2 p = -0.5 + uv;
    p.x *= aspect;

    float fade = pow(length(2.0 * p), 0.5);
    float fVal1 = 1.0 - fade;
    float fVal2 = 1.0 - fade;
	
    float angle = atan2(p.x, p.y) / 6.2832;
    float dist = length(p);
    float3 coord = float3(angle, dist, time * 0.1);
	
    float newTime1 = abs(snoise(coord + float3(0.0, -time * (0.35 + brightness * 0.001), time * 0.015), 15.0));
    float newTime2 = abs(snoise(coord + float3(0.0, -time * (0.15 + brightness * 0.001), time * 0.015), 45.0));
    for (int i = 1; i <= 7; i++)
    {
        float power = pow(2.0, float(i + 1));
        fVal1 += (0.5 / power) * snoise(coord + float3(0.0, -time, time * 0.2), (power * (10.0) * (newTime1 + 1.0)));
        fVal2 += (0.5 / power) * snoise(coord + float3(0.0, -time, time * 0.2), (power * (25.0) * (newTime2 + 1.0)));
    }
	
    float corona = pow(fVal1 * max(1.1 - fade, 0.0), 2.0) * 50.0;
    corona += pow(fVal2 * max(1.1 - fade, 0.0), 2.0) * 50.0;
    corona *= 1.2 - newTime1;
    float3 sphereNormal = float3(0.0, 0.0, 1.0);
    float3 dir = float3(0, 0, 0);
    float3 center = float3(0.5, 0.5, 1.0);
    float3 starSphere = float3(0, 0, 0);
	
    float2 sp = -1.0 + 2.0 * uv;
    sp.x *= aspect;
    sp *= (2.0 - brightness);
    float r = dot(sp, sp);
    float f = (1.0 - sqrt(abs(1.0 - r))) / (r) + brightness * 0.5;
    if (dist < radius)
    {
        corona *= pow(dist * invRadius, 24.0);
        float2 newUv;
        newUv.x = sp.x * f;
        newUv.y = sp.y * f;
        newUv += float2(time, 0.0);
		
        float3 texSample = shaderTexture.Sample(sampleType, newUv).xyz;
        float uOff = (texSample.g * brightness * 4.5 + time);
        float2 starUV = newUv + float2(uOff, 0.0);
        starSphere = shaderTexture.Sample(sampleType, starUV).xyz;
    }
	
    float starGlow = min(max(1.0 - dist * (1.0 - brightness), 0.0), 1.0);
	float4 fragColor = float4( 0,0,0,0 );
    fragColor.xyz = float3(f * (0.75 + brightness * 0.3) * orange) + starSphere + corona * orange + starGlow * orangeRed;
    fragColor.a = 1.0;
    return fragColor;
}