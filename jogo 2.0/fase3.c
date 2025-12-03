

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
#include "fase3.h"

#define LARGURA_TELA 1366
#define ALTURA_TELA 768
#define PLAYER_W 52
#define PLAYER_H 52


struct Plataforma { int x, y, w, h; };

#define QTD_PLATAFORMAS 6
struct Plataforma plataformas[QTD_PLATAFORMAS] = {
    {200, 680, 1000, 50},  
    {583, 580, 200, 30},   
    {290, 500, 150, 30},   
    {900, 500, 150, 30},    
    {90,  420, 150, 30}, 
    {1100, 420, 150, 30}    
};

enum EstadosChefe { VOANDO_ATIRANDO, CAINDO, ATORDOADO, RECUPERANDO, CHUVA, RESPIRO };


struct Projetil {
    float x, y, vel_x, vel_y;
    bool ativo;
    int raio;
};

struct Minion {
    float x, y, vel_x, vel_y;
    bool ativo;
    int frame_timer;
    int frame_atual;
};

struct Relogio {
    float x, y;
    bool ativo;
    int w, h;
    int frame_atual;
    int frame_timer;
    int src_w, src_h;
};

struct Atirador {
    float x, y;
    int w, h;
    bool ativo;
    int cooldown;
    struct Projetil bala;
    bool atirando;
    int timer_pose;
    ALLEGRO_BITMAP* img_parado;
    ALLEGRO_BITMAP* img_atirando;
    ALLEGRO_BITMAP* img_projetil;
};

struct Boss {
    float x, y;
    int w, h;
    int vida;
    enum EstadosChefe estado;
    int timer_estado;
    int cooldown_tiro;
    int cooldown_spawn; 
    int cooldown_chuva; 
    int duracao_chuva;  
    
    float velocidade_x;
    ALLEGRO_BITMAP* sprite;
    
    float tempo_resposta;
    bool fase_resposta_ativa;
};

struct ProblemaMatematico {
    int num1, num2, resp_certa, resp_errada;
    char texto_pergunta[32];
    char texto_resp_certa[8];
    char texto_resp_errada[8];
};


#define MAX_MINIONS 3
struct Minion minions[MAX_MINIONS];

struct Atirador atirador_minion; 

#define MAX_CHUVA 100
struct Projetil chuva[MAX_CHUVA];

#define MAX_RELOGIOS 2
struct Relogio relogios[MAX_RELOGIOS];



void spawnar_inimigos(float player_x) {
    for (int i = 0; i < MAX_MINIONS; i++) {
        minions[i].ativo = true;
        minions[i].frame_timer = 0;
        int tentativas = 0; bool safe = false;
        while (!safe && tentativas < 10) {
            int p = (rand() % 5) + 1; 
            float sx = plataformas[p].x + (rand() % (plataformas[p].w - 40));
            float sy = plataformas[p].y - 40;
            if (abs(sx - player_x) > 250) { minions[i].x = sx; minions[i].y = sy; safe = true; }
            tentativas++;
        }
        if (!safe) { minions[i].x = 683; minions[i].y = 50; }
        minions[i].vel_x = 0; minions[i].vel_y = 0;
    }

    atirador_minion.ativo = true;
    atirador_minion.w = 80; atirador_minion.h = 80;
    atirador_minion.cooldown = 60;
    atirador_minion.bala.ativo = false; atirador_minion.bala.raio = 6;
    atirador_minion.atirando = false; atirador_minion.timer_pose = 0;
    int t = 0; bool s = false;
    while (!s && t < 15) {
        int p = (rand() % 6); 
        float sx = plataformas[p].x + (rand() % (plataformas[p].w - 80));
        float sy = plataformas[p].y - 80;
        if (abs(sx - player_x) > 300) { atirador_minion.x = sx; atirador_minion.y = sy; s = true; }
        t++;
    }
    if (!s) { atirador_minion.x = 100; atirador_minion.y = 600; }
}

void spawnar_relogio_aleatorio() {
    for(int i=0; i<MAX_RELOGIOS; i++) {
        if(!relogios[i].ativo) {
            relogios[i].ativo = true;
            relogios[i].w = 40; relogios[i].h = 40;
            relogios[i].src_w = 166; relogios[i].src_h = 170;
            relogios[i].frame_atual = 0;
            int p = (rand() % 5) + 1; 
            relogios[i].x = plataformas[p].x + (rand() % (plataformas[p].w - 40));
            relogios[i].y = plataformas[p].y - 50;
            break; 
        }
    }
}

