#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "raylib.h"

void initWindow() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(0, 0, "Test");
    const int w = GetScreenWidth();
    const int h = GetScreenHeight();
    const int nw = w * .8f;
    const int nh = h * .8f;
    SetWindowSize(nw, nh);
    SetWindowPosition(w / 2 - nw / 2, h / 2 - nh / 2);
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

    model.materials[2].shader = getShader("../resources/shaders/base.glsl");
    Vector4 c = colorToVector(WHITE);
    Vector3 pos = {0};

    float time;

    while (!WindowShouldClose()) {
        time = GetTime();
        if (IsKeyPressed(KEY_F5)) {
            UnloadShader(model.materials[2].shader);
            model.materials[2].shader = getShader("../resources/shaders/base.glsl");
        }

        int t = GetShaderLocation(model.materials[2].shader, "_tex0");
        int t1 = GetShaderLocation(model.materials[2].shader, "_colAlbedo");
        int t2 = GetShaderLocation(model.materials[2].shader, "_time");
        BeginDrawing();
            ClearBackground(WHITE);
            BeginMode3D(camera);

                SetShaderValue(model.materials[2].shader, t2, &time, SHADER_UNIFORM_FLOAT);
                SetShaderValue(model.materials[2].shader, t1, &c, SHADER_UNIFORM_VEC4);
                SetShaderValueTexture(model.materials[2].shader, t, texture);
                DrawModelEx(model, pos, (Vector3){1, 0, 0}, -90, (Vector3){100, 100, 100}, WHITE);
                DrawGrid(10, 1);
            EndMode3D();
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
