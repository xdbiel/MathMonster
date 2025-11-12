
#include <stdio.h>


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
#include <stdlib.h> 
#include <time.h>   
#include "gameover.h"
// variavel contendo todos os estados do jogo(irei adicionar a fase 3 futuramente)
enum GameState {
    TELA_SPLASH,    
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



    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_init_primitives_addon();
    al_install_mouse(); 
    al_install_audio();
    al_init_acodec_addon(); 
    al_reserve_samples(10); 


    ALLEGRO_DISPLAY* display = al_create_display(800, 600);
    ALLEGRO_BITMAP* imagem_cursor = al_load_bitmap("cursor.png");

  

    // esconde o cursor do sistema operacional
    al_hide_mouse_cursor(display);


    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

    ALLEGRO_FONT* font_principal = al_load_ttf_font("joystix.ttf", 28, 0);

    //verificacao de erro (usada pois estava com alguns erro nao identificados)
    if (!display || !timer || !event_queue || !font_principal) {
        fprintf(stderr, "Erro ao inicializar um dos recursos principais do Allegro.\n");
        return -1;
    }

    // Registro das fontes de evento que o jogo inteiro vai usar
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_start_timer(timer);

    
    

    enum GameState estado_atual_do_jogo =   TELA_SPLASH; // variavel que controla qual tela estamos
    bool rodando = true;
    int mouse_x = 0, mouse_y = 0;
    while (rodando) {


        //usei o switch para decidir com qual funcao vou ''trabalahar'' qual funcao vai ser exibida na tela
        switch (estado_atual_do_jogo) {

        case TELA_SPLASH:
            run_splash_screen(display); 
			estado_atual_do_jogo = TELA_INICIAL; // mudar para a tela inicial apos a splash
            break;

            case TELA_INICIAL: {
                int escolha_menu = run_menu_screen(display, event_queue, font_principal, timer, imagem_cursor);
                al_stop_samples();
                if (escolha_menu == 1) { estado_atual_do_jogo = TELA_CUTSCENE; }
                else { // 2 = sair
                   rodando = false; 
                }
                break;
            }

            case TELA_CUTSCENE:
                
                run_cutscene_screen(display, event_queue, font_principal);

                // Quando a cutscene terminar, o jogo avança para a Fase 1
                estado_atual_do_jogo = FASE_1;
                break;


            case TELA_GAME_OVER: {
                int escolha_gameover = run_game_over_screen(display, event_queue, timer, imagem_cursor);

                if (escolha_gameover == 1) { // 1 = Tentar Novamente
                    estado_atual_do_jogo = FASE_1; // Volta para a Fase 1
                }
                else { // 0 = Sair do Jogo
                    rodando = false;
                }
                break;
            }


            case FASE_1: {
                int resultado_fase1 = run_fase1_screen(display);

                if (resultado_fase1 == 1) { // 1 = Venceu
                    estado_atual_do_jogo = FASE_2;
                }
                else if (resultado_fase1 == 2) { // 2 = Perdeu
                    estado_atual_do_jogo = TELA_GAME_OVER;
                }
                else { // Qualquer outro valor (como 0) = Sair
                    rodando = false;
                }
                break;
            }

            // é necessario implementar a fase 3 aqui futuramente
            case FASE_2:
                printf("BEM-VINDO À FASE 2!\n");
                run_fase2_screen(display); 

               
                rodando = false;
                break;
            
        
        }
    }

 
    al_destroy_font(font_principal);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_bitmap(imagem_cursor);

    return 0;
}