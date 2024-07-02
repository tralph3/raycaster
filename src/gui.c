#include "gui.h"
#include <raylib.h>
#include <stdio.h>
#include <raymath.h>

GUISettings gui_settings = {0};

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
  Color background = RAYWHITE;
  if (is_mouse_hovering(rec) && !IsMouseButtonDown(0))
    background = ColorBrightness(background, -0.1f);
  DrawRectangleRec(rec, background);
  DrawCenteredText(rec, label);
  if (is_button_clicked(rec))
    callback(callback_args);
}

void DrawCenteredText(Rectangle rec, const char *label) {
  float font_size = 24;
  float font_spacing = 2;
  Vector2 text_size = MeasureTextEx(gui_settings.font, label, font_size, font_spacing);
  Vector2 position = {
    .x = (rec.x + (rec.width / 2.f)) - (text_size.x / 2.f),
    .y = (rec.y + (rec.height / 2.f)) - (text_size.y / 2.f),
  };
  DrawTextEx(gui_settings.font, label, position, font_size, font_spacing, BLACK);
}


void increase_spinner_value(void *value) { *(int*)value += 1; }
void decrease_spinner_value(void *value) { *(int*)value -= 1; }

void DrawSpinnerValue(Rectangle rec, int *value, int lower_limit, int upper_limit) {
    char value_label[20];
    sprintf(value_label, "%d", *value);
    DrawSpinner(rec, value, lower_limit, upper_limit, value_label);
}

void DrawSpinner(Rectangle rec, int *value, int lower_limit, int upper_limit, const char *label) {
  Rectangle decrese_button_rec = {
    .x = rec.x,
    .y = rec.y,
    .width = 25,
    .height = rec.height,
  };
  Rectangle increase_button_rec = {
    .x = rec.x + rec.width - 25,
    .y = rec.y,
    .width = 25,
    .height = rec.height,
  };

  DrawRectangleRec(rec, RAYWHITE);
  DrawCenteredText(rec, label);
  DrawButton(decrese_button_rec, "<", decrease_spinner_value, value);
  DrawButton(increase_button_rec, ">", increase_spinner_value, value);
  *value = Clamp(*value, lower_limit, upper_limit);
}
