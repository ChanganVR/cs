#version 330 core
in vec2 TexCoord;
out vec4 color;
uniform sampler2D texture_diffuse1;
uniform float clearNess;
void main()
{
    color = vec4(texture(texture_diffuse1, TexCoord));
}