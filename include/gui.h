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
bool GUIButton(Rectangle, const char *);
void DrawCenteredText(Rectangle, const char *);
void increase_spinner_value(void *);
void decrease_spinner_value(void *);
void GUISpinnerValue(Rectangle, int *, int, int);
void GUISpinner(Rectangle, int *, int, int, const char *);

#endif
