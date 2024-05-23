
/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

//#10 ī�޶� ������ ������� ��� ���۸� �����Ѵ�. ���̴����� ī�޶��� ��ġ�� �˾ƾ� �Ѵ�. �׷��� �� ���ؽ��� ��� ���̴����� �� �� �ְ�, �̰���
//Specular ��꿡 ���δ�.
cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
};


//���� ��Ʈ���� ���� ����ü�� 3���� 3 float normal vector�� ������. normal vector�� ���� ����� �븻�� ������ ����ϴµ� ���δ�.

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
    //#10 ���ؽ��� ���̴� ��ġ�� ����ϱ� ���� �߰���
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

    //�� ���ؽ��� �븻 ���ʹ� world space���� ���Ǹ� pixel shader�� �������� ���� ����ȭ�Ѵ�.

    // Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    //#10 vertex Shader���� viewing direction�� ���⼭ ���ȴ�. vertex�� ���� �������� ���� ����ϰ�, �װ��� ī�޶� �����ǿ��� ����.
    //�׷��� ���ؽ����� ī�޶�� ���� ���Ͱ� �����ȴ�. ����ȭ�� ��ģ �� Pixel Shader�� ��������.
    // Calculate the position of the vertex in the world.
    worldPosition = mul(input.position, worldMatrix);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    output.viewDirection = normalize(output.viewDirection);

    return output;
}
