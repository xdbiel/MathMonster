

#include <stdio.h>
#include <stdlib.h> 
#include <time.h>   

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h> 
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>


#include "cutscene.h"
#include "splash.h"
#include "menu.h"
#include "fase1.h"
#include "fase2.h"
#include "fase3.h"      
#include "tela_final.h" 
#include "gameover.h"


enum GameState {
    TELA_INICIAL,
    TELA_CUTSCENE,
    TELA_GAME_OVER,
    FASE_1,
    FASE_2,
    FASE_3,
    TELA_FINAL
};

int main() {
    srand(time(NULL));

    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return -1;
    }

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_init_primitives_addon();
    al_install_mouse();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(10);

    ALLEGRO_DISPLAY* display = al_create_display(1366, 768);
    ALLEGRO_BITMAP* imagem_cursor = al_load_bitmap("cursor.png"); 

 
    ALLEGRO_MONITOR_INFO info;
    if (al_get_monitor_info(0, &info)) {
        int largura_monitor = info.x2 - info.x1;
        int altura_monitor = info.y2 - info.y1;
        al_set_window_position(display, (largura_monitor - 1366) / 2, (altura_monitor - 768) / 2);
    }

    al_hide_mouse_cursor(display);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    ALLEGRO_FONT* font_principal = al_load_ttf_font("joystix.ttf", 28, 0);

    if (!display || !timer || !event_queue || !font_principal) {
        fprintf(stderr, "Erro ao inicializar recursos principais.\n");
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_start_timer(timer);


    enum GameState estado_atual_do_jogo = TELA_INICIAL;
    enum GameState checkpoint = FASE_1; 

    bool rodando = true;

    while (rodando) {

        switch (estado_atual_do_jogo) {

         
        case TELA_INICIAL: {
          
            int escolha_menu = run_menu_screen(display, event_queue, font_principal, timer, imagem_cursor);
            al_stop_samples(); 

            if (escolha_menu == 1) {
                estado_atual_do_jogo = TELA_CUTSCENE;
            }
            else { 
                rodando = false;
            }
            break;
        }

                      
        case TELA_CUTSCENE: {
            run_cutscene_screen(display, event_queue, font_principal);
            estado_atual_do_jogo = FASE_1;
            break;
        }

                        
        case TELA_GAME_OVER: {
            int escolha_gameover = run_game_over_screen(display, event_queue, timer, imagem_cursor);
            if (escolha_gameover == 1) {
                estado_atual_do_jogo = checkpoint; 
            }
            else {
                rodando = false; 
            }
            break;
        }

                          
        case FASE_1: {
            checkpoint = FASE_1;
            int resultado = run_fase1_screen(display);

            if (resultado == 1) { 
                estado_atual_do_jogo = FASE_2;
            }
            else if (resultado == 2) { 
                estado_atual_do_jogo = TELA_GAME_OVER;
            }
            else {
                rodando = false;
            }
            break;
        }

        case FASE_2: {
            checkpoint = FASE_2;
            int resultado = run_fase2_screen(display);

            if (resultado == 1) { 
                estado_atual_do_jogo = FASE_3; 
            }
            else if (resultado == 2) { 
                estado_atual_do_jogo = TELA_GAME_OVER;
            }
            else {
                rodando = false;
            }
            break;
        }

                   
        case FASE_3: {
            checkpoint = FASE_3;
          
            int resultado = run_fase3_screen(display);

            if (resultado == 1) { 
                estado_atual_do_jogo = TELA_FINAL; 
            }
            else if (resultado == 2) { 
                estado_atual_do_jogo = TELA_GAME_OVER;
            }
            else {
                rodando = false;
            }
            break;
        }

            
        case TELA_FINAL: {
      
            int escolha = run_tela_final(display);

            if (escolha == 1) {

                estado_atual_do_jogo = TELA_INICIAL;
                checkpoint = FASE_1; 
            }
            else {
                rodando = false; 
            }
            break;
        }
        }
    }

    if (font_principal) al_destroy_font(font_principal);
    if (display) al_destroy_display(display);
    if (timer) al_destroy_timer(timer);
    if (event_queue) al_destroy_event_queue(event_queue);
    if (imagem_cursor) al_destroy_bitmap(imagem_cursor);

    return 0;
}