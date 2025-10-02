#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "menu.h"

static const char* options[] = { "JOGAR", "OPÇÕES", "SAIR" };
static const int OPTIONS_COUNT = 3;

MenuOption menu_loop(ALLEGRO_DISPLAY* display) {
    
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0); 
    if (!queue || !timer) {
        fprintf(stderr, "Erro ao criar event queue ou timer\n");
        return MENU_EXIT;
    }

    ALLEGRO_FONT* font = al_load_ttf_font("assets/OpenSans-Regular.ttf", 36, 0);
    if (!font) {
        fprintf(stderr, "Não encontrou a fonte em assets/ (verifique o caminho)\n");
        al_destroy_timer(timer);
        al_destroy_event_queue(queue);
        return MENU_EXIT;
    }

    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_mouse_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));

    al_start_timer(timer);

    int selected = 0;        
    bool running = true;
    bool redraw = true;
    ALLEGRO_EVENT ev;

    while (running) {
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
        
            redraw = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            
            if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                selected = (selected + 1) % OPTIONS_COUNT;
            }
            else if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
                selected = (selected - 1 + OPTIONS_COUNT) % OPTIONS_COUNT;
            }
            else if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER || ev.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER) {
               
                switch (selected) {
                case 0: al_destroy_font(font); al_destroy_timer(timer); al_destroy_event_queue(queue); return MENU_PLAY;
                case 1: /* placeholder para opções */ break;
                case 2: al_destroy_font(font); al_destroy_timer(timer); al_destroy_event_queue(queue); return MENU_EXIT;
                }
            }
            else if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                running = false;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
           
            float mx = ev.mouse.x;
            float my = ev.mouse.y;
            int w = al_get_display_width(display);
            int h = al_get_display_height(display);
            int base_y = h / 2 - 40; 
            for (int i = 0; i < OPTIONS_COUNT; i++) {
                const char* text = options[i];
                int text_w = al_get_text_width(font, text);
                int text_h = al_get_font_line_height(font);
                int center_x = w / 2;
                int item_y = base_y + i * 60; 

                int left = center_x - text_w / 2;
                int right = center_x + text_w / 2;
                int top = item_y;
                int bottom = item_y + text_h;
                if (mx >= left && mx <= right && my >= top && my <= bottom) {
                    if (i == 0) { al_destroy_font(font); al_destroy_timer(timer); al_destroy_event_queue(queue); return MENU_PLAY; }
                    if (i == 1) { /* abrir opções */ }
                    if (i == 2) { al_destroy_font(font); al_destroy_timer(timer); al_destroy_event_queue(queue); return MENU_EXIT; }
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            // desenha menu
            al_clear_to_color(al_map_rgb(15, 15, 30));

            // título
            ALLEGRO_FONT* title_font = font; 
            al_draw_text(title_font, al_map_rgb(255, 255, 255),
                al_get_display_width(display) / 2, 80,
                ALLEGRO_ALIGN_CENTER, "MEU JOGO");

            // opções
            int base_y = al_get_display_height(display) / 2 - 40;
            for (int i = 0; i < OPTIONS_COUNT; i++) {
                const char* text = options[i];
                ALLEGRO_COLOR color = (i == selected) ? al_map_rgb(255, 220, 0) : al_map_rgb(230, 230, 230);
                al_draw_text(font, color,
                    al_get_display_width(display) / 2,
                    base_y + i * 60,
                    ALLEGRO_ALIGN_CENTER, text);
            }

            al_flip_display();
        }
    }

    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    return MENU_EXIT;

}
