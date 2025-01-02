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

#define iterations 13
#define formuparam 0.53

#define volsteps 20
#define stepsize 0.1

#define zoom   0.800
#define tile   0.850
#define speed  0.010

#define brightness 0.0015
#define darkmatter 0.300
#define distfading 0.730
#define saturation 0.850

//#define time iTime

#define PI 3.141592
#define TWOPI 6.283184

#define R2D 180.0/PI
#define D2R PI/180.0

const static float2 iResolution = float2(1280, 720);

float2x2 rotMat(in float r)
{
    float c = cos(r);
    float s = sin(r);
    return float2x2(c, -s, s, c);
}

//frac -> -0.5 -> ABS  : coordinate absolute Looping
float abs1d(in float x)
{
    return abs(frac(x) - 0.5);
}
float2 abs2d(in float2 v)
{
    return abs(frac(v) - 0.5);
}
float cos1d(float p)
{
    return cos(p * TWOPI) * 0.25 + 0.25;
}
float sin1d(float p)
{
    return sin(p * TWOPI) * 0.25 + 0.25;
}

#define OC 15.0
float3 Oilnoise(in float2 pos, in float3 RGB)
{
    float2 q = float2(0, 0);
    float result = 0.0;
   
    float s = 2.2;
    float gain = 0.44;
    float2 aPos = abs2d(pos) * 0.5; //add pos

    for (float i = 0.0; i < OC; i++)
    {
        pos = mul(pos, rotMat(D2R * 30.0));
        float time = (sin(iTime) * 0.5 + 0.5) * 0.2 + iTime * 0.8;
        q = pos * s + time;
        q = pos * s + aPos + time;
        q = float2(cos(q));

        result = result + sin1d(dot(q, float2(0.3, 0.3))) * gain;

        s *= 1.07;
        aPos += cos(smoothstep(0.0, 0.15, q));
        aPos = mul(aPos, rotMat(D2R * 5.0));
        aPos *= 1.232;
    }
   
    result = pow(result, 4.504);
    return clamp(RGB / abs1d(dot(q, float2(-0.240, 0.000))) * .5 / result, float3(0,0,0), float3(1,1,1));
}


float easeFade(float x)
{
    return 1. - (2. * x - 1.) * (2. * x - 1.) * (2. * x - 1.) * (2. * x - 1.);
}
float holeFade(float t, float life, float lo)//lifeOffset
{
    return easeFade(fmod(t - lo, life) / life);
}
float2 getPos(float t, float life, float offset, float lo)
{
    return float2(cos(offset + floor((t - lo) / life) * life) * 1280 / 2.,
    sin(2. * offset + floor((t - lo) / life) * life) * 720 / 2.);

}

void mainVR(out float4 fragColor, in float2 fragCoord, in float3 ro, in float3 rd)
{
//get coords and direction
    float3 dir = float3(1, 1, 0);
    float3 from = float3(0, 0, 0);

//volumetric rendering
    float s = 0.1, fade = 1.;
    float3 v = float3(0, 0,0);
    for (int r = 0; r < volsteps; r++)
    {
        float3 p = from + s * dir * .5;
        p = abs(float3(tile, tile, tile) - fmod(p, float3(tile * 2.0, tile * 2.0, tile * 2.0))); // tiling fold
        float pa, a = pa = 0.;
        for (int i = 0; i < iterations; i++)
        {
            p = abs(p) / dot(p, p) - formuparam;
            p.xy = mul(p.xy, float2x2(cos(iTime * 0.01), sin(iTime * 0.01), -sin(iTime * 0.01), cos(iTime * 0.01))); // the magic formula
            a += abs(length(p) - pa); // absolute sum of average change
            pa = length(p);
        }
        float dm = max(0., darkmatter - a * a * .001); //dark matter
        a *= a * a; // add contrast
        if (r > 6)
            fade *= 1.3 - dm; // dark matter, don't render near
//v+=float3(dm,dm*.5,0.);
        v += fade;
        v += float3(s, s * s, s * s * s * s) * a * brightness * fade; // coloring based on distance
        fade *= distfading; // distance fading
        s += stepsize;
    }
    v = lerp(float3(length(v), length(v), length(v)), v, saturation); //color adjust
    fragColor = float4(v * .01, 1.);
}
float happy_star(float2 uv, float anim)
{
    uv = abs(uv);
    float2 pos = min(uv.xy / uv.yx, anim);
    float p = (2.0 - pos.x - pos.y);
    return (2.0 + p * (p * p - 1.5)) / (uv.x + uv.y);
}

#define Q(p) p *= 2.*r(round(atan(p.x, p.y) * 4.) / 4.)
#define r(a) float2x2(cos(a + asin(float4(0,1,-1,0))))


