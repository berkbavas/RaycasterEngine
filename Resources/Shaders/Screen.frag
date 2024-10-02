#version 450 core

uniform sampler2D screenTexture;

in vec2 fs_TextureCoords;

out vec4 out_Color;

void main()
{
    out_Color = texture(screenTexture, fs_TextureCoords);
}
