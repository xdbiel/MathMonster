// fase1.c
//ASSIM QUE POSSIVEL PRECISO REESTRUTURAR TODO O LOOP, POIS ESTA MUITO BAGUNCADO E POR ISSO ALGUMAS FUNCOES ESTAO 'BUGANDOP' COMO O SPRITE DO PERSONAGEM PISCANDO
//AO SEGURAR UMA TECLA DE MOVIMENTO, E OUTROS DETALHES MENOS IMPORTANTES.
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
struct Inimigo {
    float x, y;         
    float vel_x;        

    int frame_largura;  
    int frame_altura;   
    int frame_atual;    
    int frame_timer;    

    ALLEGRO_BITMAP* spritesheet; 

    bool esta_vivo;
};
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
    
    struct ProblemaMatematico p;


    p.num1 = 1 + (rand() % 20); 
    p.num2 = 10 + (rand() % 50); 

    
    p.resposta_certa = p.num1 + p.num2;

  
    p.resposta_errada = p.resposta_certa + (rand() % 3) + 1;

  
    sprintf(p.texto_pergunta, "%d + %d = ?", p.num1, p.num2);
    sprintf(p.texto_resposta_certa, "%d", p.resposta_certa);
    sprintf(p.texto_resposta_errada, "%d", p.resposta_errada);


    return p;
}


    int run_fase1_screen(ALLEGRO_DISPLAY* display) {

        


    al_set_window_position(display, 300, 300);
    al_set_window_title(display, "MathMonster");
    ALLEGRO_BITMAP* sprite_andar_direita = al_load_bitmap("personagem_movimento_direita.png"); 
    ALLEGRO_BITMAP* icone = al_load_bitmap("logol.png");
    ALLEGRO_BITMAP* sprite_andar_esquerda = al_load_bitmap("personagem_movimento_esquerda.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("china1.png"); // nas pasta do jogo 2.0, vou alterar por aqui meu bg o nome (china1) é o padrao
    ALLEGRO_FONT* font1 = al_load_ttf_font("joystix.ttf", 28, 0);
    ALLEGRO_FONT* font2 = al_load_ttf_font("joystix.ttf", 18, 0);
    ALLEGRO_BITMAP* sprite_parado_direita = al_load_bitmap("personagem_parado_direita.png");
    ALLEGRO_BITMAP* sprite_parado_esquerda = al_load_bitmap("personagem_parado_esquerda.png");
    ALLEGRO_BITMAP* tile_piso = al_load_bitmap("dungeon_piso_50x15.png");
    ALLEGRO_BITMAP* img_cubo = al_load_bitmap("cubo_resposta.png");
    ALLEGRO_FONT* font_cubo = al_load_ttf_font("joystix.ttf", 11, 4);
    ALLEGRO_BITMAP* img_fundo_equacao = al_load_bitmap("fundo_equacao.png");
    ALLEGRO_BITMAP* sprite_inimigo_sheet = al_load_bitmap("sprite-enemy.png");

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

    float tempo_restante = 60.0f;

    int cubo_certo_x = 400;
    int cubo_certo_y = 380;
    int cubo_errado_x = 550;
    int cubo_errado_y = 380;
    int cubo_largura = 32; 
    int cubo_altura = 32;
    struct ProblemaMatematico problema_atual = gerar_problema();
    bool resposta_certa_na_esquerda = (rand() % 2 == 0); 
    struct Inimigo morcego;
    int inimigos_vivos = 0;
    bool cubos_estao_ativos = false;
    morcego.x = 400;                 
    morcego.y = 450;
    morcego.vel_x = 1.0;               
    morcego.frame_largura = 43;      
    morcego.frame_altura = 25;       
    morcego.frame_atual = 0;         
    morcego.frame_timer = 0;         
    morcego.spritesheet = sprite_inimigo_sheet; 
    morcego.esta_vivo = true; 
    inimigos_vivos = 1;

   
  


    while (rodando) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
		if (event.type == ALLEGRO_EVENT_TIMER) { //arrumar o loop principal do jogo(while(rodando)) para eviar bugs visuais, como esta acontecendo agora
        
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

        if (inimigos_vivos == 0) {
            cubos_estao_ativos = true;
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
        
        morcego.x += morcego.vel_x;


        if (morcego.x <= plataforma_x) {
            morcego.vel_x = 1.0; 
        }
        if (morcego.x + morcego.frame_largura >= plataforma_x + plataforma_largura) {
            morcego.vel_x = -1.0; 
        }

        
        morcego.frame_timer++;
        if (morcego.frame_timer >= 10) { 
            morcego.frame_timer = 0;
            morcego.frame_atual++;
            if (morcego.frame_atual >= 7) { 
                morcego.frame_atual = 0;
            }
        }

 
       
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
        int jogador_esquerda = pos_x;
        int jogador_direita = pos_x + 32;
        int jogador_topo = pos_y;
        int jogador_base = pos_y + 32;

        int inimigo_esquerda = morcego.x;
        int inimigo_direita = morcego.x + morcego.frame_largura;
        int inimigo_topo = morcego.y;
        int inimigo_base = morcego.y + morcego.frame_altura;


        if (jogador_direita > inimigo_esquerda &&
            jogador_esquerda < inimigo_direita &&
            jogador_base > inimigo_topo &&
            jogador_topo < inimigo_base)
        {

            if (vel_y > 0 && (jogador_base < inimigo_topo + vel_y + 10))
            {
                
                printf("INIMIGO DERROTADO!\n");


                vel_y = FORCA_PULO * 0.75; 


                if (morcego.esta_vivo) { 
                    morcego.esta_vivo = false;
                    inimigos_vivos--; 
                } 
                morcego.y = -1000;
                morcego.vel_x = 0; 
            }
            else
            {
                printf("COLISÃO COM O INIMIGO! Game Over.\n");
                return 2; 
            }
        }
        printf("Estado Atual: %d\n", estado_atual);

       
  
        al_clear_to_color(al_map_rgb(173, 216, 216));
        al_draw_bitmap(bg, 0, 0, 0);
        al_draw_text(font1, al_map_rgb(0, 0, 0), LARGURA_TELA / 2, (ALTURA_TELA / 2) - (al_get_font_line_height(font1) / 2), ALLEGRO_ALIGN_CENTER, "FASE 1");

 
            float fundo_eq_largura = al_get_bitmap_width(img_fundo_equacao);
            float fundo_eq_altura = al_get_bitmap_height(img_fundo_equacao);
            float fundo_eq_x = (LARGURA_TELA / 2) - 250 - (fundo_eq_largura / 2);
            float fundo_eq_y = 15; 

            al_draw_bitmap(img_fundo_equacao, fundo_eq_x, fundo_eq_y, 0);

            al_draw_text(font2, al_map_rgb(255, 255, 255), fundo_eq_x + (fundo_eq_largura / 2), fundo_eq_y + (fundo_eq_altura - al_get_font_line_height(font2)) / 2, ALLEGRO_ALIGN_CENTER,problema_atual.texto_pergunta);
        

    
        char texto_timer[32];
   
        sprintf(texto_timer, "Tempo: %.2f", tempo_restante);

        al_draw_text(font2, al_map_rgb(0, 0, 0), LARGURA_TELA - 30, 30, ALLEGRO_ALIGN_RIGHT, texto_timer);


            int tile_largura = 50; 

            for (int x = plataforma_x; x < plataforma_x + plataforma_largura; x += tile_largura) {
               
                al_draw_bitmap(tile_piso, x, plataforma_y, 0);

            }
            if (cubos_estao_ativos == true) {
                
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
            }
        if (morcego.esta_vivo) {
            int flags_inimigo = 0;
            if (morcego.vel_x < 0) { 
                flags_inimigo = ALLEGRO_FLIP_HORIZONTAL; 
            }

          
            al_draw_bitmap_region(
                morcego.spritesheet,         
                morcego.frame_atual * morcego.frame_largura, 
                0,                             
                morcego.frame_largura,          
                morcego.frame_altura,           
                morcego.x,                    
                morcego.y,                      
                flags_inimigo                  
            );
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
            al_map_rgb(0, 0, 0) 

        );
   
        if (cubos_estao_ativos == true) {

          
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
            }
        }
        else {
           

        
            al_draw_filled_rectangle(
                cubo_certo_x, cubo_certo_y,
                cubo_certo_x + cubo_largura, cubo_certo_y + cubo_altura,
                al_map_rgb(20, 20, 20) 
            );
            
            al_draw_filled_rectangle(
                cubo_errado_x, cubo_errado_y,
                cubo_errado_x + cubo_largura, cubo_errado_y + cubo_altura,
                al_map_rgb(20, 20, 20) 
            );
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
    al_destroy_bitmap(sprite_inimigo_sheet);

    return 0;
}