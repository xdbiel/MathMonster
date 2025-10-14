#ifndef MENU_H
#define MENU_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

// Declaração da nossa função de menu
int run_menu_screen(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font, ALLEGRO_TIMER* timer);

#endif