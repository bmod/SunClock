#version 330

in vec2 pos;
in vec3 color;
out vec3 fragColor;
out vec3 position;

void main() {
    fragColor = color;
    gl_Position = vec4(pos, 0.0, 1.0);
}

//
//void main()
//{
//    // transform the vertex position
//    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
//
//    // transform the texture coordinates
//    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
//
//    // forward the vertex color
//    gl_FrontColor = gl_Color;
//}