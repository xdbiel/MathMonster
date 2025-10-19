#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
enum Direcao { ESQUERDA, DIREITA };
enum EstadoPersonagem { PARADO, ANDANDO };

    int run_fase1_screen(ALLEGRO_DISPLAY* display) {

        //REMOVI TODOS OS ADDON POIS A INICIALIZACAO DOS ADDONS SO PRECISA ESTAR NA MAIN (ARRUMAR DEPOIS0)


    al_set_window_position(display, 300, 300);
    al_set_window_title(display, "MathMonster");
    ALLEGRO_BITMAP* sprite_andar_direita = al_load_bitmap("personagem_movimento_direita.png"); // AL BIT MAP  (CARREGAR AS IMAGENS PARA USALAS POSTERIORMENTE)
    ALLEGRO_BITMAP* icone = al_load_bitmap("logol.png");
    ALLEGRO_BITMAP* sprite_andar_esquerda = al_load_bitmap("personagem_movimento_esquerda.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("china1.png"); // CENARIO 1
    ALLEGRO_FONT* font1 = al_load_ttf_font("joystix.ttf", 28, 0);
    ALLEGRO_FONT* font2 = al_load_ttf_font("joystix.ttf", 18, 0);
    ALLEGRO_BITMAP* sprite_parado_direita = al_load_bitmap("personagem_parado_direita.png");
    ALLEGRO_BITMAP* sprite_parado_esquerda = al_load_bitmap("personagem_parado_esquerda.png");

    al_set_display_icon(display, icone);

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
    enum EstadoPersonagem estado_atual = PARADO;

    float vel_y = 0.0f; 
    const float GRAVIDADE = 0.5f; 

    const float FORCA_PULO = -8.0f; 
    bool esta_no_chao = false;     

    bool tecla_direita_pressionada = false;
    bool tecla_esquerda_pressionada = false;

    bool rodando = true;
    bool redraw = true; 

    int bloco_x = 750;       
    int bloco_y = 368;    
    int bloco_largura = 32;  
    int bloco_altura = 32;   

    while (rodando) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return 0;
        }

        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {

          
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                tecla_direita_pressionada = true;

            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                tecla_esquerda_pressionada = true;

            }

            
            else if (event.keyboard.keycode == ALLEGRO_KEY_UP) { 
                if (esta_no_chao) {
                    vel_y = FORCA_PULO;
                    esta_no_chao = false;
                }
            }
        }
  
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {

            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                tecla_direita_pressionada = false;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                tecla_esquerda_pressionada = false;
            }
    


        }
        if (tecla_direita_pressionada || tecla_esquerda_pressionada) {
            estado_atual = ANDANDO;

           
            if (tecla_direita_pressionada) {
                pos_x += 3;
                direcao_personagem = DIREITA;
            }
            if (tecla_esquerda_pressionada) {
                pos_x -= 3;
                direcao_personagem = ESQUERDA;
            }
        }
        
        else {
            estado_atual = PARADO;
        }
        frame += 0.11111f;
        if (frame >= 6) {
            frame = 0;
        }
        if (frame >= 6) {
            frame = 0;
        }
  


        // GRAVIDADE 
       
        vel_y += GRAVIDADE;

    
        pos_y += vel_y;
     
        if (pos_x + 32 > bloco_x &&                
            pos_x < bloco_x + bloco_largura &&     
            pos_y + 32 > bloco_y &&              
            pos_y < bloco_y + bloco_altura)      
        {
           
            printf("Colisão com o bloco! Passando de fase...\n"); 
            return 1; // Retorna 1 para avisar o MAIN que a fase acabou
        }

        pos_y += vel_y;

      
        if (pos_y >= 400) {
            vel_y = 0;    
            pos_y = 400;  
            esta_no_chao = true; 
        }
        // ----------------------
        // ADICIONE ESTA LINHA PARA DEPURAR
        printf("Estado Atual: %d\n", estado_atual);

        al_clear_to_color(al_map_rgb(173, 216, 216));
        al_clear_to_color(al_map_rgb(173, 216, 216));
        al_clear_to_color(al_map_rgb(173, 216, 216));
        al_draw_bitmap(bg, 0, 0, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA / 2) - (al_get_font_line_height(font1) / 2), ALLEGRO_ALIGN_CENTER, "FASE 1");
     
        if (estado_atual == ANDANDO) {
       
            frame += 0.08f; // Velocidade da caminhada
            if (frame >= 6) frame = 0;

            if (direcao_personagem == DIREITA) {
                al_draw_bitmap_region(sprite_andar_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
            }
            else {
                al_draw_bitmap_region(sprite_andar_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
            }
        }
 
        else if (estado_atual == PARADO) {
           
            frame += 0.01f; 
            if (frame >= 4) frame = 0; 

            if (direcao_personagem == DIREITA) {
                al_draw_bitmap_region(sprite_parado_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
            }
            else {
              
          
                al_draw_bitmap_region(sprite_parado_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
            }
        }
        al_draw_filled_rectangle(
            bloco_x,
            bloco_y,
            bloco_x + bloco_largura,
            bloco_y + bloco_altura,
            al_map_rgb(255, 255, 0) // Amarelo
        );
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
    al_destroy_bitmap(sprite_parado_esquerda);
    al_destroy_bitmap(sprite_parado_direita);

    return 0;
}