// fase1.c

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

struct ProblemaMatematico {
    int num1;
    int num2;
    int resposta_certa;
    int resposta_errada;
    char texto_pergunta[32];
    char texto_resposta_certa[8];
    char texto_resposta_errada[8];
};

struct ProblemaMatematico gerar_problema() {
    // passar valores zerados da structs
    struct ProblemaMatematico p;


    p.num1 = 30 + (rand() % 401); // gerar os num aleatorio
    p.num2 = 30 + (rand() % 401); 

    
    p.resposta_certa = p.num1 + p.num2;

  
    p.resposta_errada = p.resposta_certa + (rand() % 3) + 1;

  
    sprintf(p.texto_pergunta, "%d + %d = ?", p.num1, p.num2);
    sprintf(p.texto_resposta_certa, "%d", p.resposta_certa);
    sprintf(p.texto_resposta_errada, "%d", p.resposta_errada);


    return p;
}


    int run_fase1_screen(ALLEGRO_DISPLAY* display) {

        //REMOVI TODOS OS ADDON POIS A INICIALIZACAO DOS ADDONS SO PRECISA ESTAR NA MAIN (ARRUMAR DEPOIS)


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
    ALLEGRO_BITMAP* tile_piso = al_load_bitmap("dungeon_piso_50x15.png");
    ALLEGRO_BITMAP* img_cubo = al_load_bitmap("cubo_resposta.png");
    ALLEGRO_FONT* font_cubo = al_load_ttf_font("joystix.ttf", 11, 4);
    ALLEGRO_BITMAP* img_fundo_equacao = al_load_bitmap("fundo_equacao.png");

    al_set_display_icon(display, icone);

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    float frame = 0.02f;
    int pos_x = 60;
    int pos_y = 468;
    int current_frame_y = 0;
    enum Direcao direcao_personagem = DIREITA;
    enum EstadoPersonagem estado_atual = PARADO;

    float vel_y = 0.0f; 
    const float GRAVIDADE = 0.5f; 

    const float FORCA_PULO = -9.0f; 
    bool esta_no_chao = true;     

    bool tecla_direita_pressionada = false;
    bool tecla_esquerda_pressionada = false;

    bool rodando = true;
    bool redraw = true; 

    int bloco_x = 750;       
    int bloco_y = 368;    
    int bloco_largura = 32;  
    int bloco_altura = 32;   

    int plataforma_x = 30;
    int plataforma_y = 500;
    int plataforma_largura = 700;
    int plataforma_altura = 32;

    float tempo_restante = 9.0f;

    int cubo_certo_x = 400;
    int cubo_certo_y = 380;
    int cubo_errado_x = 550;
    int cubo_errado_y = 380;
    int cubo_largura = 32; 
    int cubo_altura = 32;

    struct ProblemaMatematico problema_atual = gerar_problema();
    bool resposta_certa_na_esquerda = (rand() % 2 == 0); // para aleatorizar a posicao do cubo certo

    while (rodando) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
		if (event.type == ALLEGRO_EVENT_TIMER) { //precisei colocar esse if para o cronometro funcionar corretamente
        
            if (tempo_restante > 0) {
                tempo_restante -= 1.0 / 60.0; 
            }
            if (tempo_restante < 0) {
                tempo_restante = 0; 
            }

            if (tempo_restante == 0) {
                printf("TEMPO ESGOTADO!\n");
                return 2; 
            }
           
        }
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
            return 1; 
        }

       
      
        int pe_personagem = pos_y + 32; 
        int topo_plataforma = plataforma_y;
        int lado_esquerdo_personagem = pos_x;
        int lado_direito_personagem = pos_x + 32; 
        int lado_esquerdo_plataforma = plataforma_x;
        int lado_direito_plataforma = plataforma_x + plataforma_largura;

   
        if (vel_y > 0 &&
            pe_personagem >= topo_plataforma &&
            pe_personagem <= topo_plataforma + vel_y && 
            lado_direito_personagem > lado_esquerdo_plataforma &&
            lado_esquerdo_personagem < lado_direito_plataforma)
        {
        
            vel_y = 0;                  
            pos_y = topo_plataforma - 32; 
            esta_no_chao = true;        
        }
        else
        {
            esta_no_chao = false;
        }
    
        printf("Estado Atual: %d\n", estado_atual);

       
  
        al_clear_to_color(al_map_rgb(173, 216, 216));
        al_draw_bitmap(bg, 0, 0, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA / 2) - (al_get_font_line_height(font1) / 2), ALLEGRO_ALIGN_CENTER, "FASE 1");

 
            float fundo_eq_largura = al_get_bitmap_width(img_fundo_equacao);
            float fundo_eq_altura = al_get_bitmap_height(img_fundo_equacao);
            float fundo_eq_x = (LARGURA_TELA / 2) - 250 - (fundo_eq_largura / 2);
            float fundo_eq_y = 15; 

            al_draw_bitmap(img_fundo_equacao, fundo_eq_x, fundo_eq_y, 0);

            al_draw_text(font2, al_map_rgb(255, 255, 255), fundo_eq_x + (fundo_eq_largura / 2), fundo_eq_y + (fundo_eq_altura - al_get_font_line_height(font2)) / 2, ALLEGRO_ALIGN_CENTER,problema_atual.texto_pergunta);
        

    
        char texto_timer[32];
   
        sprintf(texto_timer, "Tempo: %.2f", tempo_restante);

        al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA - 30, 30, ALLEGRO_ALIGN_RIGHT, texto_timer);


            int tile_largura = 50; 
            // desenha o sprite do tijo n vezes, fica melhor do que fazer varios sprites de plataforma com diferente valores de x
            for (int x = plataforma_x; x < plataforma_x + plataforma_largura; x += tile_largura) {
               
                al_draw_bitmap(tile_piso, x, plataforma_y, 0);

            }
        //verifica o cubo certo 1 aceitou 2 errou indo para o tela game over
        if (pos_x + 32 > cubo_certo_x && pos_x < cubo_certo_x + cubo_largura &&
            pos_y + 32 > cubo_certo_y && pos_y < cubo_certo_y + cubo_altura)
        {
            if (resposta_certa_na_esquerda) { 
                printf("ACERTOU! (Esquerda)\n");
                return 1; 
            }
            else { 
                printf("ERROU! (Esquerda) Reiniciando...\n");
                return 2; 
            }
        }


        if (pos_x + 32 > cubo_errado_x && pos_x < cubo_errado_x + cubo_largura &&
            pos_y + 32 > cubo_errado_y && pos_y < cubo_errado_y + cubo_altura)
        {
            if (!resposta_certa_na_esquerda) { 
                printf("ACERTOU! (Direita)\n");
                return 1; 
            }
            else { 
                printf("ERROU! (Direita) Reiniciando...\n");
                return 2;
            }
        }



        if (estado_atual == ANDANDO) {
       
            frame += 0.08f; 
        

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
            al_map_rgb(255, 255, 255) 

        );

        if (img_cubo) {
            
            al_draw_bitmap(img_cubo, cubo_certo_x, cubo_certo_y, 0);
            al_draw_bitmap(img_cubo, cubo_errado_x, cubo_errado_y, 0);
            if (resposta_certa_na_esquerda) {
             
                al_draw_text(font_cubo, al_map_rgb(0, 0, 0),
                    cubo_certo_x + cubo_largura / 2,
                    cubo_certo_y + (cubo_altura - al_get_font_line_height(font_cubo)) / 2 - 1,
                    ALLEGRO_ALIGN_CENTER,
                    problema_atual.texto_resposta_certa); 
                al_draw_text(font_cubo, al_map_rgb(0, 0, 0),
                    cubo_errado_x + cubo_largura / 2,
                    cubo_errado_y + (cubo_altura - al_get_font_line_height(font_cubo)) / 2 - 1,
                    ALLEGRO_ALIGN_CENTER,
                    problema_atual.texto_resposta_errada); 
            }
            else {
          
                al_draw_text(font_cubo, al_map_rgb(0, 0, 0),
                    cubo_certo_x + cubo_largura / 2,
                    cubo_certo_y + (cubo_altura - al_get_font_line_height(font_cubo)) / 2 - 1,
                    ALLEGRO_ALIGN_CENTER,
                    problema_atual.texto_resposta_errada); 
                al_draw_text(font_cubo, al_map_rgb(0, 0, 0),
                    cubo_errado_x + cubo_largura / 2,
                    cubo_errado_y + (cubo_altura - al_get_font_line_height(font_cubo)) / 2 - 1,
                    ALLEGRO_ALIGN_CENTER,
                    problema_atual.texto_resposta_certa); 
            }
            //al_draw_text(font_cubo, al_map_rgb(0, 0, 0), cubo_errado_x + cubo_largura / 2 - 1,cubo_errado_y + (cubo_altura - al_get_font_line_height(font2)) / 2 + 3,ALLEGRO_ALIGN_CENTER,problema_atual.texto_resposta_errada);
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
    al_destroy_bitmap(sprite_parado_esquerda);
    al_destroy_bitmap(sprite_parado_direita);
    al_destroy_bitmap(tile_piso);
    al_destroy_bitmap(img_cubo);
    al_destroy_font(font_cubo);
    al_destroy_bitmap(img_fundo_equacao);

    return 0;
}