int contar_inimigos_vivos() {
    int vivos = 0;
    for (int i = 0; i < MAX_MINIONS; i++) if (minions[i].ativo) vivos++;
    if (atirador_minion.ativo) vivos++;
    return vivos;
}

struct ProblemaMatematico gerar_problema_boss() {
    struct ProblemaMatematico p;
    p.num1 = rand() % 50; p.num2 = rand() % 50;
    p.resp_certa = p.num1 + p.num2;
    p.resp_errada = p.resp_certa + (rand() % 10) + 1;
    sprintf(p.texto_pergunta, "BOSS: %d + %d = ?", p.num1, p.num2);
    sprintf(p.texto_resp_certa, "%d", p.resp_certa);
    sprintf(p.texto_resp_errada, "%d", p.resp_errada);
    return p;
}

int run_fase3_screen(ALLEGRO_DISPLAY* display) {
    al_set_window_title(display, "MathMonster - FINAL BOSS");


    ALLEGRO_BITMAP* sprite_andar_dir = al_load_bitmap("personagem_movimento_direita.png");
    ALLEGRO_BITMAP* sprite_andar_esq = al_load_bitmap("personagem_movimento_esquerda.png");
    ALLEGRO_BITMAP* sprite_parado_dir = al_load_bitmap("personagem_parado_direita.png");
    ALLEGRO_BITMAP* sprite_parado_esq = al_load_bitmap("personagem_parado_esquerda.png");
    ALLEGRO_BITMAP* bg = al_load_bitmap("bg2.png"); 
    ALLEGRO_BITMAP* tile_piso = al_load_bitmap("dungeon_piso_50x15.png");
    ALLEGRO_BITMAP* img_cubo = al_load_bitmap("bloco_resposta_selecionado.png");

    ALLEGRO_BITMAP* bmp_boss = al_load_bitmap("boss_idle_esquerda.png");
    ALLEGRO_BITMAP* bmp_tiro = al_load_bitmap("inimigo1_tiro.png");
    ALLEGRO_BITMAP* sprite_minion = al_load_bitmap("sprite-enemy.png");
    
    ALLEGRO_BITMAP* img_ati_parado = al_load_bitmap("inimigo1_parado.png");
    ALLEGRO_BITMAP* img_ati_acao = al_load_bitmap("inimigo1_atirando.png");
    ALLEGRO_BITMAP* bmp_relogio = al_load_bitmap("relogio_sprite.png");
    ALLEGRO_BITMAP* bmp_coracao = al_load_bitmap("coracao_sprite.png");

    ALLEGRO_FONT* font_big = al_load_ttf_font("joystix.ttf", 40, 0);
    ALLEGRO_FONT* font_small = al_load_ttf_font("joystix.ttf", 20, 0);
    ALLEGRO_FONT* font_cubo = al_load_ttf_font("joystix.ttf", 13, 0);

    if (!sprite_andar_dir || !bmp_boss || !bmp_tiro || !sprite_minion || !img_ati_parado || !bmp_relogio || !bmp_coracao) {
        printf("Erro assets fase 3\n"); return 0;
    }
    
    atirador_minion.img_parado = img_ati_parado;
    atirador_minion.img_atirando = img_ati_acao;
    atirador_minion.img_projetil = bmp_tiro;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_start_timer(timer);


    bool rodando = true, redesenhar = true;
    bool key_dir = false, key_esq = false;
    float px = 220, py = 600, vy = 0;
    const float GRAVIDADE = 0.5f;
    const float FORCA_PULO = -10.0f;
    bool no_chao = true;
    int dir_player = 1;
    float frame_player = 0;

    struct Boss boss;
    boss.w = 150; boss.h = 150;
    boss.x = LARGURA_TELA / 2 - boss.w / 2; boss.y = 50;
    boss.vida = 3;
    boss.estado = VOANDO_ATIRANDO;
    boss.timer_estado = 0;
    boss.cooldown_tiro = 0;
    boss.cooldown_spawn = 0;
    boss.cooldown_chuva = 0;
    boss.duracao_chuva = 0;
    boss.velocidade_x = 3.0f;
    boss.sprite = bmp_boss;
    boss.fase_resposta_ativa = false;
    boss.tempo_resposta = 10.0f;

    struct Projetil tiro_boss;
    tiro_boss.ativo = false; tiro_boss.raio = 15;

    for (int i = 0; i < MAX_MINIONS; i++) minions[i].ativo = false;
    for (int i = 0; i < MAX_CHUVA; i++) chuva[i].ativo = false;
    for (int i = 0; i < MAX_RELOGIOS; i++) relogios[i].ativo = false;
    atirador_minion.ativo = false;
    
    spawnar_inimigos(px); 

    struct ProblemaMatematico questao = gerar_problema_boss();
    int cubo_esq_x = plataformas[4].x + 50, cubo_esq_y = plataformas[4].y - 50;
    int cubo_dir_x = plataformas[5].x + 50, cubo_dir_y = plataformas[5].y - 50;
    bool resp_certa_esq = (rand() % 2 == 0);
    
    bool matou_neste_frame = false;
    float tempo_geral = 60.0f;

    while (rodando) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
            matou_neste_frame = false;
            
            if (tempo_geral > 0) tempo_geral -= 1.0/60.0;
            if (tempo_geral <= 0) {
                 al_destroy_timer(timer); al_destroy_event_queue(queue); return 2;
            }


            if (boss.vida > 0) {
                

                if (boss.estado == VOANDO_ATIRANDO) {
                    if (boss.x + boss.w / 2 < px + PLAYER_W / 2) boss.x += boss.velocidade_x;
                    else boss.x -= boss.velocidade_x;
                    boss.y = 50 + sin(al_get_time() * 2) * 30;

                    int vivos = contar_inimigos_vivos();
                    if (vivos > 0) {
                        boss.fase_resposta_ativa = false;
                        boss.tempo_resposta = 10.0f;
                    } else {
                        boss.fase_resposta_ativa = true;
                        boss.tempo_resposta -= 1.0 / 60.0;
                        if (boss.tempo_resposta <= 0) {
                            spawnar_inimigos(px);
                            boss.tempo_resposta = 10.0f;
                            boss.fase_resposta_ativa = false;
                        }
                    }

                    boss.cooldown_tiro++;
                    int dificuldade = 100 - ((3 - boss.vida) * 20);
                    if (boss.cooldown_tiro > dificuldade) {
                        if (!tiro_boss.ativo) {
                            tiro_boss.ativo = true;
                            tiro_boss.x = boss.x + boss.w / 2;
                            tiro_boss.y = boss.y + boss.h;
                            float dx = (px + PLAYER_W / 2) - tiro_boss.x;
                            float dy = (py + PLAYER_H / 2) - tiro_boss.y;
                            float h = sqrt(dx * dx + dy * dy);
                            tiro_boss.vel_x = (dx / h) * 7.0f;
                            tiro_boss.vel_y = (dy / h) * 7.0f;
                            boss.cooldown_tiro = 0;
                        }
                    }
                }

                else if (boss.estado == CAINDO) {
                    boss.y += 6.0f;
                    if (boss.y + boss.h >= plataformas[0].y) {
                        boss.y = plataformas[0].y - boss.h;
                        boss.estado = ATORDOADO;
                        boss.timer_estado = 0;
                        tiro_boss.ativo = false;
                        for (int i = 0; i < MAX_MINIONS; i++) minions[i].ativo = false;
                        atirador_minion.ativo = false;
                    }
                }

                else if (boss.estado == ATORDOADO) {
                    boss.timer_estado++;
                    if (boss.timer_estado > 300) boss.estado = RECUPERANDO;

                    if (!matou_neste_frame && vy > 0 && px + PLAYER_W > boss.x && px < boss.x + boss.w &&
                        py + PLAYER_H > boss.y && py + PLAYER_H < boss.y + 50) {
                        boss.vida--;
                        vy = -15.0f;
                        matou_neste_frame = true;
                        
                        if (boss.vida <= 0) { 
                            al_destroy_timer(timer); al_destroy_event_queue(queue); return 1; 
                        }
                        else {
                            boss.estado = RECUPERANDO; 
                        }
                    }
                }

                else if (boss.estado == RECUPERANDO) {
                    boss.y -= 5.0f;
                    if (boss.y <= 50) {
                        if (boss.vida < 3) {
                            boss.estado = CHUVA;
                            boss.duracao_chuva = 600; 
                            boss.cooldown_chuva = 0;
                            boss.x = LARGURA_TELA/2 - boss.w/2; 
                        } else {
                            boss.estado = VOANDO_ATIRANDO;
                            spawnar_inimigos(px);
                            questao = gerar_problema_boss();
                            resp_certa_esq = (rand() % 2 == 0);
                        }
                    }
                }

                else if (boss.estado == CHUVA) {
                    boss.duracao_chuva--;
                    boss.x = LARGURA_TELA/2 - boss.w/2;
                    boss.y = 50;

                    boss.cooldown_chuva++;
                    if (boss.cooldown_chuva >= 40) {
                        boss.cooldown_chuva = 0;
                        int gap_start = rand() % (LARGURA_TELA - 200);
                        int gap_width = 150; 

                        for (int x = 0; x < LARGURA_TELA; x += 100) { 
                            if (x < gap_start || x > gap_start + gap_width) {
                                for(int k=0; k<MAX_CHUVA; k++) {
                                    if(!chuva[k].ativo) {
                                        chuva[k].ativo = true;
                                        chuva[k].x = x;
                                        chuva[k].y = -20;
                                        chuva[k].vel_x = 0;
                                        chuva[k].vel_y = 5.0f + (rand()%3);
                                        chuva[k].raio = 10;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    
                    if (boss.duracao_chuva % 150 == 0) spawnar_relogio_aleatorio();

                    if (boss.duracao_chuva <= 0) {
                        boss.estado = RESPIRO;
                        boss.timer_estado = 0;
                        for(int k=0; k<MAX_CHUVA; k++) chuva[k].ativo = false;
                    }
                }

                else if (boss.estado == RESPIRO) {
                    boss.timer_estado++;
                    if (boss.timer_estado >= 90) {
                        boss.estado = VOANDO_ATIRANDO;
                        spawnar_inimigos(px);
                        questao = gerar_problema_boss();
                        resp_certa_esq = (rand() % 2 == 0);
                    }
                }
            }


            for(int k=0; k<MAX_CHUVA; k++) {
                if(chuva[k].ativo) {
                    chuva[k].y += chuva[k].vel_y;
                    if (chuva[k].x > px && chuva[k].x < px + PLAYER_W && 
                        chuva[k].y > py && chuva[k].y < py + PLAYER_H) {
                        al_destroy_timer(timer); al_destroy_event_queue(queue); return 2;
                    }
                    if (chuva[k].y > ALTURA_TELA) chuva[k].ativo = false;
                }
            }


            for(int k=0; k<MAX_RELOGIOS; k++) {
                if(relogios[k].ativo) {
                    relogios[k].frame_timer++;
                    if(relogios[k].frame_timer >= 25) {
                        relogios[k].frame_timer = 0; relogios[k].frame_atual++;
                        if(relogios[k].frame_atual >= 6) relogios[k].frame_atual = 0;
                    }
                    if (px+PLAYER_W > relogios[k].x && px < relogios[k].x + relogios[k].w &&
                        py+PLAYER_H > relogios[k].y && py < relogios[k].y + relogios[k].h) {
                        relogios[k].ativo = false;
                        tempo_geral += 15.0f; 
                    }
                }
            }

   
            for (int i = 0; i < MAX_MINIONS; i++) {
                if (minions[i].ativo) {
                    float speed = 2.0f;
                    if (minions[i].x < px) minions[i].x += speed; else minions[i].x -= speed;
                    if (minions[i].y < py) minions[i].y += speed; else minions[i].y -= speed;

                    minions[i].frame_timer++;
                    if (minions[i].frame_timer >= 10) {
                        minions[i].frame_timer = 0; minions[i].frame_atual++;
                        if (minions[i].frame_atual >= 7) minions[i].frame_atual = 0;
                    }

                    float mx = minions[i].x + 10, my = minions[i].y + 5, mw = 23, mh = 15;
                    if (px + PLAYER_W > mx && px < mx + mw && py + PLAYER_H > my && py < my + mh) {
                        if (!matou_neste_frame && vy > 0 && (py + PLAYER_H) < (my + mh + 5)) {
                            minions[i].ativo = false; vy = -12.0f; matou_neste_frame = true;
                        }
                        else if (!matou_neste_frame) {
                            al_destroy_timer(timer); al_destroy_event_queue(queue); return 2;
                        }
                    }
                }
            }


            if (atirador_minion.ativo) {
                if (atirador_minion.atirando) {
                    atirador_minion.timer_pose++;
                    if (atirador_minion.timer_pose > 30) {
                        atirador_minion.atirando = false; atirador_minion.timer_pose = 0;
                    }
                }
                atirador_minion.cooldown++;
                if (atirador_minion.cooldown >= 120) {
                    if (!atirador_minion.bala.ativo) {
                        atirador_minion.atirando = true; atirador_minion.timer_pose = 0;
                        atirador_minion.bala.ativo = true;
                        atirador_minion.bala.x = atirador_minion.x + 40;
                        atirador_minion.bala.y = atirador_minion.y + 40;
                        float dx = (px+PLAYER_W/2) - atirador_minion.bala.x;
                        float dy = (py+PLAYER_H/2) - atirador_minion.bala.y;
                        float h = sqrt(dx * dx + dy * dy);
                        atirador_minion.bala.vel_x = (dx / h) * 5.0f; atirador_minion.bala.vel_y = (dy / h) * 5.0f;
                        atirador_minion.cooldown = 0;
                    }
                }
                float ax = atirador_minion.x + 20, ay = atirador_minion.y + 10, aw = 40, ah = 60;
                if (px + PLAYER_W > ax && px < ax + aw && py + PLAYER_H > ay && py < ay + ah) {
                    if (!matou_neste_frame && vy > 0 && (py + PLAYER_H) < (ay + ah / 2)) {
                        atirador_minion.ativo = false; vy = -12.0f; matou_neste_frame = true;
                    }
                    else if (!matou_neste_frame) {
                        al_destroy_timer(timer); al_destroy_event_queue(queue); return 2;
                    }
                }
            }
            if (atirador_minion.bala.ativo) {
                atirador_minion.bala.x += atirador_minion.bala.vel_x;
                atirador_minion.bala.y += atirador_minion.bala.vel_y;
                if (atirador_minion.bala.x > px && atirador_minion.bala.x < px+PLAYER_W &&
                    atirador_minion.bala.y > py && atirador_minion.bala.y < py+PLAYER_H) {
                    al_destroy_timer(timer); al_destroy_event_queue(queue); return 2;
                }
                if (atirador_minion.bala.x < 0 || atirador_minion.bala.x > LARGURA_TELA) atirador_minion.bala.ativo = false;
            }


            if (tiro_boss.ativo && boss.estado != CHUVA && boss.estado != RESPIRO) {
                tiro_boss.x += tiro_boss.vel_x;
                tiro_boss.y += tiro_boss.vel_y;
                if (tiro_boss.x > px && tiro_boss.x < px+PLAYER_W &&
                    tiro_boss.y > py && tiro_boss.y < py+PLAYER_H) {
                    al_destroy_timer(timer); al_destroy_event_queue(queue); return 2;
                }
                if (tiro_boss.x < 0 || tiro_boss.x > LARGURA_TELA) tiro_boss.ativo = false;
            }


            if (key_dir) { px += 5; dir_player = 1; frame_player += 0.15; }
            else if (key_esq) { px -= 5; dir_player = -1; frame_player += 0.15; }
            else { frame_player = 0; }
            if (frame_player >= 6) frame_player = 0;
            vy += GRAVIDADE; py += vy;
            if (py > ALTURA_TELA) { al_destroy_timer(timer); al_destroy_event_queue(queue); return 2; }

            no_chao = false;
            int pe_y = py + PLAYER_H; int margem = 10;
            if(vy >= 0) {
                for(int i=0; i<QTD_PLATAFORMAS; i++) {
                    struct Plataforma p = plataformas[i];
                    if(pe_y >= p.y && pe_y <= p.y + vy + 5 &&
                       px + PLAYER_W - margem > p.x && px + margem < p.x + p.w) {
                        vy = 0; py = p.y - PLAYER_H; no_chao = true;
                    }
                }
            }
            if (px < 0) px = 0; if (px > LARGURA_TELA - PLAYER_W) px = LARGURA_TELA - PLAYER_W;


            if (boss.estado == VOANDO_ATIRANDO && boss.fase_resposta_ativa) {
                if (px + PLAYER_W > cubo_esq_x && px < cubo_esq_x + 50 && py + PLAYER_H > cubo_esq_y && py < cubo_esq_y + 50) {
                    if (resp_certa_esq) { boss.estado = CAINDO; tiro_boss.ativo = false; }
                    else { al_destroy_timer(timer); al_destroy_event_queue(queue); return 2; }
                }
                if (px + PLAYER_W > cubo_dir_x && px < cubo_dir_x + 50 && py + PLAYER_H > cubo_dir_y && py < cubo_dir_y + 50) {
                    if (!resp_certa_esq) { boss.estado = CAINDO; tiro_boss.ativo = false; }
                    else { al_destroy_timer(timer); al_destroy_event_queue(queue); return 2; }
                }
            }

        } else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) key_dir = true;
            if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) key_esq = true;
            if (ev.keyboard.keycode == ALLEGRO_KEY_UP && no_chao) vy = FORCA_PULO;
        } else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_RIGHT) key_dir = false;
            if (ev.keyboard.keycode == ALLEGRO_KEY_LEFT) key_esq = false;
        } else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) return 0;

        if (redesenhar && al_is_event_queue_empty(queue)) {
            redesenhar = false;
            al_clear_to_color(al_map_rgb(20, 10, 10));
            al_draw_scaled_bitmap(bg, 0, 0, al_get_bitmap_width(bg), al_get_bitmap_height(bg), 0, 0, LARGURA_TELA, ALTURA_TELA, 0);

            if (tile_piso) {
                for (int i = 0; i < QTD_PLATAFORMAS; i++)
                    for (int k = 0; k < plataformas[i].w; k += 50) al_draw_bitmap(tile_piso, plataformas[i].x + k, plataformas[i].y, 0);
            }

   
            for (int i = 0; i < MAX_MINIONS; i++) {
                if (minions[i].ativo) {
                    int flags_m = (minions[i].vel_x < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
                    al_draw_bitmap_region(sprite_minion, minions[i].frame_atual * 43, 0, 43, 25, minions[i].x, minions[i].y, flags_m);
                }
            }


            if (atirador_minion.ativo) {
                ALLEGRO_BITMAP* spr = atirador_minion.atirando ? atirador_minion.img_atirando : atirador_minion.img_parado;
                int flip = (px < atirador_minion.x) ? ALLEGRO_FLIP_HORIZONTAL : 0;
                al_draw_scaled_bitmap(spr, 0, 0, al_get_bitmap_width(spr), al_get_bitmap_height(spr),
                    atirador_minion.x, atirador_minion.y, atirador_minion.w, atirador_minion.h, flip);
            }
            if (atirador_minion.bala.ativo) {
                al_draw_scaled_bitmap(atirador_minion.img_projetil, 0, 0, al_get_bitmap_width(atirador_minion.img_projetil), al_get_bitmap_height(atirador_minion.img_projetil),
                    atirador_minion.bala.x - 24, atirador_minion.bala.y - 24, 48, 48, 0);
            }


            for(int k=0; k<MAX_CHUVA; k++) {
                if(chuva[k].ativo) {
                    al_draw_filled_circle(chuva[k].x, chuva[k].y, chuva[k].raio, al_map_rgb(139, 7, 0));
                }
            }

            for(int k=0; k<MAX_RELOGIOS; k++) {
                if(relogios[k].ativo) {
                    int col = relogios[k].frame_atual % 3; int row = relogios[k].frame_atual / 3;
                    al_draw_scaled_bitmap(bmp_relogio, col*relogios[k].src_w, row*relogios[k].src_h, relogios[k].src_w, relogios[k].src_h, relogios[k].x, relogios[k].y, relogios[k].w, relogios[k].h, 0);
                }
            }


            if (boss.estado == VOANDO_ATIRANDO) {
                if (boss.fase_resposta_ativa) {
                    al_draw_bitmap(img_cubo, cubo_esq_x, cubo_esq_y, 0);
                    al_draw_bitmap(img_cubo, cubo_dir_x, cubo_dir_y, 0);
                    int off_y = 15;
                    if (resp_certa_esq) {
                        al_draw_text(font_cubo, al_map_rgb(255, 255, 255), cubo_esq_x + 25, cubo_esq_y + off_y, ALLEGRO_ALIGN_CENTER, questao.texto_resp_certa);
                        al_draw_text(font_cubo, al_map_rgb(255, 255, 255), cubo_dir_x + 25, cubo_dir_y + off_y, ALLEGRO_ALIGN_CENTER, questao.texto_resp_errada);
                    }
                    else {
                        al_draw_text(font_cubo, al_map_rgb(255, 255, 255), cubo_esq_x + 25, cubo_esq_y + off_y, ALLEGRO_ALIGN_CENTER, questao.texto_resp_errada);
                        al_draw_text(font_cubo, al_map_rgb(255, 255, 255), cubo_dir_x + 25, cubo_dir_y + off_y, ALLEGRO_ALIGN_CENTER, questao.texto_resp_certa);
                    }
                    char txt_corre[32]; sprintf(txt_corre, "CORRA: %.1fs", boss.tempo_resposta);
                    al_draw_text(font_big, al_map_rgb(255, 100, 100), LARGURA_TELA / 2, 120, ALLEGRO_ALIGN_CENTER, txt_corre);
                }
            }

            ALLEGRO_COLOR cor_boss = (boss.estado == ATORDOADO) ? al_map_rgb(100, 100, 255) : al_map_rgb(255, 100, 100);
            if (boss.estado == CHUVA) cor_boss = al_map_rgb(255, 50, 50); 

            al_draw_tinted_scaled_bitmap(boss.sprite, cor_boss, 0, 0, al_get_bitmap_width(boss.sprite), al_get_bitmap_height(boss.sprite),
                boss.x, boss.y, boss.w, boss.h, 0);


            int h_w = 40; 
            int h_h = 40;
            int total_w = boss.vida * (h_w + 10);
            int start_x = (LARGURA_TELA / 2) - (total_w / 2);

            for (int i = 0; i < boss.vida; i++) {
                 al_draw_scaled_bitmap(bmp_coracao,
                     0, 0, al_get_bitmap_width(bmp_coracao), al_get_bitmap_height(bmp_coracao),
                     start_x + (i * (h_w + 10)), 20, 
                     h_w, h_h, 0);
            }
            
            char txt_time_g[32]; sprintf(txt_time_g, "TEMPO: %.2f", tempo_geral);
            al_draw_text(font_small, al_map_rgb(255, 255, 255), LARGURA_TELA - 20, 20, ALLEGRO_ALIGN_RIGHT, txt_time_g);

            if (boss.estado == VOANDO_ATIRANDO)
                al_draw_text(font_big, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 70, ALLEGRO_ALIGN_CENTER, questao.texto_pergunta);
            else if (boss.estado == ATORDOADO) {
        
                al_draw_text(font_small, al_map_rgb(255, 255, 255), boss.x + boss.w / 2, boss.y - 50, ALLEGRO_ALIGN_CENTER, "ATAQUE AGORA!");
            }

            if (tiro_boss.ativo && boss.estado != CHUVA && boss.estado != RESPIRO) al_draw_scaled_bitmap(bmp_tiro, 0, 0, al_get_bitmap_width(bmp_tiro), al_get_bitmap_height(bmp_tiro), tiro_boss.x - 24, tiro_boss.y - 24, 48, 48, 0);

            if (dir_player == 1) al_draw_scaled_bitmap(key_dir ? sprite_andar_dir : sprite_parado_dir, 32 * (int)frame_player, 0, 32, 32, px, py, PLAYER_W, PLAYER_H, 0);
            else al_draw_scaled_bitmap(key_esq ? sprite_andar_esq : sprite_parado_esq, 32 * (int)frame_player, 0, 32, 32, px, py, PLAYER_W, PLAYER_H, 0);

            al_flip_display();
        }
    }

    al_destroy_bitmap(bg); al_destroy_bitmap(sprite_andar_dir); al_destroy_bitmap(sprite_andar_esq);
    al_destroy_bitmap(sprite_parado_dir); al_destroy_bitmap(sprite_parado_esq);
    al_destroy_bitmap(tile_piso); al_destroy_bitmap(img_cubo); al_destroy_bitmap(img_ati_parado); al_destroy_bitmap(img_ati_acao);
    al_destroy_bitmap(bmp_boss); al_destroy_bitmap(bmp_tiro); al_destroy_bitmap(sprite_minion); al_destroy_bitmap(bmp_relogio); al_destroy_bitmap(bmp_coracao);
    al_destroy_font(font_big); al_destroy_font(font_small); al_destroy_font(font_cubo);
    al_destroy_timer(timer); al_destroy_event_queue(queue);

    return 0;
}