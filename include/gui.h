#ifndef GUI_H
#define GUI_H

#include <raylib.h>
#include <stdbool.h>

typedef struct {
  Font font;
} GUISettings;

extern GUISettings gui_settings;

bool is_mouse_hovering(Rectangle);
bool is_button_clicked(Rectangle);
void DrawButton(Rectangle, const char *, void (*Callback)(void *), void *);
void DrawCenteredText(Rectangle, const char *);
void increase_spinner_value(void *);
void decrease_spinner_value(void *);
void DrawSpinnerValue(Rectangle, int *, int, int);
void DrawSpinner(Rectangle, int *, int, int, const char *);

#endif
