//Texture Vertex Shader는 ColorShader와 비슷하지만 texturing을 포함하는 부분이 추가된다.

/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//더 이상 vertex type에 대해 color를 사용하지 않는다.
//텍스처의 좌표는 u와 v이므로 float2를 사용한다.

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

//이전 vertex shader와 다른 점은 input vertex로부터 color를 복사해오는 것이 아닌 텍스처 좌표를 가져오고 이것을 pixel shader에 넘기는 것이다.

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    return output;
}
