uniform sampler2D texture;

void main()
{
    // multiply it by the color
    vec2 uv = gl_TexCoord[0];
    gl_FragColor = vec4(uv.x, uv.y, 0, 1);
}