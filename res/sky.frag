uniform sampler2D texture;

void main()
{
    // multiply it by the color
    vec2 uv = gl_TexCoord[0].xy;
    gl_FragColor = vec4(uv.x, uv.y, 0.0, 1.0);
}