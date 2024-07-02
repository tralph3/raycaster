#ifndef GUI_H
#define GUI_H

#include <raylib.h>
#include <stdbool.h>

typedef enum {
  SPINNER_LABEL_VALUE = 0,
  SPINNER_LABEL_PROVIDED,
} SpinnerLabelMode;


bool is_mouse_hovering(Rectangle);
bool is_button_clicked(Rectangle);
void DrawButton(Rectangle, const char *, void (*Callback)(void *), void *);
void DrawCenteredText(Rectangle, const char *);
void increase_spinner_value(void *);
void decrease_spinner_value(void *);
void DrawSpinner(Rectangle, int *, int, int, const char *, SpinnerLabelMode);

#endif
