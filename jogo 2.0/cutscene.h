#ifndef CUTSCENE_H
#define CUTSCENE_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

// A função da nossa cutscene vai precisar do display, da fila de eventos e de uma fonte
void run_cutscene_screen(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font);

#endif