#ifndef MENU_H
#define MENU_H

#include <allegro5/allegro.h>

typedef enum {
    MENU_PLAY,
    MENU_OPTIONS,
    MENU_EXIT
} MenuOption;

MenuOption menu_loop(ALLEGRO_DISPLAY* display);

#endif
