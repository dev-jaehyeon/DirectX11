//Texture Vertex Shader�� ColorShader�� ��������� texturing�� �����ϴ� �κ��� �߰��ȴ�.

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//�� �̻� vertex type�� ���� color�� ������� �ʴ´�.
//�ؽ�ó�� ��ǥ�� u�� v�̹Ƿ� float2�� ����Ѵ�.

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType TextureVertexShader(VertexInputType input)
{
    PixelInputType output;
    

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

//���� vertex shader�� �ٸ� ���� input vertex�κ��� color�� �����ؿ��� ���� �ƴ� �ؽ�ó ��ǥ�� �������� �̰��� pixel shader�� �ѱ�� ���̴�.

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    return output;
}
