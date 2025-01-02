//Texture Pixel Shader�� �� ���� ���� ������ ������.
//ù ��°�� Texture2D shaderTexture��, �ؽ�ó ���ҽ��� �ǹ��Ѵ�. �̰��� �𵨿� �ؽ�ó�� �������ϴµ� ���ȴ�.
//�� ��°�� SamplerState SamplerType�̴�. Sampler State�� �������� ���� ���̵��� �� �ȼ����� ��� ���̴��� ������ �� �ְ� ���ش�.
//���� ���, �������� �ʹ� �ָ� �־ ���� 8���� �ȼ��� ��ũ���� ��Ÿ���ٰ� ����.
//�׷��� �츰 Sample State�� �̿��� ���� �ؽ�ó�κ��� � ������ �ȼ�, Ȥ�� � �ȼ����� ��� �� �׷����� �˾Ƴ� �� �ִ�. ���� �ؽ�ó�� 256x256�̶��,
//� �ȼ����� �׷����� �����ϴ� ���� �ſ� �߿��ϴ�. ���� ���� �������� �鿡 �ؽ�ó�� ������ �׷����� ��ó�� ���̱� ���ؼ��� ���̴�.
//TextureShaderClass���� �� SamplerState�� �����ϰ� ���ҽ� �����Ϳ� ���� ���̴�. �׷��� �� pixel shader�� �ȼ��� � sample�� �׸� ������ ������ �� ���� ���̴�.

/////////////
// GLOBALS //
/////////////
Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

//texture pixel shader�� ���� PixelInputType�� color ��� texture coordinate�� ����Ͽ� �����ȴ�.

//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

//Pixel Shader�� HLSL sample �Լ��� ����ϱ� ���� ����ȴ�. sample �Լ��� �� �ȼ��� ���� ������ ������ sampler state�� �ؽ�ó ��ǥ�� ����Ѵ�.
//�� �� ������ ����� ������ ���� UV ��ġ�� ���� �ȼ����� ��ȯ�Ѵ�.

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
