#ifndef GUI_H
#define GUI_H

#include <raylib.h>
#include <stdbool.h>


bool is_mouse_hovering(Rectangle);
bool is_button_clicked(Rectangle);
void DrawButton(Rectangle, const char *, void (*Callback)(void *), void *);
void DrawCenteredText(Rectangle, const char *);
void increase_spinner_value(void *);
void decrease_spinner_value(void *);
void DrawSpinner(Rectangle, int *);

#endif
