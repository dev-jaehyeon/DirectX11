
// Protean clouds by nimitz (twitter: @stormoid)
// https://www.shadertoy.com/view/3l23Rh
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
// Contact the author for other licensing options

/*
	Technical details:

	The main volume noise is generated from a deformed periodic grid, which can produce
	a large range of noise-like patterns at very cheap evalutation cost. Allowing for multiple
	fetches of volume gradient computation for improved lighting.

	To further accelerate marching, since the volume is smooth, more than half the the density
	information isn't used to rendering or shading but only as an underlying volume	distance to 
	determine dynamic step size, by carefully selecting an equation	(polynomial for speed) to 
	step as a function of overall density (not necessarily rendered) the visual results can be 
	the	same as a naive implementation with ~40% increase in rendering performance.

	Since the dynamic marching step size is even less uniform due to steps not being rendered at all
	the fog is evaluated as the difference of the fog integral at each rendered step.

*/

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

const static float3x3 m3 = float3x3(0.33338, 0.56034, -0.71817, -0.87887, 0.32651, -0.15323, 0.15162, 0.69596, 0.61339) * 1.93;
const static float2 iResolution = float2(800 , 600);
static float prm1 = 0.;
static float2 bsMo = float2(0,0);

float2x2 rot(float a)
{
    float c = cos(a), s = sin(a);
    return float2x2(c, s, -s, c);
}
float mag2(float2 p)
{
    return dot(p, p);
}
float linstep(float mn, float mx, float x)
{
    return clamp((x - mn) / (mx - mn), 0., 1.);
}


float2 disp(float t)
{
    return float2(sin(t * 0.22) * 1., cos(t * 0.175) * 1.) * 2.;
}

float2 map(float3 p)
{
    float3 p2 = p;
    p2.xy -= disp(p.z).xy;
    p.xy = mul(p.xy, rot(sin(p.z + iTime) * (0.1 + prm1 * 0.05) + iTime * 0.09));
    float cl = mag2(p2.xy);
    float d = 0.;
    p *= .61;
    float z = 1.;
    float trk = 1.;
    float dspAmp = 0.1 + prm1 * 0.2;
    for (int i = 0; i < 5; i++)
    {
        p += sin(p.zxy * 0.75 * trk + iTime * trk * .8) * dspAmp;
        d -= abs(dot(cos(p), sin(p.yzx)) * z);
        z *= 0.57;
        trk *= 1.4;
        p = mul(p, m3);
    }
    d = abs(d + prm1 * 3.) + prm1 * .3 - 2.5 + bsMo.y;
    return float2(d + cl * .2 + 0.25, cl);
}

float4 render(float3 ro, float3 rd, float time)
{
    float4 rez = float4(0,0,0,0);
    const float ldst = 8.;
    float3 lpos = float3(disp(time + ldst) * 0.5, time + ldst);
    float t = 1.5;
    float fogT = 0.;
    for (int i = 0; i < 130; i++)
    {
        if (rez.a > 0.99)
            break;

        float3 pos = ro + t * rd;
        float2 mpv = map(pos);
        float den = clamp(mpv.x - 0.3, 0., 1.) * 1.12;
        float dn = clamp((mpv.x + 2.), 0., 3.);
        
        float4 col = float4(0, 0,0,0);
        if (mpv.x > 0.6)
        {
        
            col = float4(sin(float3(5., 0.4, 0.2) + mpv.y * 0.1 + sin(pos.z * 0.4) * 0.5 + 1.8) * 0.5 + 0.5, 0.08);
            col *= den * den * den;
            col.rgb *= linstep(4., -2.5, mpv.x) * 2.3;
            float dif = clamp((den - map(pos + .8).x) / 9., 0.001, 1.);
            dif += clamp((den - map(pos + .35).x) / 2.5, 0.001, 1.);
            col.xyz *= den * (float3(0.005, .045, .075) + 1.5 * float3(0.033, 0.07, 0.03) * dif);
        }
		
        float fogC = exp(t * 0.2 - 2.2);
        col.rgba += float4(0.06, 0.11, 0.11, 0.1) * clamp(fogC - fogT, 0., 1.);
        fogT = fogC;
        rez = rez + col * (1. - rez.a);
        t += clamp(0.5 - dn * dn * .05, 0.09, 0.3);
    }
    return clamp(rez, 0.0, 1.0);
}

float getsat(float3 c)
{
    float mi = min(min(c.x, c.y), c.z);
    float ma = max(max(c.x, c.y), c.z);
    return (ma - mi) / (ma + 1e-7);
}

//from my "Will it blend" shader (https://www.shadertoy.com/view/lsdGzN)
float3 iLerp(in float3 a, in float3 b, in float x)
{
    float3 ic = lerp(a, b, x) + float3(1e-6, 0., 0.);
    float sd = abs(getsat(ic) - lerp(getsat(a), getsat(b), x));
    float3 dir = normalize(float3(2. * ic.x - ic.y - ic.z, 2. * ic.y - ic.x - ic.z, 2. * ic.z - ic.y - ic.x));
    float lgt = dot(float3(1.0, 1.0, 1.0), ic);
    float ff = dot(dir, normalize(ic));
    ic += 1.5 * dir * sd * ff * lgt;
    return clamp(ic, 0., 1.);
}


//fragCoord는 진짜 픽셀 좌표다. fragCoord는 0에서 뷰포트(1280 정도) 값의 범위를 가진다.
//iResolution은 뷰포트의 실제 좌표다.
//따라서 uv는 
float4 main(PixelShaderInput input) : SV_TARGET
{
    float2 iMouse = float2(0.75, 0.75);
    float2 q = input.texcoord.xy;
    float2 fragCoord = input.texcoord * iResolution.xy;
    float2 p = (fragCoord.xy - 0.5 * iResolution.xy) / iResolution.y;
    bsMo = (iMouse.xy - 0.5 * iResolution.xy) / iResolution.y;
    
    float time = iTime * 3.;
    float3 ro = float3(0, 0, time);
    
    ro += float3(sin(iTime) * 0.5, sin(iTime * 1.) * 0., 0);
        
    float dspAmp = .85;
    ro.xy += disp(ro.z) * dspAmp;
    float tgtDst = 3.5;
    
    float3 target = normalize(ro - float3(disp(time + tgtDst) * dspAmp, time + tgtDst));
    ro.x -= bsMo.x * 2.;
    float3 rightdir = normalize(cross(target, float3(0, 1, 0)));
    float3 updir = normalize(cross(rightdir, target));
    rightdir = normalize(cross(updir, target));
    float3 rd = normalize((p.x * rightdir + p.y * updir) * 1. - target);
    rd.xy = mul(rd.xy, rot(-disp(time + 3.5).x * 0.2 + bsMo.x));
    prm1 = smoothstep(-0.4, 0.4, sin(iTime * 0.3));
    float4 scn = render(ro, rd, time);
		
    float3 col = scn.rgb;
    col = iLerp(col.bgr, col.rgb, clamp(1. - prm1, 0.05, 1.));
    
    col = pow(col, float3(.55, 0.65, 0.6)) * float3(1., .97, .9);

    col *= pow(16.0 * q.x * q.y * (1.0 - q.x) * (1.0 - q.y), 0.12) * 0.7 + 0.3; //Vign
    return float4(col, 1.0);
    
}