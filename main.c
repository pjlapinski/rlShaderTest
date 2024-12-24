#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

#define ROBERTS

#ifdef ROBERTS
#define SHADER "../resources/shaders/roberts.glsl"
#elif defined CANNY
#define SHADER "../resources/shaders/canny.glsl"
#else
#define SHADER "../resources/shaders/base.glsl"
#endif

Vector2 screenSize() {
    return (Vector2){1600, 900};
}

void initWindow() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(0, 0, "Test");
    const Vector2 ss = screenSize();
    SetWindowSize(ss.x, ss.y);
    SetWindowPosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
}

Shader getShader(const char *fileName) {
    char *preamble = LoadFileText("../resources/shaders/preamble.glsl");
    char *shd = LoadFileText(fileName);
    int shdLength = strlen(shd);
    int preambleLen = strlen(preamble);

    int idx = TextFindIndex(shd, "/* Frag */");
    char *vert = malloc(sizeof(char) * (idx + preambleLen + 2));
    char *frag = malloc(sizeof(char) * (shdLength - idx + preambleLen + 2));
    memcpy(vert, preamble, preambleLen);
    memcpy(frag, preamble, preambleLen);
    vert[preambleLen] = '\n';
    frag[preambleLen] = '\n';
    memcpy(vert + preambleLen + 1, shd, idx);
    vert[preambleLen + idx + 1] = '\0';
    memcpy(frag + preambleLen + 1, shd + idx, shdLength - idx + 1);
    Shader shader = LoadShaderFromMemory(vert, frag);
    free(vert);
    free(frag);
    free(shd);
    free(preamble);
    return shader;
}

Vector4 colorToVector(Color color) {
    return (Vector4){
        .x = color.r / 255.f,
        .y = color.g / 255.f,
        .z = color.b / 255.f,
        .w = color.a / 255.f,
    };
}

void mainLoop() {
    Camera camera = {0};
    camera.position = (Vector3){10, 10, 10};
    camera.up = (Vector3){0, 1, 0};
    camera.fovy = 45;
    camera.projection = CAMERA_PERSPECTIVE;

    Model model = LoadModel("../resources/Astronaut.glb");
    Texture2D texture = LoadTexture("../resources/textures/Atlas.png");
    model.materials[2].maps[MATERIAL_MAP_ALBEDO].texture = texture;

    Shader shader = getShader(SHADER);
    Vector3 pos = {0};

    float time;
#ifdef ROBERTS
    float outlineThickness = 1.f;
    Vector2 resolution;
    Vector4 edgeColor = colorToVector(BLACK);
#endif

    const Vector2 ss = screenSize();
    RenderTexture2D target = LoadRenderTexture(ss.x, ss.y);

    while (!WindowShouldClose()) {
        time = GetTime();
#ifdef ROBERTS
        resolution = (Vector2) {
            GetScreenWidth(),
            GetScreenHeight()
        };
#endif
        if (IsKeyPressed(KEY_F5)) {
            UnloadShader(shader);
            shader = getShader(SHADER);
        }

        const int t = GetShaderLocation(shader, "_tex0");
        const int t2 = GetShaderLocation(shader, "_time");
#ifdef ROBERTS
        const int res = GetShaderLocation(shader, "_resolution");
        const int thick = GetShaderLocation(shader, "_outlineThickness");
        const int col = GetShaderLocation(shader, "_edgeColor");
#endif

        UpdateCamera(&camera, CAMERA_ORBITAL);
        BeginTextureMode(target);
            ClearBackground(WHITE);
            BeginMode3D(camera);
                DrawModelEx(model, pos, (Vector3){1, 0, 0}, -90, (Vector3){100, 100, 100}, WHITE);
            EndMode3D();
        EndTextureMode();

        BeginDrawing();
            BeginShaderMode(shader);
                SetShaderValue(shader, t2, &time, SHADER_UNIFORM_FLOAT);
                SetShaderValueTexture(shader, t, target.texture);
#ifdef ROBERTS
                SetShaderValue(shader, res, &resolution, SHADER_UNIFORM_VEC2);
                SetShaderValue(shader, thick, &outlineThickness, SHADER_UNIFORM_FLOAT);
                SetShaderValue(shader, col, &edgeColor, SHADER_UNIFORM_VEC4);
#endif
                DrawTextureRec(target.texture, (Rectangle){0,0,(float)target.texture.width, (float)-target.texture.height}, (Vector2){0, 0}, WHITE);
            EndShaderMode();
        EndDrawing();
    }

    UnloadShader(model.materials[2].shader);
    UnloadTexture(texture);
    UnloadModel(model);
}

int main() {
    initWindow();
    mainLoop();
    CloseWindow();
}
