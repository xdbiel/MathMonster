
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "fase2.h" 

#define LARGURA_TELA 1366
#define ALTURA_TELA 768
#define PLAYER_W 52
#define PLAYER_H 52


struct Plataforma {
    int x, y, w, h;
};

#define QTD_PLATAFORMAS 7
struct Plataforma plataformas_fixas[QTD_PLATAFORMAS] = {
    {0, 650, 350, 40},     
    {1000, 650, 366, 40},  
    {50, 400, 200, 30},    
    {1100, 350, 200, 30},  
    {440, 580, 140, 30},   
    {270, 500, 140, 30},   
    {680, 500, 250, 30}    
};

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

struct Projetil {
    float x, y;
    float vel_x, vel_y;
    bool ativo;
    int raio;
};

struct InimigoAtirador {
    float x, y;
    int w, h;
    bool vivo;
    int cooldown_tiro;
    ALLEGRO_BITMAP* img_parado;
    ALLEGRO_BITMAP* img_atirando;
    ALLEGRO_BITMAP* img_projetil;
    bool pose_atirando_ativa;
    int timer_pose;
    struct Projetil bala;
};


struct ItemRelogio {
    float x, y;
    int w, h;
    bool ativo;
    ALLEGRO_BITMAP* sprite;
    int frame_atual;
    int frame_timer;
    int src_w, src_h;
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

struct ProblemaMatematico gerar_problema_fase2() {
    struct ProblemaMatematico p;
    p.num1 = 50 + (rand() % 100);
    p.num2 = 50 + (rand() % 100);
    p.resposta_certa = p.num1 + p.num2;
    p.resposta_errada = p.resposta_certa + (rand() % 10) + 1;
    sprintf(p.texto_pergunta, "%d + %d = ?", p.num1, p.num2);
    sprintf(p.texto_resposta_certa, "%d", p.resposta_certa);
    sprintf(p.texto_resposta_errada, "%d", p.resposta_errada);
    return p;
}

int run_fase2_screen(ALLEGRO_DISPLAY* display) {

    al_set_window_title(display, "MathMonster - Fase 2");


    ALLEGRO_BITMAP* sprite_andar_direita = al_load_bitmap("personagem_movimento_direita.png");
    ALLEGRO_BITMAP* sprite_andar_esquerda = al_load_bitmap("personagem_movimento_esquerda.png");
    ALLEGRO_BITMAP* sprite_parado_direita = al_load_bitmap("personagem_parado_direita.png");
    ALLEGRO_BITMAP* sprite_parado_esquerda = al_load_bitmap("personagem_parado_esquerda.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("bg2.png");
    ALLEGRO_BITMAP* tile_piso = al_load_bitmap("dungeon_piso_50x15.png");
    ALLEGRO_BITMAP* img_cubo = al_load_bitmap("bloco_resposta_selecionado.png");
    ALLEGRO_BITMAP* img_cubo_inativo = al_load_bitmap("bloco_resposta_normal.png");
    ALLEGRO_BITMAP* sprite_inimigo_sheet = al_load_bitmap("sprite-enemy.png");

    ALLEGRO_BITMAP* bmp_inimigo_parado = al_load_bitmap("inimigo1_parado.png");
    ALLEGRO_BITMAP* bmp_inimigo_atirando = al_load_bitmap("inimigo1_atirando.png");
    ALLEGRO_BITMAP* bmp_tiro = al_load_bitmap("inimigo1_tiro.png");
    ALLEGRO_BITMAP* bmp_relogio = al_load_bitmap("relogio_sprite.png");


    ALLEGRO_BITMAP* img_tutorial = al_load_bitmap("tutorial_fase_2.png");

    ALLEGRO_FONT* font1 = al_load_ttf_font("joystix.ttf", 28, 0);
    ALLEGRO_FONT* font2 = al_load_ttf_font("joystix.ttf", 18, 0);
    ALLEGRO_FONT* font_cubo = al_load_ttf_font("joystix.ttf", 13, 0);



    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());

    al_start_timer(timer);


    bool rodando = true;
    bool redesenhar = true;
    bool tecla_direita_pressionada = false;
    bool tecla_esquerda_pressionada = false;

    static bool tutorial_ja_visto = false;
    bool exibindo_tutorial = !tutorial_ja_visto;
    int timer_blink = 0; 

    float frame = 0.0f;
    int pos_x = 50;
    int pos_y = 550;
    enum Direcao direcao_personagem = DIREITA;
    enum EstadoPersonagem estado_atual = PARADO;
    float vel_y = 0.0f;
    const float GRAVIDADE = 0.5f;
    const float FORCA_PULO = -10.0f;
    bool esta_no_chao = true;

    int plat_elev_x = 850;
    int plat_elev_y = 410;
    int plat_elev_w = 150;
    int plat_elev_h = 20;
    float vel_plataforma = 2.0f;
    int limite_esq_plat = 800;
    int limite_dir_plat = 1200;

    float tempo_restante = 12.0f;
    struct ProblemaMatematico problema_atual = gerar_problema_fase2();

    int cubo_largura = 50;
    int cubo_altura = 50;
    int cubo_certo_x, cubo_certo_y;
    int cubo_errado_x, cubo_errado_y;
    bool resposta_certa_na_esquerda = (rand() % 2 == 0);
    bool cubos_estao_ativos = false;

    if (resposta_certa_na_esquerda) {
        cubo_certo_x = plataformas_fixas[2].x + 75;
        cubo_certo_y = plataformas_fixas[2].y - 50;
        cubo_errado_x = plataformas_fixas[3].x + 75;
        cubo_errado_y = plataformas_fixas[3].y - 50;
    }
    else {
        cubo_certo_x = plataformas_fixas[3].x + 75;
        cubo_certo_y = plataformas_fixas[3].y - 50;
        cubo_errado_x = plataformas_fixas[2].x + 75;
        cubo_errado_y = plataformas_fixas[2].y - 50;
    }


    struct Inimigo morcego;
    morcego.x = plataformas_fixas[6].x;
    morcego.y = plataformas_fixas[6].y - 40;
    morcego.vel_x = 2.0;
    morcego.frame_largura = 43;
    morcego.frame_altura = 25;
    morcego.frame_atual = 0;
    morcego.frame_timer = 0;
    morcego.spritesheet = sprite_inimigo_sheet;
    morcego.esta_vivo = true;

    int inimigos_vivos = 3;


#define QTD_ATIRADORES 2
    struct InimigoAtirador atiradores[QTD_ATIRADORES];


    atiradores[0].x = 1010;
    atiradores[0].w = 80;
    atiradores[0].h = 80;
    atiradores[0].y = 650 - 80;
    atiradores[0].vivo = true;
    atiradores[0].cooldown_tiro = 0;
    atiradores[0].img_parado = bmp_inimigo_parado;
    atiradores[0].img_atirando = bmp_inimigo_atirando;
    atiradores[0].img_projetil = bmp_tiro;
    atiradores[0].pose_atirando_ativa = false;
    atiradores[0].timer_pose = 0;
    atiradores[0].bala.ativo = false;
    atiradores[0].bala.raio = 6;


    atiradores[1].x = 50;
    atiradores[1].w = 80;
    atiradores[1].h = 80;
    atiradores[1].y = 400 - 80;
    atiradores[1].vivo = true;
    atiradores[1].cooldown_tiro = 60;
    atiradores[1].img_parado = bmp_inimigo_parado;
    atiradores[1].img_atirando = bmp_inimigo_atirando;
    atiradores[1].img_projetil = bmp_tiro;
    atiradores[1].pose_atirando_ativa = false;
    atiradores[1].timer_pose = 0;
    atiradores[1].bala.ativo = false;
    atiradores[1].bala.raio = 6;


    struct ItemRelogio relogio;
    relogio.x = 1190;
    relogio.y = 600;
    relogio.w = 50;
    relogio.h = 50;
    relogio.ativo = true;
    relogio.sprite = bmp_relogio;
    relogio.frame_atual = 0;
    relogio.frame_timer = 0;
    relogio.src_w = 166;
    relogio.src_h = 170;


    bool mostrar_popup_tempo = false;
    int timer_popup = 0;
    float popup_y_flutuante = 0;
    float popup_x_fixo = 0;


    while (rodando) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;

          
            if (!exibindo_tutorial) {

                if (tempo_restante > 0) tempo_restante -= 1.0 / 60.0;
                if (tempo_restante <= 0) {
                    al_destroy_timer(timer); al_destroy_event_queue(event_queue);
                    return 2;
                }

                if (inimigos_vivos == 0) cubos_estao_ativos = true;


                if (mostrar_popup_tempo) {
                    timer_popup++;
                    popup_y_flutuante -= 0.5f;
                    if (timer_popup > 60) {
                        mostrar_popup_tempo = false;
                        timer_popup = 0;
                    }
                }

  
                if (relogio.ativo) {
                    relogio.frame_timer++;
                    if (relogio.frame_timer >= 25) {
                        relogio.frame_timer = 0;
                        relogio.frame_atual++;
                        if (relogio.frame_atual >= 6) relogio.frame_atual = 0;
                    }
                    if (pos_x + PLAYER_W > relogio.x && pos_x < relogio.x + relogio.w &&
                        pos_y + PLAYER_H > relogio.y && pos_y < relogio.y + relogio.h) {
                        relogio.ativo = false;
                        tempo_restante += 15.0f;
                        mostrar_popup_tempo = true;
                        popup_x_fixo = pos_x;
                        popup_y_flutuante = pos_y - 20;
                        timer_popup = 0;
                    }
                }


                if (morcego.esta_vivo) {
                    morcego.x += morcego.vel_x;
                    if (morcego.x <= plataformas_fixas[6].x) morcego.vel_x = 2.0;
                    if (morcego.x + morcego.frame_largura >= plataformas_fixas[6].x + plataformas_fixas[6].w) morcego.vel_x = -2.0;

                    morcego.frame_timer++;
                    if (morcego.frame_timer >= 10) {
                        morcego.frame_timer = 0;
                        morcego.frame_atual++;
                        if (morcego.frame_atual >= 7) morcego.frame_atual = 0;
                    }
                }

                for (int i = 0; i < QTD_ATIRADORES; i++) {
                    if (atiradores[i].vivo) {
                        if (atiradores[i].pose_atirando_ativa) {
                            atiradores[i].timer_pose++;
                            if (atiradores[i].timer_pose > 30) {
                                atiradores[i].pose_atirando_ativa = false;
                                atiradores[i].timer_pose = 0;
                            }
                        }
                        atiradores[i].cooldown_tiro++;
                        if (atiradores[i].cooldown_tiro >= 120) {
                            if (!atiradores[i].bala.ativo) {
                                atiradores[i].pose_atirando_ativa = true;
                                atiradores[i].timer_pose = 0;
                                atiradores[i].bala.ativo = true;
                                atiradores[i].bala.x = atiradores[i].x + atiradores[i].w / 2;
                                atiradores[i].bala.y = atiradores[i].y + atiradores[i].h / 2;
                                float cx = pos_x + PLAYER_W / 2; float cy = pos_y + PLAYER_H / 2;
                                float dx = cx - atiradores[i].bala.x; float dy = cy - atiradores[i].bala.y;
                                float dist = sqrt(dx * dx + dy * dy);
                                if (dist > 0) {
                                    atiradores[i].bala.vel_x = (dx / dist) * 4.0f;
                                    atiradores[i].bala.vel_y = (dy / dist) * 4.0f;
                                }
                                atiradores[i].cooldown_tiro = 0;
                            }
                        }
                        int margem_lateral = 25; int margem_topo = 15;
                        if (pos_x + PLAYER_W > atiradores[i].x + margem_lateral &&
                            pos_x < (atiradores[i].x + atiradores[i].w) - margem_lateral &&
                            pos_y + PLAYER_H > atiradores[i].y + margem_topo &&
                            pos_y < atiradores[i].y + atiradores[i].h) {

                            if (vel_y > 0 && pos_y + PLAYER_H < atiradores[i].y + atiradores[i].h / 2) {
                                atiradores[i].vivo = false; inimigos_vivos--; vel_y = -13.0f;
                            }
                            else {
                                al_destroy_timer(timer); al_destroy_event_queue(event_queue); return 2;
                            }
                        }
                    }
                    if (atiradores[i].bala.ativo) {
                        atiradores[i].bala.x += atiradores[i].bala.vel_x;
                        atiradores[i].bala.y += atiradores[i].bala.vel_y;
                        if (atiradores[i].bala.x > pos_x && atiradores[i].bala.x < pos_x + PLAYER_W &&
                            atiradores[i].bala.y > pos_y && atiradores[i].bala.y < pos_y + PLAYER_H) {
                            al_destroy_timer(timer); al_destroy_event_queue(event_queue); return 2;
                        }
                        if (atiradores[i].bala.x < 0 || atiradores[i].bala.x > LARGURA_TELA ||
                            atiradores[i].bala.y < 0 || atiradores[i].bala.y > ALTURA_TELA) {
                            atiradores[i].bala.ativo = false;
                        }
                    }
                }

                plat_elev_x += (int)vel_plataforma;
                if (plat_elev_x < limite_esq_plat) vel_plataforma = 2.0f;
                else if (plat_elev_x + plat_elev_w > limite_dir_plat) vel_plataforma = -2.0f;

                if (tecla_direita_pressionada) { pos_x += 4; direcao_personagem = DIREITA; estado_atual = ANDANDO; }
                else if (tecla_esquerda_pressionada) { pos_x -= 4; direcao_personagem = ESQUERDA; estado_atual = ANDANDO; }
                else { estado_atual = PARADO; }

                if (pos_x < 0) pos_x = 0; if (pos_x + PLAYER_W > LARGURA_TELA) pos_x = LARGURA_TELA - PLAYER_W;
                if (estado_atual == ANDANDO) { frame += 0.15f; if (frame >= 6) frame = 0; }
                else { frame += 0.05f; if (frame >= 4) frame = 0; }
                vel_y += GRAVIDADE; pos_y += vel_y;
                if (pos_y > ALTURA_TELA + 50) { al_destroy_timer(timer); al_destroy_event_queue(event_queue); return 2; }

                esta_no_chao = false; int pe_y = pos_y + PLAYER_H; int margem_pe = 10;
                if (vel_y >= 0) {
                    for (int i = 0; i < QTD_PLATAFORMAS; i++) {
                        struct Plataforma p = plataformas_fixas[i];
                        if (pe_y >= p.y && pe_y <= p.y + vel_y + 2) {
                            if (pos_x + PLAYER_W - margem_pe >= p.x && pos_x + margem_pe <= p.x + p.w) {
                                vel_y = 0; pos_y = p.y - PLAYER_H; esta_no_chao = true; break;
                            }
                        }
                    }
                }
                if (!esta_no_chao && vel_y >= 0) {
                    if (pe_y >= plat_elev_y && pe_y <= plat_elev_y + vel_y + 2) {
                        if (pos_x + PLAYER_W - margem_pe >= plat_elev_x && pos_x + margem_pe <= plat_elev_x + plat_elev_w) {
                            vel_y = 0; pos_y = plat_elev_y - PLAYER_H; esta_no_chao = true; pos_x += (int)vel_plataforma;
                        }
                    }
                }

                if (morcego.esta_vivo) {
                    if (pos_x + PLAYER_W > morcego.x && pos_x < morcego.x + morcego.frame_largura &&
                        pos_y + PLAYER_H > morcego.y && pos_y < morcego.y + morcego.frame_altura) {
                        if (vel_y > 0 && (pos_y + PLAYER_H < morcego.y + vel_y + 20)) {
                            vel_y = FORCA_PULO * 0.8; morcego.esta_vivo = false; inimigos_vivos--;
                        }
                        else { al_destroy_timer(timer); al_destroy_event_queue(event_queue); return 2; }
                    }
                }

                if (cubos_estao_ativos) {
                    if (pos_x + PLAYER_W > cubo_certo_x && pos_x < cubo_certo_x + cubo_largura &&
                        pos_y + PLAYER_H > cubo_certo_y && pos_y < cubo_certo_y + cubo_altura) {
                        al_destroy_timer(timer); al_destroy_event_queue(event_queue); return 1;
                    }
                    if (pos_x + PLAYER_W > cubo_errado_x && pos_x < cubo_errado_x + cubo_largura &&
                        pos_y + PLAYER_H > cubo_errado_y && pos_y < cubo_errado_y + cubo_altura) {
                        al_destroy_timer(timer); al_destroy_event_queue(event_queue); return 2;
                    }
                }
            } 
            else {
                
                timer_blink++;
            }

        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            return 0;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {

          
            if (exibindo_tutorial) {
                if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    exibindo_tutorial = false; 
                    tutorial_ja_visto = true;
                }
            }
            else {
               
                if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) tecla_direita_pressionada = true;
                else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) tecla_esquerda_pressionada = true;
                else if (event.keyboard.keycode == ALLEGRO_KEY_UP) { if (esta_no_chao) { vel_y = FORCA_PULO; esta_no_chao = false; } }
            }

        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) tecla_direita_pressionada = false;
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) tecla_esquerda_pressionada = false;
        }

        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = false;
            al_clear_to_color(al_map_rgb(40, 40, 60));

          
            if (exibindo_tutorial) {
            
                al_draw_scaled_bitmap(img_tutorial, 0, 0, al_get_bitmap_width(img_tutorial), al_get_bitmap_height(img_tutorial),
                    0, 0, LARGURA_TELA, ALTURA_TELA, 0);

           
                if (timer_blink % 60 < 35) {
                    al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA / 2 + 430, ALTURA_TELA - 53, ALLEGRO_ALIGN_CENTER, "APERTE ENTER PARA CONTINUAR");
                }
            }
            else {
   
                al_draw_scaled_bitmap(bg, 0, 0, al_get_bitmap_width(bg), al_get_bitmap_height(bg), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);

                al_draw_text(font1, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 20, ALLEGRO_ALIGN_CENTER, "FASE 2");
                al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA - 20, 70, ALLEGRO_ALIGN_RIGHT, problema_atual.texto_pergunta);
                char texto_timer[32]; sprintf(texto_timer, "Tempo: %.2f", tempo_restante);
                al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA - 30, 30, ALLEGRO_ALIGN_RIGHT, texto_timer);

                if (tile_piso) {
                    for (int i = 0; i < QTD_PLATAFORMAS; i++) {
                        struct Plataforma p = plataformas_fixas[i];
                        for (int k = 0; k < p.w; k += 50) al_draw_bitmap(tile_piso, p.x + k, p.y, 0);
                    }
                }
                else {
                    for (int i = 0; i < QTD_PLATAFORMAS; i++) {
                        struct Plataforma p = plataformas_fixas[i]; al_draw_filled_rectangle(p.x, p.y, p.x + p.w, p.y + p.h, al_map_rgb(150, 50, 50));
                    }
                }

                al_draw_filled_rectangle(plat_elev_x, plat_elev_y, plat_elev_x + plat_elev_w, plat_elev_y + 10, al_map_rgb(255, 215, 0));
                if (tile_piso) {
                    al_draw_bitmap(tile_piso, plat_elev_x, plat_elev_y, 0);
                    al_draw_bitmap(tile_piso, plat_elev_x + 50, plat_elev_y, 0);
                    al_draw_bitmap(tile_piso, plat_elev_x + 100, plat_elev_y, 0);
                }

                if (cubos_estao_ativos && img_cubo) {
                    al_draw_bitmap(img_cubo, cubo_certo_x, cubo_certo_y, 0);
                    al_draw_bitmap(img_cubo, cubo_errado_x, cubo_errado_y, 0);
                    int offset_y = (cubo_altura - al_get_font_line_height(font_cubo)) / 2 - 4;

          
                    if (resposta_certa_na_esquerda) {
                        al_draw_text(font_cubo, al_map_rgb(255, 255, 255), cubo_certo_x + cubo_largura / 2 - 3, cubo_certo_y + offset_y, ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_certa);
                        al_draw_text(font_cubo, al_map_rgb(255, 255, 255), cubo_errado_x + cubo_largura / 2 - 3, cubo_errado_y + offset_y, ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_errada);
                    }
                    else {
                        al_draw_text(font_cubo, al_map_rgb(255, 255, 255), cubo_certo_x + cubo_largura / 2 - 3, cubo_certo_y + offset_y, ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_certa);
                        al_draw_text(font_cubo, al_map_rgb(255, 255, 255), cubo_errado_x + cubo_largura / 2 - 3, cubo_errado_y + offset_y, ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_errada);
                    }
                }
                else if (img_cubo_inativo) {
                    al_draw_bitmap(img_cubo_inativo, cubo_certo_x, cubo_certo_y, 0);
                    al_draw_bitmap(img_cubo_inativo, cubo_errado_x, cubo_errado_y, 0);
                }

           
                if (relogio.ativo) {
                    int col = relogio.frame_atual % 3;
                    int row = relogio.frame_atual / 3;
                    int sx = col * relogio.src_w;
                    int sy = row * relogio.src_h;
                    al_draw_scaled_bitmap(relogio.sprite, sx, sy, relogio.src_w, relogio.src_h, relogio.x, relogio.y, relogio.w, relogio.h, 0);
                }

                if (morcego.esta_vivo) {
                    int flags_inimigo = (morcego.vel_x < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
                    al_draw_bitmap_region(morcego.spritesheet, morcego.frame_atual * morcego.frame_largura, 0, morcego.frame_largura, morcego.frame_altura, morcego.x, morcego.y, flags_inimigo);
                }

                for (int i = 0; i < QTD_ATIRADORES; i++) {
                    if (atiradores[i].vivo) {
                        ALLEGRO_BITMAP* img_atual;
                        if (atiradores[i].pose_atirando_ativa) img_atual = atiradores[i].img_atirando;
                        else img_atual = atiradores[i].img_parado;

                        if (img_atual) {
                            int flags = 0;
                            if (pos_x < atiradores[i].x) flags = ALLEGRO_FLIP_HORIZONTAL;
                            al_draw_scaled_bitmap(img_atual, 0, 0, al_get_bitmap_width(img_atual), al_get_bitmap_height(img_atual),
                                atiradores[i].x, atiradores[i].y, atiradores[i].w, atiradores[i].h, flags);
                        }
                    }
                    if (atiradores[i].bala.ativo && atiradores[i].img_projetil) {
                        int tamanho_tiro = 48;
                        al_draw_scaled_bitmap(atiradores[i].img_projetil, 0, 0, al_get_bitmap_width(atiradores[i].img_projetil), al_get_bitmap_height(atiradores[i].img_projetil),
                            atiradores[i].bala.x - tamanho_tiro / 2, atiradores[i].bala.y - tamanho_tiro / 2, tamanho_tiro, tamanho_tiro, 0);
                    }
                }

                if (estado_atual == ANDANDO) {
                    if (direcao_personagem == DIREITA) al_draw_scaled_bitmap(sprite_andar_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, PLAYER_W, PLAYER_H, 0);
                    else al_draw_scaled_bitmap(sprite_andar_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, PLAYER_W, PLAYER_H, 0);
                }
                else {
                    if (direcao_personagem == DIREITA) al_draw_scaled_bitmap(sprite_parado_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, PLAYER_W, PLAYER_H, 0);
                    else al_draw_scaled_bitmap(sprite_parado_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, PLAYER_W, PLAYER_H, 0);
                }

    
                if (mostrar_popup_tempo) {
                    al_draw_text(font2, al_map_rgb(255, 215, 0), popup_x_fixo, popup_y_flutuante, 0, "+15s");
                }
            }

            al_flip_display();
        }
    }


    al_destroy_bitmap(bg); al_destroy_bitmap(sprite_andar_direita); al_destroy_bitmap(sprite_andar_esquerda);
    al_destroy_font(font1); al_destroy_font(font2); al_destroy_event_queue(event_queue);
    al_destroy_bitmap(sprite_parado_esquerda); al_destroy_bitmap(sprite_parado_direita);
    al_destroy_bitmap(tile_piso); al_destroy_bitmap(img_cubo); al_destroy_font(font_cubo);
    al_destroy_bitmap(img_cubo_inativo); al_destroy_bitmap(sprite_inimigo_sheet);
    al_destroy_bitmap(bmp_inimigo_parado); al_destroy_bitmap(bmp_inimigo_atirando); al_destroy_bitmap(bmp_tiro);
    al_destroy_bitmap(bmp_relogio);
    al_destroy_bitmap(img_tutorial);

    return 0;
}