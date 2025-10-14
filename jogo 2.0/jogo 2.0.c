

// IMPORTANTE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// EU DEI O NOME QUE SERIA O MAIN.C DE JOGO 2.0 PQ SOU IDIOTA, IREI ARRUMA FUTURAMENTE, TRATEM O JOGO 2.0 COMO MAIN C
#include <stdio.h>


#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h> 
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>


#include "menu.h"
#include "fase1.h"
#include "fase2.h"

// variavel contendo todos os estados do jogo(irei adicionar a fase 3 futuramente)
enum GameState {
    TELA_INICIAL,
    FASE_1,
    FASE_2,
    FASE_3,
    TELA_FINAL
};


int main() {

    //iniciando os addons do allegor
    al_init();
    al_init_font_addon();
    al_init_ttf_addon();
    al_init_image_addon();
    al_install_keyboard();
    al_init_primitives_addon();


    ALLEGRO_DISPLAY* display = al_create_display(800, 600);
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

    al_start_timer(timer);

    
    

    enum GameState estado_atual_do_jogo = TELA_INICIAL; // variavel que controla qual tela estamos
    bool rodando = true;

    while (rodando) {
        //usei o switch para decidir com qual funcao vou ''trabalahar'' qual funcao vai ser exibida na tela
        switch (estado_atual_do_jogo) {

            
            case TELA_INICIAL: {
                int escolha_menu = run_menu_screen(display, event_queue, font_principal, timer);

                if (escolha_menu == 1) { // 1 = JOGAR
                    estado_atual_do_jogo = FASE_1; // Muda o estado para a fase )1
                } else { // 2 = SAIR ou fechou a janela
                    rodando = false; 
                }
                break;
            }

            
            case FASE_1: { 
                // Chama a função da fase 1 e GUARDA o resultado que ela retorna
                int resultado_fase1 = run_fase1_screen(display);

            
                if (resultado_fase1 == 1) { // 1 significa "Passou de Fase"
                    estado_atual_do_jogo = FASE_2; // MUDA O ESTADO PARA A PRÓXIMA FASE!
                }
                else { 
                    rodando = false; 
                }
                break;
            }

            // Aqui entrariam as outras fases
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

    return 0;
}