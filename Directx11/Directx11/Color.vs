//이 셰이더의 목적은 실제로 모델을 렌더랑하는 것이다.


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

//Vertex Shader는 GPU로 보내진 Vertex Buffer의 데이터를 처리할 때 GPU에 의해 호출된다. Vertex Shader로의 입력은 반드시 Vertex Buffer 뿐만 아니라
//셰이더의 타입 정의와 일치해야 한다.
//Vertex Shader의 출력은 Pixel Input 타입인데, 이것은 그 위치를 취하고 World, View, Projection Matrix와 연산된다. 즉, Vertex가 3D공간에
//올바른 위치(우리가 스크린으로 봤을 때)에 놓여질 것이다. 

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}