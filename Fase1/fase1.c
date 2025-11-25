
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include <string.h>
#define LARGURA_TELA 1366
#define ALTURA_TELA 768
#define PLAYER_W 50
#define PLAYER_H 50
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

   
    al_set_window_title(display, "MathMonster");
    ALLEGRO_BITMAP* sprite_andar_direita = al_load_bitmap("personagem_movimento_direita.png"); 
    ALLEGRO_BITMAP* icone = al_load_bitmap("logol.png");
    ALLEGRO_BITMAP* sprite_andar_esquerda = al_load_bitmap("personagem_movimento_esquerda.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("bg1.png"); 
    ALLEGRO_FONT* font1 = al_load_ttf_font("joystix.ttf", 28, 0);
    ALLEGRO_FONT* font2 = al_load_ttf_font("joystix.ttf", 18, 0);
    ALLEGRO_BITMAP* sprite_parado_direita = al_load_bitmap("personagem_parado_direita.png");
    ALLEGRO_BITMAP* sprite_parado_esquerda = al_load_bitmap("personagem_parado_esquerda.png");
    ALLEGRO_BITMAP* tile_piso = al_load_bitmap("dungeon_piso_50x15.png");
    ALLEGRO_BITMAP* img_cubo = al_load_bitmap("bloco_resposta_selecionado.png");
    ALLEGRO_FONT* font_cubo = al_load_ttf_font("joystix.ttf", 13, 4);
    ALLEGRO_BITMAP* img_fundo_equacao = al_load_bitmap("fundo_equacao.png");
    ALLEGRO_BITMAP* sprite_inimigo_sheet = al_load_bitmap("sprite-enemy.png");
    ALLEGRO_BITMAP* img_cubo_inativo = al_load_bitmap("bloco_resposta_normal.png");
    ALLEGRO_BITMAP* img_tutorial = al_load_bitmap("tutorial.png");
    ALLEGRO_BITMAP* img_cabeca = al_load_bitmap("cabeca.png");
    al_set_display_icon(display, icone);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_start_timer(timer);

    float frame = 0.02f;
 
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
    int bloco_largura = PLAYER_W;
    int bloco_altura = PLAYER_H;
    int plataforma_x = 0;
    int plataforma_y = 600;
    int plataforma_largura = 1366;
    int plataforma_altura = 32;
    int buraco_inicio = 500;
    int buraco_fim = 800;
    int timer_pisca = 0; 
    
    int plat_elev_x = 600;
    int plat_elev_y = 550;
    int plat_elev_w = 100;
    int plat_elev_h = 32;

   
    float vel_plataforma = 2.0f;
   
    int limite_esquerdo = 200;
    int limite_direito = 1100;
   
    int pos_x = 100;
    int pos_y = 600 - PLAYER_H;;
    float tempo_restante = 60.0f;
    int cubo_certo_x = 250;
    int cubo_certo_y = 465;
    int cubo_errado_x = 1100;
    int cubo_errado_y = 465;
    int cubo_largura = 32; 
    int cubo_altura = 32;
    struct ProblemaMatematico problema_atual = gerar_problema();
    bool resposta_certa_na_esquerda = (rand() % 2 == 0); 
    struct Inimigo morcego;
    int inimigos_vivos = 0;
    bool cubos_estao_ativos = false;
    morcego.x = 600;
    morcego.y = 550;
    morcego.vel_x = 1.0;               
    morcego.frame_largura = 43;      
    morcego.frame_altura = 25;       
    morcego.frame_atual = 0;         
    morcego.frame_timer = 0;         
    morcego.spritesheet = sprite_inimigo_sheet; 
    morcego.esta_vivo = true; 
    inimigos_vivos = 1;
    bool redesenhar = true;
    const char* falas_tutorial[] = {
     "Ai... Minha cabeca...",
     "Nossa! Onde foi que eu cai?",
     "Este lugar parece o antigo Subsolo Logico.",
     "O Senhor Aritmetica deve ter trazido meus amigos para ca.",
     "Preciso derrotar os capangas dele para abrir o caminho.",
     "Nao tenho tempo a perder. \nTenho que salva-los!"
    };
    int letras_aparecendo = 0;  
    int timer_texto = 0;        
    const int VELOCIDADE_TUTORIAL = 3; 
    int fala_atual = 0; 
    int total_falas = 6; 
    static bool tutorial_ja_foi_visto = false;
    bool tutorial_ativo = !tutorial_ja_foi_visto;

    static bool instrucoes_ja_vistas = false;

 
    if (!instrucoes_ja_vistas && img_tutorial) {

        bool mostrando_instrucoes = true;
        int timer_pisca = 0; 
        bool redesenhar_instrucao = true; 

        while (mostrando_instrucoes) {
            ALLEGRO_EVENT event;
            al_wait_for_event(event_queue, &event);

    
            if (event.type == ALLEGRO_EVENT_TIMER) {
                timer_pisca++; 
                redesenhar_instrucao = true;
            }
            else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    mostrando_instrucoes = false; 
                }
                else if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    return 0; 
                }
            }
            else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                return 0;
            }

      
            if (redesenhar_instrucao && al_is_event_queue_empty(event_queue)) {
                redesenhar_instrucao = false;

     
                al_draw_scaled_bitmap(img_tutorial,
                    0, 0, al_get_bitmap_width(img_tutorial), al_get_bitmap_height(img_tutorial),
                    0, 0, LARGURA_TELA, ALTURA_TELA, 0);

            
                if (timer_pisca % 60 < 30) {
                    al_draw_text(font2, al_map_rgb(255, 255, 255),
                        LARGURA_TELA - 30, ALTURA_TELA - 50,
                        ALLEGRO_ALIGN_RIGHT, "Aperte ENTER para continuar");
                }

                al_flip_display();
            }
        }


        instrucoes_ja_vistas = true;
    }

    while (rodando) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {

            redesenhar = true; 

          
            if (!tutorial_ativo) {

                if (tempo_restante > 0) tempo_restante -= 1.0 / 60.0;
                if (tempo_restante < 0) tempo_restante = 0;
                if (tempo_restante == 0) {
                    printf("TEMPO ESGOTADO!\n");
                    return 2; 
                }
               

               
                plat_elev_x += (int)vel_plataforma;

              
                if (plat_elev_x < buraco_inicio) {
                    vel_plataforma = 2.0f; 
                }
     
                else if (plat_elev_x + plat_elev_w > buraco_fim) {
                    vel_plataforma = -2.0f;
                }
     
                if (inimigos_vivos == 0) cubos_estao_ativos = true;

                if (morcego.esta_vivo) {
                    morcego.x += morcego.vel_x;
          
                    if (morcego.x <= plataforma_x) morcego.vel_x = 1.0;
                    if (morcego.x + morcego.frame_largura >= plataforma_x + plataforma_largura) morcego.vel_x = -1.0;

                    morcego.frame_timer++;
                    if (morcego.frame_timer >= 10) {
                        morcego.frame_timer = 0;
                        morcego.frame_atual++;
                        if (morcego.frame_atual >= 7) morcego.frame_atual = 0;
                    }
                }


                if (tecla_direita_pressionada) {
                    pos_x += 3;
                    direcao_personagem = DIREITA;
                    estado_atual = ANDANDO;
                }
                else if (tecla_esquerda_pressionada) {
                    pos_x -= 3;
                    direcao_personagem = ESQUERDA;
                    estado_atual = ANDANDO;
                }
                else {
                    estado_atual = PARADO;
                }


                if (estado_atual == ANDANDO) {
                    frame += 0.08f;
                    if (frame >= 6) frame = 0;
                }
                else { 
                    frame += 0.01f;
                    if (frame >= 4) frame = 0;
                }

              
                vel_y += GRAVIDADE;
                pos_y += vel_y;
                if (pos_y > ALTURA_TELA + 100) {
                    printf("CAIU NO BURACO! Game Over.\n");
                    return 2; 
                }
            
                if (pos_x < 0) pos_x = 0;
                if (pos_x + PLAYER_W > LARGURA_TELA) pos_x = LARGURA_TELA - PLAYER_W;

          
                if (pos_x + PLAYER_W > bloco_x && pos_x < bloco_x + bloco_largura &&
                    pos_y + PLAYER_H > bloco_y && pos_y < bloco_y + bloco_altura) {
                    return 1;
                }

                int pe_personagem = pos_y + PLAYER_H;
                int topo_plataforma = plataforma_y;
                int lado_direito_personagem = pos_x + PLAYER_W; 
                int centro_personagem_x = pos_x + (PLAYER_W / 2);

         
                bool sobre_chao_solido = (centro_personagem_x < buraco_inicio) || (centro_personagem_x > buraco_fim);

                if (vel_y > 0 &&
                    pe_personagem >= topo_plataforma && pe_personagem <= topo_plataforma + vel_y &&
                    lado_direito_personagem > plataforma_x && pos_x < plataforma_x + plataforma_largura &&
                    sobre_chao_solido)
                {
                    vel_y = 0;
                    pos_y = topo_plataforma - PLAYER_H;
                    esta_no_chao = true;
                    
                }
                else {
                    esta_no_chao = false;
                }
                if (vel_y > 0 &&
                    pos_y + PLAYER_H <= plat_elev_y + vel_y + 2 && 
                    pos_y + PLAYER_H >= plat_elev_y &&             
                    pos_x + PLAYER_W > plat_elev_x &&              
                    pos_x < plat_elev_x + plat_elev_w)
                {
                    vel_y = 0;
                    pos_y = plat_elev_y - PLAYER_H;
                    esta_no_chao = true;
                }
      
                if (morcego.esta_vivo) {
                    int j_dir = pos_x + PLAYER_W; int j_base = pos_y + PLAYER_H;
                    int i_dir = morcego.x + morcego.frame_largura; int i_base = morcego.y + morcego.frame_altura;

                    if (j_dir > morcego.x && pos_x < i_dir && j_base > morcego.y && pos_y < i_base) {
                        if (vel_y > 0 && (j_base < morcego.y + vel_y + 15)) {
                            vel_y = FORCA_PULO * 0.82;
                            morcego.esta_vivo = false;
                            inimigos_vivos--;
                        }
                        else {
                            return 2; 
                        }
                    }
                }

   
                if (cubos_estao_ativos) {
                    if (pos_x + PLAYER_W > cubo_certo_x && pos_x < cubo_certo_x + cubo_largura &&
                        pos_y + PLAYER_H > cubo_certo_y && pos_y < cubo_certo_y + cubo_altura) {
                        if (resposta_certa_na_esquerda) return 1; else return 2;
                    }
                    if (pos_x + PLAYER_W > cubo_errado_x && pos_x < cubo_errado_x + cubo_largura &&
                        pos_y + PLAYER_H > cubo_errado_y && pos_y < cubo_errado_y + cubo_altura) {
                        if (!resposta_certa_na_esquerda) return 1; else return 2;
                    }
                }
                if (pos_y + PLAYER_H == plat_elev_y) {

                 
                    if (pos_x + PLAYER_W > plat_elev_x && pos_x < plat_elev_x + plat_elev_w) {

                 
                        pos_x += (int)vel_plataforma;
                    }
                }

            }
  
            else {
                timer_texto++;
                if (timer_texto >= VELOCIDADE_TUTORIAL) {
                    timer_texto = 0;
                    int tamanho_total = strlen(falas_tutorial[fala_atual]);
                    if (letras_aparecendo < tamanho_total) {
                        letras_aparecendo++;
                    }
                }
            }
        }

    
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return 0;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) tecla_direita_pressionada = true;
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) tecla_esquerda_pressionada = true;
            else if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                if (esta_no_chao) {
                    vel_y = FORCA_PULO;
                    esta_no_chao = false;
                }
            }
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                if (tutorial_ativo) {
                    int tamanho_total = strlen(falas_tutorial[fala_atual]);
                    if (letras_aparecendo < tamanho_total) {
                        letras_aparecendo = tamanho_total;
                    }
                    else {
                        fala_atual++;
                        letras_aparecendo = 0;
                        if (fala_atual >= total_falas) {
                            tutorial_ativo = false;
                            tutorial_ja_foi_visto = true;
                        }
                    }
                }
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) tecla_direita_pressionada = false;
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) tecla_esquerda_pressionada = false;
        }

      
        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = false;

            al_clear_to_color(al_map_rgb(173, 216, 216));
            al_draw_bitmap(bg, 0, 0, 0);
            al_draw_text(font1, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA / 2) - 300, ALLEGRO_ALIGN_CENTER, "FASE 1");

     
            al_clear_to_color(al_map_rgb(173, 216, 216));
            al_draw_bitmap(bg, 0, 0, 0);
            al_draw_text(font1, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA / 2) - 300, ALLEGRO_ALIGN_CENTER, "FASE 1");

          
            al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA - 42, 60, ALLEGRO_ALIGN_RIGHT, problema_atual.texto_pergunta);
     
            char texto_timer[32];
            sprintf(texto_timer, "Tempo: %.2f", tempo_restante);
            al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA - 30, 30, ALLEGRO_ALIGN_RIGHT, texto_timer);

          
            if (tile_piso) {
                int tile_largura = 50;
                for (int x = plataforma_x; x < plataforma_x + plataforma_largura; x += tile_largura) {
                 
                    if (x < buraco_inicio || x >= buraco_fim) {
                        al_draw_bitmap(tile_piso, x, plataforma_y, 0);
                    }
                }
            }
            if (tile_piso) {
               
                al_draw_bitmap(tile_piso, plat_elev_x, plat_elev_y, 0);
                al_draw_bitmap(tile_piso, plat_elev_x + 50, plat_elev_y, 0);
            }

           
            if (cubos_estao_ativos && img_cubo) {
                al_draw_bitmap(img_cubo, cubo_certo_x, cubo_certo_y, 0);
                al_draw_bitmap(img_cubo, cubo_errado_x, cubo_errado_y, 0);

               
                int offset_y = (cubo_altura - al_get_font_line_height(font_cubo)) / 2 + 4;

                if (resposta_certa_na_esquerda) {
                 
                    al_draw_text(font_cubo, al_map_rgb(255, 255, 255),
                        cubo_certo_x + cubo_largura / 2 + 5,
                        cubo_certo_y + offset_y,
                        ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_certa);

                    al_draw_text(font_cubo, al_map_rgb(255, 255, 255),
                        cubo_errado_x + cubo_largura / 2 + 5,
                        cubo_errado_y + offset_y,
                        ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_errada);
                }
                else {
                    al_draw_text(font_cubo, al_map_rgb(255, 255, 255),
                        cubo_certo_x + cubo_largura / 2 + 5,
                        cubo_certo_y + offset_y ,
                        ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_errada);

                    al_draw_text(font_cubo, al_map_rgb(255, 255, 255),
                        cubo_errado_x + cubo_largura / 2 + 5,
                        cubo_errado_y + offset_y,
                        ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_certa);
                }
            }
            else {
               
                if (img_cubo_inativo) {
                   
                    al_draw_bitmap(img_cubo_inativo, cubo_certo_x, cubo_certo_y, 0);

                    
                    al_draw_bitmap(img_cubo_inativo, cubo_errado_x, cubo_errado_y, 0);
                }
            
            }

 
            if (morcego.esta_vivo) {
                int flags_inimigo = (morcego.vel_x < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
                al_draw_bitmap_region(morcego.spritesheet, morcego.frame_atual * morcego.frame_largura, 0, morcego.frame_largura, morcego.frame_altura, morcego.x, morcego.y, flags_inimigo);
            }

       
            int flags_player = (direcao_personagem == ESQUERDA) ? ALLEGRO_FLIP_HORIZONTAL : 0; 
       
            if (estado_atual == ANDANDO) {
                if (direcao_personagem == DIREITA) al_draw_scaled_bitmap(sprite_andar_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, PLAYER_W, PLAYER_H, 0);
                else al_draw_scaled_bitmap(sprite_andar_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, PLAYER_W, PLAYER_H, 0);
            }
            else {
                if (direcao_personagem == DIREITA) al_draw_scaled_bitmap(sprite_parado_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, PLAYER_W, PLAYER_H, 0);
                else al_draw_scaled_bitmap(sprite_parado_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, PLAYER_W, PLAYER_H, 0);
            }

          
            if (tutorial_ativo) {
                float caixa_x = 20, caixa_y = ALTURA_TELA - 150, caixa_w = LARGURA_TELA - 40, caixa_h = 130;

            
                al_draw_filled_rectangle(caixa_x, caixa_y, caixa_x + caixa_w, caixa_y + caixa_h, al_map_rgba(0, 0, 0, 170));
                al_draw_rectangle(caixa_x, caixa_y, caixa_x + caixa_w, caixa_y + caixa_h, al_map_rgb(255, 255, 255), 4);

              
                if (img_cabeca) {
                    
                    al_draw_bitmap(img_cabeca, caixa_x + 20, caixa_y + 20, 0);
                }

                
                char buffer_texto[256];
                strncpy(buffer_texto, falas_tutorial[fala_atual], letras_aparecendo);
                buffer_texto[letras_aparecendo] = '\0';

                
                al_draw_multiline_text(font2, al_map_rgb(255, 255, 255),
                    caixa_x + 110, caixa_y + 20, 
                    caixa_w - 130, 40, 0, buffer_texto); 


                al_draw_text(font_cubo, al_map_rgb(255, 255, 255), caixa_x + caixa_w - 20, caixa_y + caixa_h - 20, ALLEGRO_ALIGN_RIGHT, "Aperte ENTER >");
            }

            al_flip_display();
        }
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
    al_destroy_bitmap(img_cubo_inativo);
    al_destroy_bitmap(img_tutorial);
    al_destroy_bitmap(img_cabeca);

    return 0;
}