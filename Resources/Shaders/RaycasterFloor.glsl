#version 450 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, location = 0, binding = 0) uniform image2D outputImage;
layout(rgba8, location = 1, binding = 1) uniform image2D textures;

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

void main()
{
    uint y = gl_GlobalInvocationID.y + int(screen.height / 2) + 1;

    // rayDir for leftmost ray (x = 0) and rightmost ray (x = w)
    float rayDirX0 = player.direction.x - camera.plane.x;
    float rayDirY0 = player.direction.y - camera.plane.y;
    float rayDirX1 = player.direction.x + camera.plane.x;
    float rayDirY1 = player.direction.y + camera.plane.y;

    // Current y position compared to the center of the screen (the horizon)
    int p = int(y - screen.height / 2.0f);

    // Vertical position of the camera.
    // NOTE: with 0.5, it's exactly in the center between floor and ceiling,
    // matching also how the walls are being raycasted. For different values
    // than 0.5, a separate loop must be done for ceiling and floor since
    // they're no longer symmetrical.
    float posZ = 0.5 * screen.height;

    // Horizontal distance from the camera to the floor for the current row.
    // 0.5 is the z position exactly in the middle between floor and ceiling.
    // NOTE: this is affine texture mapping, which is not perspective correct
    // except for perfectly horizontal and vertical surfaces like the floor.
    // NOTE: this formula is explained as follows: The camera ray goes through
    // the following two points: the camera itself, which is at a certain
    // height (posZ), and a point in front of the camera (through an imagined
    // vertical plane containing the screen pixels) with horizontal distance
    // 1 from the camera, and vertical position p lower than posZ (posZ - p). When going
    // through that point, the line has vertically traveled by p units and
    // horizontally by 1 unit. To hit the floor, it instead needs to travel by
    // posZ units. It will travel the same ratio horizontally. The ratio was
    // 1 / p for going through the camera plane, so to go posZ times farther
    // to reach the floor, we get that the total horizontal distance is posZ / p.
    float rowDistance = posZ / p;

    // calculate the real world step vector we have to add for each x (parallel to camera plane)
    // adding step by step avoids multiplications with a weight in the inner loop
    float floorStepX = rowDistance * (rayDirX1 - rayDirX0) / screen.width;
    float floorStepY = rowDistance * (rayDirY1 - rayDirY0) / screen.width;

    // real world coordinates of the leftmost column. This will be updated as we step to the right.
    float floorX = player.position.x + rowDistance * rayDirX0;
    float floorY = player.position.y + rowDistance * rayDirY0;

    for (int x = 0; x < screen.width; ++x)
    {
        // the cell coord is simply got from the integer parts of floorX and floorY
        int cellX = int(floorX);
        int cellY = int(floorY);

        // get the texture coordinate from the fractional part
        int tx = int(1 * 64 * (floorX - cellX)) & (64 - 1);
        int ty = int(1 * 64 * (floorY - cellY)) & (64 - 1);

        floorX += floorStepX;
        floorY += floorStepY;

        // Floor
        vec3 color = imageLoad(textures, ivec2(3 * 64 + tx, ty)).rgb / 2;
        imageStore(outputImage, ivec2(x, screen.height - y - 1), vec4(color, 1));

        // Ceiling
        color = imageLoad(textures, ivec2(6 * 64 + tx, ty)).rgb / 2;
        imageStore(outputImage, ivec2(x, y), vec4(color, 1));
    }
}
