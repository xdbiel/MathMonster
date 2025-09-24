#include <stdio.h>
#include <stdbool.h> 
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>


const int LARGURA_TELA = 1650;
const int ALTURA_TELA = 600;
const int FPS = 60;
const int LARGURA_JOGADOR = 40;
const int ALTURA_JOGADOR = 40;

const float GRAVIDADE = 0.5f;
const float FORCA_PULO = -12.0f;
const float VELOCIDADE_JOGADOR = 5.0f;

typedef struct {
    float x, y;        
    float vel_x, vel_y; 
    bool esta_no_chao; 
} Jogador;



int main() {
  
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_DISPLAY* display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();


    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());


    Jogador jogador = { LARGURA_TELA / 2.0f, ALTURA_TELA / 2.0f, 0, 0, false };

    bool rodando = true;
    bool redesenhar = true;
    bool teclas[ALLEGRO_KEY_MAX] = { false }; 

    al_start_timer(timer);


    while (rodando) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

   
        if (ev.type == ALLEGRO_EVENT_TIMER) {
        
            if (teclas[ALLEGRO_KEY_LEFT] && jogador.x > 0) {
                jogador.x -= VELOCIDADE_JOGADOR;
            }
            if (teclas[ALLEGRO_KEY_RIGHT] && jogador.x < LARGURA_TELA - LARGURA_JOGADOR) {
                jogador.x += VELOCIDADE_JOGADOR;
            }

            
            if (teclas[ALLEGRO_KEY_UP] && jogador.esta_no_chao) {
                jogador.vel_y = FORCA_PULO; 
                jogador.esta_no_chao = false;
            }

            
            jogador.vel_y += GRAVIDADE;

           
            jogador.y += jogador.vel_y;

          
            if (jogador.y >= ALTURA_TELA - ALTURA_JOGADOR) {
                jogador.y = ALTURA_TELA - ALTURA_JOGADOR; 
                jogador.vel_y = 0;                        
                jogador.esta_no_chao = true;              
            }

            redesenhar = true;
        }
 
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            rodando = false;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            teclas[ev.keyboard.keycode] = true;
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            teclas[ev.keyboard.keycode] = false;
        }

        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = false;

            al_clear_to_color(al_map_rgb(25, 25, 50));

            al_draw_filled_rectangle(jogador.x, jogador.y, jogador.x + LARGURA_JOGADOR, jogador.y + ALTURA_JOGADOR, al_map_rgb(255, 255, 255));

            al_flip_display();
        }
    }

    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);

    return 0;
}