#include "raylib.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#define GRID_WIDTH 30
#define GRID_HEIGHT 45
#define SNAKE_LENGTH GRID_WIDTH *GRID_HEIGHT + 1
#define GRID_SCALE 20
#define WINDOW_WIDTH GRID_WIDTH *GRID_SCALE
#define WINDOW_HEIGHT GRID_HEIGHT *GRID_SCALE
#define SCREEN_TITLE "Cnake"
#define TARGET_FPS 60

typedef struct {
  Vector2 *body;
  float movementTimer;
  float movementRate;
  int length;
  bool isAlive;
} snake_t;

typedef struct {
  Vector2 pos;
  float respawnTimer;
  float respawnRate;
} fruit_t;

typedef struct {
  int score;
  Vector2 *movementBuffer;
  Vector2 *currentMovement;
  snake_t *snake;
  fruit_t *fruit;
} state_t;

snake_t *initSnake() {
  snake_t *snake = malloc(sizeof(snake_t));
  Vector2 *body = calloc(SNAKE_LENGTH, sizeof(Vector2));

  snake->body = body;
  snake->body[0].x = floorf(((float)GRID_WIDTH / 2) - 1.0f);
  snake->body[0].y = floorf(((float)GRID_HEIGHT / 2) - 1.0f);
  snake->length = 1;
  snake->movementTimer = 0.0f;
  snake->movementRate = 0.4f;
  snake->isAlive = true;

  return snake;
}

void moveFruit(fruit_t *fruit) {
  int x = GetRandomValue(1, GRID_WIDTH - 1);
  int y = GetRandomValue(1, GRID_HEIGHT - 1);

  fruit->pos.x = x;
  fruit->pos.y = y;
}

fruit_t *initFruit() {
  fruit_t *fruit = malloc(sizeof(fruit_t));

  moveFruit(fruit);
  fruit->respawnTimer = 0.0f;
  fruit->respawnRate = 15.0f;

  return fruit;
}

state_t *initState() {
  state_t *state = malloc(sizeof(state_t));
  state->score = 0;
  state->movementBuffer = malloc(sizeof(Vector2));
  state->currentMovement = malloc(sizeof(Vector2));
  state->movementBuffer->x = 0;
  state->movementBuffer->y = 0;
  state->snake = initSnake();
  state->fruit = initFruit();

  return state;
}

void resetState(state_t *state) {
  free(state->snake->body);
  free(state->snake);
  free(state->fruit);
  free(state->movementBuffer);
  free(state->currentMovement);

  state->snake = initSnake();
  state->fruit = initFruit();
  state->score = 0;
  state->movementBuffer = malloc(sizeof(Vector2));
  state->movementBuffer->x = 0;
  state->movementBuffer->y = 0;
  state->currentMovement = malloc(sizeof(Vector2));
}

void update(state_t *state) {
  if (IsKeyPressed(KEY_R)) {
    resetState(state);
  }
  if (!state->snake->isAlive) {
    return;
  }
  if (state->snake->length > 1) {
    for (int i = 1; i < state->snake->length; i++) {
      if (state->snake->body[0].x == state->snake->body[i].x &&
          state->snake->body[0].y == state->snake->body[i].y) {
        resetState(state);
      }
    }
  }

  float delta = GetFrameTime();
  state->snake->movementTimer += delta;
  state->fruit->respawnTimer += delta;

  if (state->fruit->respawnTimer >= state->fruit->respawnRate) {
    state->fruit->respawnTimer = 0.0f;
    moveFruit(state->fruit);
  }

  switch (GetKeyPressed()) {
  case KEY_W:
    if (state->currentMovement->y == 1 && state->snake->length > 1) {
      break;
    }
    state->movementBuffer->x = 0;
    state->movementBuffer->y = -1;
    break;
  case KEY_A:
    if (state->currentMovement->x == 1 && state->snake->length > 1) {
      break;
    }
    state->movementBuffer->x = -1;
    state->movementBuffer->y = 0;
    break;
  case KEY_S:
    if (state->currentMovement->y == -1 && state->snake->length > 1) {
      break;
    }
    state->movementBuffer->x = 0;
    state->movementBuffer->y = 1;
    break;
  case KEY_D:
    if (state->currentMovement->x == -1 && state->snake->length > 1) {
      break;
    }
    state->movementBuffer->x = 1;
    state->movementBuffer->y = 0;
    break;
  }

  if (state->snake->movementTimer >= state->snake->movementRate) {
    state->snake->movementTimer = 0.0f;

    if (state->fruit->pos.x == state->snake->body[0].x &&
        state->fruit->pos.y == state->snake->body[0].y) {
      moveFruit(state->fruit);
      state->snake->length += 1;
      state->score += 1;
      if (state->snake->movementRate > 0.05f)
        state->snake->movementRate -= 0.05f;
    }

    for (int i = state->snake->length - 1; i > 0; i--) {
      state->snake->body[i].x = state->snake->body[i - 1].x;
      state->snake->body[i].y = state->snake->body[i - 1].y;
    }

    if (state->snake->body[0].y < 0) {
      state->snake->body[0].y = GRID_HEIGHT - 1;
    } else if (state->snake->body[0].y > GRID_HEIGHT - 1) {
      state->snake->body[0].y = 0;
    } else if (state->snake->body[0].x < 0) {
      state->snake->body[0].x = GRID_WIDTH - 1;
    } else if (state->snake->body[0].x > GRID_WIDTH - 1) {
      state->snake->body[0].x = 0;
    } else {
      state->currentMovement->x = state->movementBuffer->x;
      state->currentMovement->y = state->movementBuffer->y;
      state->snake->body[0].x += state->movementBuffer->x;
      state->snake->body[0].y += state->movementBuffer->y;
    }
  }
}

void draw(state_t *state) {
  DrawText(TextFormat("Score: %d", state->score),
           WINDOW_WIDTH / 2 - (GRID_SCALE * 4), GRID_SCALE, GRID_SCALE * 2,
           RAYWHITE);

  DrawRectangle(state->fruit->pos.x * GRID_SCALE,
                state->fruit->pos.y * GRID_SCALE, GRID_SCALE, GRID_SCALE,
                GREEN);
  DrawRectangle(state->snake->body[0].x * GRID_SCALE,
                state->snake->body[0].y * GRID_SCALE, GRID_SCALE, GRID_SCALE,
                MAROON);
  for (int i = 1; i < state->snake->length; i++) {
    DrawRectangle(state->snake->body[i].x * GRID_SCALE,
                  state->snake->body[i].y * GRID_SCALE, GRID_SCALE, GRID_SCALE,
                  RED);
  }
}

int main(void) {
  InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, SCREEN_TITLE);
  SetTargetFPS(TARGET_FPS);

  state_t *state = initState();

  while (!WindowShouldClose()) {

    update(state);

    BeginDrawing();
    ClearBackground(BLACK);

    draw(state);

    EndDrawing();
  }

  CloseWindow();

  return 0;
}
