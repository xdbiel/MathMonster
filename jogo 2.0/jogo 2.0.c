#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <stdio.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600

int main() {

    al_init();
    al_init_font_addon();
    al_init_ttf_addon();

    ALLEGRO_DISPLAY* display = al_create_display(800, 600);
    al_set_window_position(display, 300, 300);
    al_set_window_title(display,"Jogo de plataforma");
    

    ALLEGRO_FONT* font = al_load_ttf_font("joystix.ttf", 28, 0);
    if (!font) {
        fprintf(stderr, "Erro ao carregar a fonte 'Roboto-Regular.ttf'.\n");
        al_destroy_display(display);
        return -1;
    }
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_start_timer(timer);

    while (true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        al_clear_to_color(al_map_rgb(0, 0, 0));
        al_draw_text(font, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA / 2) - (al_get_font_line_height(font) / 2), ALLEGRO_ALIGN_CENTER, "Jogo de plataforma");
        al_flip_display();
    }

    al_destroy_font(font);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}