Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer SamplingPixelConstantData : register(b0)
{
    float dx;
    float dy;
    float threshHold;
    float strength;
    float iTime;
    float dummy[3];
}

struct PixelShaderInput
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD;
};

const static float3 iResolution = float3(800, 600, 0);
//static float4 sampled = float4(0, 0, 0, 0);

float noise(in float3 x)
{
    float3 p = floor(x);
    float3 f = frac(x);
    f = f * f * (3.0 - 2.0 * f);
	
    float2 uv = (p.xy + float2(37.0, 17.0) * p.z) + f.xy;
    float2 rg = shaderTexture.Sample(SampleType, (uv + 0.5) / float2(256.0, 256.0)).yx;
    //float2 rg = textureLod(iChannel0, (uv + 0.5) / 256.0, 0.0).yx;
    return lerp(rg.x, rg.y, f.z);
}

float4 map(float3 p)
{
    float den = 0.2 - p.y;

    // invert space	
    p = -7.0 * p / dot(p, p);

    // twist space	
    float co = cos(den - 0.25 * iTime);
    float si = sin(den - 0.25 * iTime);
    p.xz = mul(float2x2(co, -si, si, co), p.xz);

    // smoke	
    float f;
    float3 q = p - float3(0.0, 1.0, 0.0) * iTime;;
    f = 0.50000 * noise(q);
    q = q * 2.02 - float3(0.0, 1.0, 0.0) * iTime;
    f += 0.25000 * noise(q);
    q = q * 2.03 - float3(0.0, 1.0, 0.0) * iTime;
    f += 0.12500 * noise(q);
    q = q * 2.01 - float3(0.0, 1.0, 0.0) * iTime;
    f += 0.06250 * noise(q);
    q = q * 2.02 - float3(0.0, 1.0, 0.0) * iTime;
    f += 0.03125 * noise(q);

    den = den + 4.0 * f;
	
    float3 col = lerp(float3(1.0, 0.9, 0.8), float3(0.4, 0.15, 0.1), den) + 0.05 * sin(p);
	
    return float4(col, den);
}

float3 raymarch(in float3 ro, in float3 rd, in float2 pixel)
{
    float4 sum = float4(0, 0, 0,0);

    // dithering	
    float t = 0.05 * frac(10.5421 * dot(float2(0.0149451, 0.038921), pixel));
	
    for (int i = 0; i < 1; i++)
    {
        float3 pos = ro + t * rd;
        float4 col = map(pos);
        if (col.w > 0.0)
        {
            //float len = length(pos);
            col.w = min(col.w, 1.0);
            
            col.xyz *= lerp(3.1 * float3(1.0, 0.5, 0.05), float3(0.48, 0.53, 0.5), clamp((pos.y - 0.2) / 1.9, 0.0, 1.0));
            //col.xyz *= mix( 3.1*float3(1.0,0.5,0.05), float3(0.48,0.53,0.5), clamp( 0.35*col.w+0.15*dot(pos,pos), 0.0, 1.0 ) );

            col.a *= 0.6;
            col.rgb *= col.a;

            sum = sum + col * (1.0 - sum.a);
            if (sum.a > 0.99)
                break;
        }
        t += 0.05;
    }

    return clamp(sum.xyz, 0.0, 1.0);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
    float2 fragCoord = input.texcoord * iResolution.xy;
    float2 p = (2.0 * fragCoord - iResolution.xy) / iResolution.xy;
	
    // camera
    float3 ro = 4.0 * normalize(float3(1.0, 1.5, 0.0));
    float4 sampled = shaderTexture.Sample(SampleType, input.texcoord);
    //float4 sampled = shaderTexture.Sample(SampleType, input.texcoord);
    //return sampled;
    float3 ta = float3(0.0, 1.0, 0.0) + 0.05 * (-1.0 + 2.0 * sampled.xyz);
    float cr = 0.5 * cos(0.7 * iTime);
	
	// build ray
    float3 ww = normalize(ta - ro);
    float3 uu = normalize(cross(float3(sin(cr), cos(cr), 0.0), ww));
    float3 vv = normalize(cross(ww, uu));
    float3 rd = normalize(p.x * uu + p.y * vv + 2.0 * ww);
	
    // raymarch	
    float3 col = raymarch(ro, rd, fragCoord);
    //float3 col = sampled.xyz;
	// color grade
    col = col * 0.5 + 0.5 * col * col * (3.0 - 2.0 * col);
    
    // vignetting	
    float2 q = fragCoord.xy / iResolution.xy;
    col *= 0.2 + 0.8 * pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.1);
	
    return float4(col, 1.0);
}