float4 main(PixelShaderInput input) : SV_TARGET
{
    float4 fragColor = float4(0,1,0,0);
    float4 o = fragColor;
    float2 uv = input.texcoord;
    float2 u = input.texcoord;
    float2 cPos = -1.0 + 2.0 * uv.xy / uv.xy;
   
    // distance of current pixel from center
    float cLength = length(cPos);


    float2 st = (input.texcoord / iResolution.xy);
    st.x = ((st.x - 0.5) * (iResolution.x / iResolution.y)) + 0.5;
    float stMask = step(0.0, st.x * (1.0 - st.x));


    //st-=.5; //st move centor. Oil noise sampling base to 0.0 coordinate
    st *= 3.;
   
    float3 rgb = float3(0.30, .8, 1.200);
   
   
    //berelium, 2024-06-07 - anti-aliasing
    float AA = 1.0;
    float2 pix = 1.0 / iResolution.xy;
    float2 aaST = float2(0, 0);
    float3 col = float3(0, 0, 0);
    for (float i = 0.0; i < AA; i++)
    {
        for (float j = 0.0; j < AA; j++)
        {
            aaST = st + pix * float2((i + 0.5) / AA, (j + 0.5) / AA);
            col += Oilnoise(aaST, rgb);
        }
   
    }
   
    col /= AA * AA;
    uv.y *= iResolution.y / iResolution.x;
    float2 v = iResolution.xy,
         w,
         k = u = .2 * (u + u - v) / v.y;
         
    o = float4(1, 2, 3, 0);
     
    
    for (float a = .5, t = iTime * 0.21, iter;
         ++iter < 19.;
         o += (1. + cos(float4(0, 1, 3, 0) + t))
           / length((1. + iter * dot(v, v)) * sin(w * 3. - 9. * u.yx + t))
         )  
        v = cos(++t - 7. * u * pow(a += .03, iter)) - 5. * u,
        u = mul(u, float2x2(cos(iter + t * .02 - float4(0, 11, 33, 0)))),
        u += .005 * tanh(40. * dot(u, u) * cos(1e2 * u.yx + t))
           + .2 * a * u
           + .003 * cos(t + 4. * exp(-.01 * dot(o, o))),
        w = u / (1. - 2. * dot(u, u));
             
    o = pow(o = 1. - sqrt(exp(-o * o * o / 2e2)), .3 * o / o)
      - dot(k -= u, k) / 250.;
    float3 dir = float3(uv * zoom, 1.);

    float2 resolution = iResolution.xy;

    // Initialize color and texture accumulators
    float4 color = float4(1.0, 2.0, 3.0, 0.0);
    float4 baseColor = color;
   
    // Initialize time and amplitude variables

    float amplitude = 0.5;
    float2 coord = input.texcoord * 2. - iResolution.xy;
    // Normalized pixel coordinates (from 0 to 1)
 
   
   
    float holeSize = iResolution.y / 10.;
    float holeLife = 2.;
   
   
    float3 final;
    for (int iter = 0; iter < 45; iter++)
    {
        float3 col = 0.5 + 0.5 * cos(iTime + uv.xyx + float3(float(iter), 2. * float(iter) + 4., 4. * float(iter) + 16.));

        float s = holeSize;
        float lifeOffset = float(iter) / 2.;

        float2 pos = getPos(iTime, holeLife, float(iter) * 4.5, lifeOffset);

        float d = distance(coord, pos) / s;
        d = 1. / d - .1;
       
        final += lerp(float3(0,0,0), col, d) * holeFade(iTime, holeLife, lifeOffset);
    }
    float2 pos = 0.5 - uv;
    // Adjust y by aspect for uniform transforms
    pos.y /= iResolution.x / iResolution.y;
   
    //**********         Glow        **********
   
    // Equation 1/x gives a hyperbola which is a nice shape to use for drawing glow as
    // it is intense near 0 followed by a rapid fall off and an eventual slow fade
    float dist = 1.0 / length(pos);
   
    //**********        Radius       **********
   
    // Dampen the glow to control the radius
    dist *= 0.1;
   
    //**********       Intensity     **********
   
    // Raising the result to a power allows us to change the glow fade behaviour
    // See https://www.desmos.com/calculator/eecd6kmwy9 for an illustration
    // (Move the slider of m to see different fade rates)
    dist = pow(dist, 0.8);
   
    // Knowing the distance from a fragment to the source of the glow, the above can be
    // written compactly as:
    // float getGlow(float dist, float radius, float intensity){
    // return pow(radius/dist, intensity);
// }
    // The returned value can then be multiplied with a colour to get the final result
       
    // Add colour
    float3 col2 = dist * float3(1.0, 2.5, 1.25);

    // Tonemapping. See comment by P_Malin
    col2 = 1.0 - exp(-col);
     
    float2 uv2 = tanh(uv);
    uv2 *= 10.;
 
    // Final color adjustment for visual output
   
    float3 from = float3(1., .5, 0.5);
    //Q(from.xy);
    from.xy += (cPos / cLength) * cos(cLength * 8.0 - iTime * 2.0) * 0.03;
     
    mainVR(fragColor, input.texcoord, from, dir);
   
    fragColor *= float4(final * float3(0.4, 1., 1.) + o.xyz, 1.);
    fragColor += float4(col2, 1.);
    
    return fragColor;

}