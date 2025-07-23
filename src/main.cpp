#include <raylib.h>
#include <raymath.h>
#include <string>
#include <ferox.h>

#define FEROX_RAYLIB_IMPLEMENTATION
#include <ferox_raylib.h>

#include <vector>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

const int WIDTH = 850;
const int HEIGHT = 450;

frWorld* world;
std::vector<frBody*> boxes;
frBody* box;
frBody* ground;

void UpdateDrawFrame();

int main(void)
{
    InitWindow(WIDTH, HEIGHT, "Ferox + Raylib Demo");

    world = frCreateWorld({0, 9.8f}, 1.f);

    frShape* boxShape = frCreateRectangle({ 1.0f, 0.5f, 0.0f }, 1.0f, 1.0f);
    box = frCreateBody(FR_BODY_DYNAMIC, frVector2PixelsToUnits({100, 100}));
    frSetBodyShape(box, boxShape);
    frAddBodyToWorld(world, box);

    ground = frCreateBody(FR_BODY_STATIC, frVector2PixelsToUnits({WIDTH / 2, HEIGHT - 18}));
    frShape* groundShape = frCreateRectangle({ 0.0f, 0.8f, 0.0f }, frPixelsToUnits(WIDTH), frPixelsToUnits(20));
    frSetBodyShape(ground, groundShape);
	frSetShapeFriction(groundShape, 0.5f);
    frAddBodyToWorld(world, ground);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateDrawFrame();
    }
#endif

    CloseWindow();
    return 0;
}

void UpdateDrawFrame()
{
    const float dt = 1.0f / 60.0f;
    frStepWorld(world, dt);
    frSetBodyAngle(box, 0);

    BeginDrawing();
    ClearBackground(RAYWHITE);

    frDrawGrid({0, 0, WIDTH, HEIGHT}, 1.0f, 0.25f, LIGHTGRAY);

    frDrawBodyAABB(ground, 1.0f, DARKGRAY);
    frDrawBodyLines(box, 2.0f, RED);

    for (frBody* b : boxes)
        frDrawBodyLines(b, 1.0f, BLUE);

    DrawText("Click to drop boxes", 10, 10, 20, BLACK);
    frVector2 v = frGetBodyVelocity(box);
    DrawText(TextFormat("Box vel: %.2f, %.2f", v.x, v.y), 10, 40, 20, DARKGRAY);

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = { (float)GetMouseX(), (float)GetMouseY() };
        frShape* newShape = frCreateRectangle({ 1.0f, 0.5f, 0.0f }, 1.0f, 1.0f);
        frBody* newBox = frCreateBody(FR_BODY_DYNAMIC, frVector2PixelsToUnits({mouse.x, mouse.y}));
        frSetBodyShape(newBox, newShape);
        frAddBodyToWorld(world, newBox);

        boxes.push_back(newBox);
    }

    if (IsKeyDown(KEY_A))
    {
        frApplyForceToBody(box, {frGetBodyPosition(box).x, frGetBodyPosition(box).y}, {-10, 0});
    }
    if (IsKeyDown(KEY_D))
    {
        frApplyForceToBody(box, {frGetBodyPosition(box).x, frGetBodyPosition(box).y}, {10, 0});
    }

    if (IsKeyPressed(KEY_SPACE))
    {
        frApplyForceToBody(box, {frGetBodyPosition(box).x, frGetBodyPosition(box).y}, {0, -500});
    }

	std::string info_str = "Bodies: " + std::to_string(frGetBodyCountInWorld(world));
	DrawText(info_str.c_str(), 10, 60, 20, BLACK);
	DrawFPS(10, 80);

    EndDrawing();
}
