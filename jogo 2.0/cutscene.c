#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h> // Pode ser �til para caixas de texto
#include <stdio.h>

#include "cutscene.h"

void run_cutscene_screen(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font) {

    printf("Entrei na cutscene!\n");
    al_clear_to_color(al_map_rgb(20, 0, 0)); // Um fundo escuro tempor�rio
    al_flip_display();
    al_rest(2.0); // Pausa por 2 segundos

    // A l�gica das imagens e do texto aparecer� aqui no futuro
}