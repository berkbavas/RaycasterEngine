#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, location = 0, binding = 0) uniform image2D outputImage;
layout(rgba8, location = 1, binding = 1) uniform image2D textures;
layout(rgba32f, location = 2, binding = 2) uniform image1D depthBuffer;

struct Sprite {
    int width;
    int height;
    int screenX;
    int textureIndex;
};

struct Draw {
    int startX;
    int endX;
    int startY;
    int endY;
    float transformX;
    float transformY;
};

struct Screen {
    int width;
    int height;
};

uniform Sprite sprite;
uniform Draw draw;
uniform Screen screen;

// Code taken from https://lodev.org/cgtutor/raycasting.html and adopted.
void main()
{
    int x = int(gl_GlobalInvocationID.x) + draw.startX;
    int tx = int(256 * (x - (sprite.screenX - sprite.width / 2)) * 64 / sprite.width) / 256;
    float depth = imageLoad(depthBuffer, x).r;

    if (0 < draw.transformY && draw.transformY < depth)
    {
        for (int y = draw.startY; y < draw.endY; y++) // For every pixel of the current stripe
        {
            int d = 256 * y - screen.height * 128 + sprite.height * 128; // 256 and 128 factors to avoid floats
            int ty = ((d * 64) / sprite.height) / 256;

            vec4 color = imageLoad(textures, ivec2(64 * sprite.textureIndex + tx, 64 - ty));
            if (color.r > 0 || color.g > 0 || color.b > 0) // Black is the invisible color
                imageStore(outputImage, ivec2(x, y), color);
        }
    }
}
