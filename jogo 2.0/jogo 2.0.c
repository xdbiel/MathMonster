#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <stdio.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
enum Direcao { ESQUERDA, DIREITA };
int main() {

    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();

    ALLEGRO_DISPLAY* display = al_create_display(800, 600);
    al_set_window_position(display, 300, 300);
    al_set_window_title(display,"World traveler");
    ALLEGRO_BITMAP* sprite_andar_direita = al_load_bitmap("personagem_movimento_direita.png");
    ALLEGRO_BITMAP* icone = al_load_bitmap("logol.png");
    ALLEGRO_BITMAP* sprite_andar_esquerda = al_load_bitmap("personagem_movimento_esquerda.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("background.png");
    ALLEGRO_FONT* font1 = al_load_ttf_font("joystix.ttf", 28, 0);
    ALLEGRO_FONT* font2 = al_load_ttf_font("joystix.ttf", 18, 0);
  
        al_set_display_icon(display, icone); 
        // eu nao sei porque nao fucnona esse comando e funciona so o de cima para colocar icon, perguntar depois, segundo o que pesquisei e o que está na documentacao do allegro 5.2 o certo seria esse>>>> al_set_window_icon(display, icone);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    float frame = 0.02f;
    int pos_x = 200;
    int pos_y = 200;
    int current_frame_y = 0;
    enum Direcao direcao_personagem = DIREITA;

    while (true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
        else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
            pos_x += 3;
            direcao_personagem = DIREITA;
        }
        else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
            pos_x -= 3;
            direcao_personagem = ESQUERDA;
        }

        frame += 0.11111f;
        if (frame >= 6) {
            frame = 0;
        }
        al_clear_to_color(al_map_rgb(173, 216, 216));
        al_draw_bitmap(bg, 0, 0, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA / 2) - (al_get_font_line_height(font1) / 2), ALLEGRO_ALIGN_CENTER, "MOVIMENTACAO EM DESENVOLVIMENTO!!");
        al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA / 4) - (al_get_font_line_height(font2) / 2), ALLEGRO_ALIGN_CENTER, "TENTE USAR SETA PARA DIREITA E SETA PARA ESQUERDA!");
        if (direcao_personagem == DIREITA) {
            al_draw_bitmap_region(sprite_andar_direita, 32 * (int)frame, current_frame_y, 32, 32, pos_x, pos_y, 0);
        }
        else {
            al_draw_bitmap_region(sprite_andar_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
        }
        al_flip_display();
    }
    al_destroy_bitmap(bg);
    al_destroy_bitmap(icone);
    al_destroy_bitmap(sprite_andar_direita);
    al_destroy_bitmap(sprite_andar_esquerda);
    al_destroy_font(font1);
    al_destroy_font(font2);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
     
    return 0;
}