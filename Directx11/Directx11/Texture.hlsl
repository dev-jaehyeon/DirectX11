//Texture Pixel Shader는 두 개의 전역 변수를 가진다.
//첫 번째는 Texture2D shaderTexture로, 텍스처 리소스를 의미한다. 이것은 모델에 텍스처를 렌더링하는데 사용된다.
//두 번째는 SamplerState SamplerType이다. Sampler State는 폴리곤의 면이 셰이딩될 때 픽셀들이 어떻게 쓰이는지 조정할 수 있게 해준다.
//예를 들어, 폴리곤이 너무 멀리 있어서 오직 8개의 픽셀만 스크린에 나타난다고 하자.
//그러면 우린 Sample State를 이용해 원본 텍스처로부터 어떤 조합의 픽셀, 혹은 어떤 픽셀들이 사실 상 그려질지 알아낼 수 있다. 원본 텍스처가 256x256이라면,
//어떤 픽셀들이 그려질지 결정하는 것은 매우 중요하다. 아주 작은 폴리곤의 면에 텍스처가 여전히 그려지는 것처럼 보이기 위해서는 말이다.
//TextureShaderClass에서 이 SamplerState를 설정하고 리소스 포인터에 붙일 것이다. 그러면 이 pixel shader가 픽셀의 어떤 sample을 그릴 젓인지 결정할 수 있을 것이다.

/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

//texture pixel shader를 위한 PixelInputType은 color 대신 texture coordinate를 사용하여 조정된다.

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

//Pixel Shader는 HLSL sample 함수를 사용하기 위해 변경된다. sample 함수는 이 픽셀에 대해 위에서 정의한 sampler state와 텍스처 좌표를 사용한다.
//이 두 변수를 사용해 폴리곤 면의 UV 위치를 위한 픽셀값을 반환한다.

////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 main(PixelInputType input) : SV_TARGET
{
    float4 textureColor;


    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
    textureColor = shaderTexture.Sample(SampleType, input.tex);

    return textureColor;
}
