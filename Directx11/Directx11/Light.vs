
/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//#10 카메라 정보를 담기위한 상수 버퍼를 선언한다. 셰이더에서 카메라의 위치를 알아야 한다. 그래야 이 버텍스가 어디서 보이는지를 알 수 있고, 이것이
//Specular 계산에 쓰인다.
cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};


//이제 매트릭스 버퍼 구조체는 3개의 3 float normal vector를 가진다. normal vector는 빛의 방향과 노말의 방향을 계산하는데 쓰인다.

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    //#10 버텍스가 보이는 위치를 계산하기 위해 추가됨
    float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType LightVertexShader(VertexInputType input)
{
    PixelInputType output;
    //#10
    float4 worldPosition;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    //이 버텍스의 노말 벡터는 world space에서 계산되며 pixel shader로 보내지기 전에 정규화한다.

    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    //#10 vertex Shader에서 viewing direction은 여기서 계산된다. vertex의 월드 포지션을 먼저 계산하고, 그것을 카메라 포지션에서 뺀다.
    //그러면 버텍스에서 카메라로 가는 벡터가 생성된다. 정규화를 거친 후 Pixel Shader로 보내진다.
    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}
