#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

#include "gameover.h"
#define LARGURA_TELA 800
#define ALTURA_TELA 600

struct BotaoGameOver {
    float x, y, largura, altura;
    ALLEGRO_BITMAP* img_normal;
    ALLEGRO_BITMAP* img_selecionado;
    bool selecionado;
};

int run_game_over_screen(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_TIMER* timer, ALLEGRO_BITMAP* cursor_img) {


    ALLEGRO_BITMAP* background = al_load_bitmap("gameoverscreen.png");
    ALLEGRO_BITMAP* btn_normal = al_load_bitmap("botao_tentenovamente_normal.png");
    ALLEGRO_BITMAP* btn_selecionado = al_load_bitmap("botao_tentenovamente_selecionado.png");
    ALLEGRO_SAMPLE* musica_gameover = al_load_sample("gameoverscreen_music.ogg");

  

    struct BotaoGameOver botao_tentar_novamente;
    botao_tentar_novamente.largura = 180;
    botao_tentar_novamente.altura = 40;
    botao_tentar_novamente.x = 310;
    botao_tentar_novamente.y = 500;
    botao_tentar_novamente.img_normal = btn_normal;
    botao_tentar_novamente.img_selecionado = btn_selecionado;
    botao_tentar_novamente.selecionado = false;


    al_play_sample(musica_gameover, 0.7, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    al_flush_event_queue(event_queue);
    bool rodando = true;
    bool redesenhar = true;
    int resultado = 1;
    int mouse_x = 0, mouse_y = 0;

    while (rodando) {
        ALLEGRO_EVENT evento;
        al_wait_for_event(event_queue, &evento);

        if (evento.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
        }
        else if (evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            resultado = 0;
            return 0;
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_AXES) {
          
            mouse_x = evento.mouse.x; 
            mouse_y = evento.mouse.y; 
            if (evento.mouse.x >= botao_tentar_novamente.x && evento.mouse.x <= botao_tentar_novamente.x + botao_tentar_novamente.largura &&
                evento.mouse.y >= botao_tentar_novamente.y && evento.mouse.y <= botao_tentar_novamente.y + botao_tentar_novamente.altura) {
                botao_tentar_novamente.selecionado = true;
            }
            else {
                botao_tentar_novamente.selecionado = false;
            }
        }
        else if (evento.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
           
            if (botao_tentar_novamente.selecionado) {
                resultado = 1;
                rodando = false;
            }
        }

        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = false;
            al_draw_bitmap(background, 0, 0, 0);

            if (botao_tentar_novamente.selecionado) {
                al_draw_bitmap(botao_tentar_novamente.img_selecionado, botao_tentar_novamente.x, botao_tentar_novamente.y, 0);
            }
            else {
                al_draw_bitmap(botao_tentar_novamente.img_normal, botao_tentar_novamente.x, botao_tentar_novamente.y, 0);
            }
            al_draw_bitmap(cursor_img, mouse_x, mouse_y, 0);
            al_flip_display();
        }
    }


    al_stop_samples();
    al_destroy_bitmap(background);
    al_destroy_bitmap(btn_normal);
    al_destroy_bitmap(btn_selecionado);
    al_destroy_sample(musica_gameover);

    return resultado;
}