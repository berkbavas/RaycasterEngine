#version 450 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, location = 0, binding = 0) uniform image2D outputImage;
layout(r8i, location = 1, binding = 1) uniform iimage2D worldMap;

struct Player
{
    vec2 position;
    vec2 direction;
};

struct Camera
{
    vec2 plane;
};

struct Screen
{
    int width;
    int height;
};

uniform Player player;
uniform Camera camera;
uniform Screen screen;

// Code taken from https://lodev.org/cgtutor/raycasting.html and adopted.
void main()
{
    float x = float(gl_GlobalInvocationID.x);

    // Calculate ray position and direction
    float cameraX = 2 * x / screen.width - 1; // x-coordinate in camera space
    vec2 rayDir = vec2(player.direction.x + camera.plane.x * cameraX, player.direction.y + camera.plane.y * cameraX);

    // Which box of the map we're in
    ivec2 map = ivec2(int(player.position.x), int(player.position.y));

    // Length of ray from current position to next x or y-side
    vec2 sideDist;
    vec2 deltaDist;

    deltaDist.x = (rayDir.x == 0) ? 1e30 : abs(1.0 / rayDir.x);
    deltaDist.y = (rayDir.y == 0) ? 1e30 : abs(1.0 / rayDir.y);

    float perpWallDist;

    // What direction to step in x or y-direction (either +1 or -1)
    int stepX;
    int stepY;

    int hit = 0; // Was there a wall hit?
    int side;    // Was a NS or a EW wall hit?

    // Calculate step and initial sideDist
    if (rayDir.x < 0)
    {
        stepX = -1;
        sideDist.x = (player.position.x - map.x) * deltaDist.x;
    }
    else
    {
        stepX = 1;
        sideDist.x = (map.x + 1.0 - player.position.x) * deltaDist.x;
    }

    if (rayDir.y < 0)
    {
        stepY = -1;
        sideDist.y = (player.position.y - map.y) * deltaDist.y;
    }
    else
    {
        stepY = 1;
        sideDist.y = (map.y + 1.0 - player.position.y) * deltaDist.y;
    }

    int dof = 0;
    // Perform DDA
    while (hit == 0 && dof < 100)
    {
        // Jump to next map square, either in x-direction, or in y-direction
        if (sideDist.x < sideDist.y)
        {
            sideDist.x += deltaDist.x;
            map.x += stepX;
            side = 0;
        }
        else
        {
            sideDist.y += deltaDist.y;
            map.y += stepY;
            side = 1;
        }

        int node = imageLoad(worldMap, map).r;

        // Check if ray has hit a wall
        if (node > 0)
            hit = 1;

        dof++;
    }

    // Calculate distance projected on camera direction. This is the shortest distance from the point where the wall is
    // hit to the camera plane. Euclidean to center camera point would give fisheye effect!
    // This can be computed as (mapX - posX + (1 - stepX) / 2) / rayDirX for side == 0, or same formula with Y
    // for size == 1, but can be simplified to the code below thanks to how sideDist and deltaDist are computed:
    // because they were left scaled to |rayDir|. sideDist is the entire length of the ray above after the multiple
    // steps, but we subtract deltaDist once because one step more into the wall was taken above.
    if (side == 0)
        perpWallDist = (sideDist.x - deltaDist.x);
    else
        perpWallDist = (sideDist.y - deltaDist.y);

    // Calculate height of line to draw on screen
    int lineHeight = int(screen.height / perpWallDist);

    // Calculate lowest and highest pixel to fill in current stripe
    int drawStart = int(-lineHeight / 2 + screen.height / 2.0f);
    if (drawStart < 0)
        drawStart = 0;

    int drawEnd = int(lineHeight / 2 + screen.height / 2.0f);
    if (drawEnd >= screen.height)
        drawEnd = int(screen.height - 1);

    vec3 color;
    int node = imageLoad(worldMap, map).r;

    if (node == 1)
        color = vec3(1, 0, 0);
    else if (node == 2)
        color = vec3(0, 1, 0);
    else if (node == 3)
        color = vec3(0, 0, 1);
    else if (node == 4)
        color = vec3(1, 1, 1);
    else
        color = vec3(1, 1, 0);

    // Give x and y sides different brightness
    if (side == 1)
    {
        color = color / 2;
    }

    for (int y = drawStart; y < drawEnd; y++)
    {
        imageStore(outputImage, ivec2(gl_GlobalInvocationID.x, y), vec4(color, 1));
    }
}
