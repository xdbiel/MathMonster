#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/keyboard.h>
#include <stdio.h>
#include <stdbool.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define GRAVIDADE 0.5f
#define PULO_FORCA -10.0f
#define VELOCIDADE 3

enum Direcao { DIREITA, ESQUERDA };

typedef struct {
    float x, y;
    float largura, altura;
} Plataforma;

int main() {
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_init_primitives_addon();

    ALLEGRO_DISPLAY* display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    al_set_window_title(display, "World traveler ");

    ALLEGRO_BITMAP* bg = al_load_bitmap("9259351.jpg");
    ALLEGRO_BITMAP* sprite_andar_direita = al_load_bitmap("personagem_movimento_direita.png");
    ALLEGRO_BITMAP* sprite_andar_esquerda = al_load_bitmap("personagem_movimento_esquerda.png");
    ALLEGRO_FONT* font1 = al_load_ttf_font("joystix.ttf", 20, 0);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    bool tecla[ALLEGRO_KEY_MAX] = { false };
    bool sair = false;

    float pos_x = 200, pos_y = 500;
    float vel_y = 0;
    bool no_chao = false;
    enum Direcao direcao_personagem = DIREITA;
    float frame = 0.02f;


    Plataforma plataformas[5] = {
        { 0, 580, 800, 20 },     
        { 150, 480, 120, 15 },
        { 320, 380, 120, 15 },
        { 500, 300, 120, 15 },
        { 650, 220, 120, 15 }
    };

    while (!sair) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            sair = true;
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN)
            tecla[event.keyboard.keycode] = true;
        else if (event.type == ALLEGRO_EVENT_KEY_UP)
            tecla[event.keyboard.keycode] = false;
        else if (event.type == ALLEGRO_EVENT_TIMER) {

           
            if (tecla[ALLEGRO_KEY_RIGHT]) {
                pos_x += VELOCIDADE;
                direcao_personagem = DIREITA;
            }
            if (tecla[ALLEGRO_KEY_LEFT]) {
                pos_x -= VELOCIDADE;
                direcao_personagem = ESQUERDA;
            }

            
            if (tecla[ALLEGRO_KEY_UP] && no_chao) {
                vel_y = PULO_FORCA;
                no_chao = false;
            }

           
            vel_y += GRAVIDADE;
            pos_y += vel_y;
            no_chao = false;

            
            for (int i = 0; i < 5; i++) {
                if (pos_x + 32 > plataformas[i].x &&
                    pos_x < plataformas[i].x + plataformas[i].largura &&
                    pos_y + 32 > plataformas[i].y &&
                    pos_y + 32 < plataformas[i].y + plataformas[i].altura + 10 &&
                    vel_y >= 0)
                {
                    pos_y = plataformas[i].y - 32; 
                    vel_y = 0;
                    no_chao = true;
                }
            }

            
            frame += 0.2f;
            if (frame >= 6) frame = 0;

           
            al_clear_to_color(al_map_rgb(150, 200, 255));
            al_draw_bitmap(bg, 0, 0, 0);

            
            for (int i = 0; i < 5; i++) {
                al_draw_filled_rectangle(plataformas[i].x, plataformas[i].y,
                    plataformas[i].x + plataformas[i].largura, plataformas[i].y + plataformas[i].altura,
                    al_map_rgb(100, 60, 30));
            }

            
            if (direcao_personagem == DIREITA)
                al_draw_bitmap_region(sprite_andar_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
            else
                al_draw_bitmap_region(sprite_andar_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);

            al_draw_text(font1, al_map_rgb(255, 255, 255), 10, 10, 0, "Use setas para andar e pular");
            al_flip_display();
        }
    }

    al_destroy_bitmap(bg);
    al_destroy_bitmap(sprite_andar_direita);
    al_destroy_bitmap(sprite_andar_esquerda);
    al_destroy_font(font1);
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);

    return 0;
}
