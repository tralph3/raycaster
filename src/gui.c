#include "gui.h"
#include <raylib.h>
#include <stdio.h>
#include <raymath.h>

bool is_mouse_hovering(Rectangle rec) {
  Vector2 mouse_pos = GetMousePosition();
  return mouse_pos.x >= rec.x &&
         mouse_pos.x <= rec.x + rec.width &&
         mouse_pos.y >= rec.y &&
         mouse_pos.y <= rec.y + rec.height;
}

bool is_button_clicked(Rectangle rec) {
  return is_mouse_hovering(rec) && IsMouseButtonPressed(0);
}

void DrawButton(Rectangle rec, const char *label, void (*callback)(void *), void *callback_args) {
  Color background = WHITE;
  if (is_mouse_hovering(rec) && !IsMouseButtonDown(0))
    background = ColorBrightness(background, -0.1f);
  DrawRectangleRec(rec, background);
  DrawCenteredText(rec, label);
  if (is_button_clicked(rec))
    callback(callback_args);
}

void DrawCenteredText(Rectangle rec, const char *label) {
  int text_width = MeasureText(label, 12);
  float text_x = (rec.x + (rec.width / 2.f)) - (text_width / 2.f);
  DrawText(label, text_x, rec.y + (rec.height / 2.f) - 5, 10, BLACK);
}


void increase_spinner_value(void *value) { *(int*)value += 1; }
void decrease_spinner_value(void *value) { *(int*)value -= 1; }

void DrawSpinner(Rectangle rec, int *value, int lower_limit, int upper_limit, const char *provided_label, SpinnerLabelMode mode) {
  const char *label;
  if (mode == SPINNER_LABEL_VALUE) {
    char value_label[20];
    sprintf(value_label, "%d", *value);
    label = value_label;
  } else if (mode == SPINNER_LABEL_PROVIDED) {
    label = provided_label;
  }

  Rectangle decrese_button_rec = {
    .x = rec.x,
    .y = rec.y,
    .width = 20,
    .height = rec.height,
  };
  Rectangle increase_button_rec = {
    .x = rec.x + rec.width - 10,
    .y = rec.y,
    .width = 20,
    .height = rec.height,
  };

  DrawRectangleRec(rec, WHITE);
  DrawButton(decrese_button_rec, "<", decrease_spinner_value, value);
  DrawCenteredText(rec, label);
  DrawButton(increase_button_rec, ">", increase_spinner_value, value);

  *value = Clamp(*value, lower_limit, upper_limit);
}
