
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
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
    int pos_y = 448;
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
    bool redesenhar = true;
    const char* falas_tutorial[] = {
    "Bem-vindo ao Math Monster! Vamos aprender a jogar.",
    "Use as SETAS ESQUERDA e DIREITA para andar.",
    "Pressione a SETA PARA CIMA para pular.",
    "Cuidado! Se tocar no inimigo de lado, voce perde.",
    "Pule na CABECA do inimigo para derrota-lo.",
    "Apos derrotar o inimigo, os cubos irao ativar.",
    "Resolva a conta no topo da tela e pule na resposta correta!",
    "Boa sorte! Pressione ENTER para comecar."
    };

    int fala_atual = 0; // Qual frase estamos mostrando (0 é a primeira)
    int total_falas = 8; // Quantas frases existem no total acima
    static bool tutorial_ja_foi_visto = false;
    bool tutorial_ativo = !tutorial_ja_foi_visto;




    while (rodando) {

        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        // ---------------------------------------------------------
        // 1. LÓGICA DE ATUALIZAÇÃO (O CÉREBRO DO JOGO)
        // Tudo aqui roda exatamente 60 vezes por segundo
        // ---------------------------------------------------------
        if (event.type == ALLEGRO_EVENT_TIMER) {

            redesenhar = true;

            if (!tutorial_ativo) {

            // --- Lógica do Cronômetro ---
            if (tempo_restante > 0) {
                tempo_restante -= 1.0 / 60.0;
            }
            if (tempo_restante < 0) tempo_restante = 0;

            if (tempo_restante == 0) {
                printf("TEMPO ESGOTADO!\n");
                return 2; // Perdeu
            }

            // --- Lógica dos Inimigos (Ativação dos Cubos) ---
            if (inimigos_vivos == 0) {
                cubos_estao_ativos = true;
            }

            // --- Movimento do Inimigo (Morcego) ---
            if (morcego.esta_vivo) {
                morcego.x += morcego.vel_x;

                // IA Bate e Volta
                if (morcego.x <= plataforma_x) {
                    morcego.vel_x = 1.0;
                }
                if (morcego.x + morcego.frame_largura >= plataforma_x + plataforma_largura) {
                    morcego.vel_x = -1.0;
                }

                // Animação do Inimigo
                morcego.frame_timer++;
                if (morcego.frame_timer >= 10) {
                    morcego.frame_timer = 0;
                    morcego.frame_atual++;
                    if (morcego.frame_atual >= 7) morcego.frame_atual = 0;
                }
            }

            // --- Movimento do Jogador (Baseado nas teclas) ---
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

            // --- Animação do Jogador ---
            if (estado_atual == ANDANDO) {
                frame += 0.08f;
                if (frame >= 6) frame = 0;
            }
            else { // PARADO
                frame += 0.01f;
                if (frame >= 4) frame = 0;
            }

            // --- Física (Gravidade) ---
            vel_y += GRAVIDADE;
            pos_y += vel_y;

            // --- Colisões ---

            // 1. Colisão com o Bloco de Teleporte (Fim de fase falsa, só teste)
            if (pos_x + 32 > bloco_x && pos_x < bloco_x + bloco_largura &&
                pos_y + 32 > bloco_y && pos_y < bloco_y + bloco_altura) {
                printf("Colisão com o bloco! Passando de fase...\n");
                return 1;
            }

            // 2. Colisão com o Chão (Plataforma)
            int pe_personagem = pos_y + PLAYER_H;
            int topo_plataforma = plataforma_y;
            int lado_esquerdo_personagem = pos_x;
            int lado_direito_personagem = pos_x + PLAYER_W;
            int lado_esquerdo_plataforma = plataforma_x;
            int lado_direito_plataforma = plataforma_x + plataforma_largura;

            if (vel_y > 0 &&
                pe_personagem >= topo_plataforma &&
                pe_personagem <= topo_plataforma + vel_y &&
                lado_direito_personagem > lado_esquerdo_plataforma &&
                lado_esquerdo_personagem < lado_direito_plataforma)
            {
                vel_y = 0;
                pos_y = topo_plataforma - PLAYER_W;
                esta_no_chao = true;
            }
            else {
                esta_no_chao = false;
            }

            // 3. Colisão com o Inimigo
            if (morcego.esta_vivo) {
                // Definindo caixas de colisão temporárias
                int j_dir = pos_x + PLAYER_W; int j_esq = pos_x; int j_base = pos_y + PLAYER_H; int j_topo = pos_y;
                int i_dir = morcego.x + morcego.frame_largura; int i_esq = morcego.x; int i_base = morcego.y + morcego.frame_altura; int i_topo = morcego.y;

                if (j_dir > i_esq && j_esq < i_dir && j_base > i_topo && j_topo < i_base) {
                    // Stomp (Pulo na cabeça)
                    if (vel_y > 0 && (j_base < i_topo + vel_y + 10)) {
                        printf("INIMIGO DERROTADO!\n");
                        vel_y = FORCA_PULO * 0.75;
                        morcego.esta_vivo = false;
                        inimigos_vivos--;
                    }
                    else {
                        printf("COLISAO COM O INIMIGO! Game Over.\n");
                        return 2; // Perdeu
                    }
                }
            }

            // 4. Colisão com os Cubos
            if (cubos_estao_ativos) {
                // Cubo Certo
                if (pos_x + 32 > cubo_certo_x && pos_x < cubo_certo_x + cubo_largura &&
                    pos_y + 32 > cubo_certo_y && pos_y < cubo_certo_y + cubo_altura) {
                    if (resposta_certa_na_esquerda) return 1; else return 2;
                }
                // Cubo Errado
                if (pos_x + 32 > cubo_errado_x && pos_x < cubo_errado_x + cubo_largura &&
                    pos_y + 32 > cubo_errado_y && pos_y < cubo_errado_y + cubo_altura) {
                    if (!resposta_certa_na_esquerda) return 1; else return 2;
                }
            }

            // Limites da Tela
            if (pos_x < 0) pos_x = 0;
            if (pos_x + 32 > LARGURA_TELA) pos_x = LARGURA_TELA - 32;

            // Avisa que os dados mudaram e podemos desenhar
            redesenhar = true;
            }
        }
        // ---------------------------------------------------------
        // 2. LÓGICA DE EVENTOS (INPUTS)
        // Só altera as variáveis de controle, não move nada
        // ---------------------------------------------------------
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
                    fala_atual++; // Passa para a próxima fala
                    if (fala_atual >= total_falas) {
                        tutorial_ativo = false; // Acabaram as falas, fecha o tutorial
                        tutorial_ja_foi_visto = true;
                    }
                }
            }
        }
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) tecla_direita_pressionada = false;
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) tecla_esquerda_pressionada = false;
        }

        // ---------------------------------------------------------
        // 3. LÓGICA DE DESENHO
        // Só desenha se o timer mandou e se não tem eventos atrasados
        // ---------------------------------------------------------
        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = false;

            al_clear_to_color(al_map_rgb(173, 216, 216));
            al_draw_bitmap(bg, 0, 0, 0);
            al_draw_text(font1, al_map_rgb(255,255,255), LARGURA_TELA / 2, (ALTURA_TELA / 2) - (al_get_font_line_height(font1) / 2), ALLEGRO_ALIGN_CENTER, "FASE 1");

            // Desenha o Fundo da Equação e Texto
            if (img_fundo_equacao) {
                float feq_w = al_get_bitmap_width(img_fundo_equacao);
                float feq_h = al_get_bitmap_height(img_fundo_equacao);
                float feq_x = (LARGURA_TELA / 2) - 250 - (feq_w / 2);
                float feq_y = 15;
                al_draw_bitmap(img_fundo_equacao, feq_x, feq_y, 0);
                al_draw_text(font2, al_map_rgb(255, 255, 255), feq_x + (feq_w / 2), feq_y + (feq_h - al_get_font_line_height(font2)) / 2, ALLEGRO_ALIGN_CENTER, problema_atual.texto_pergunta);
            }

            // Desenha Timer
            char texto_timer[32];
            sprintf(texto_timer, "Tempo: %.2f", tempo_restante);
            al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA - 30, 30, ALLEGRO_ALIGN_RIGHT, texto_timer);

            // Desenha Plataforma (Tiled)
            if (tile_piso) {
                int tile_largura = 50;
                for (int x = plataforma_x; x < plataforma_x + plataforma_largura; x += tile_largura) {
                    al_draw_bitmap(tile_piso, x, plataforma_y, 0);
                }
            }

            // Desenha Cubos
            if (cubos_estao_ativos) {
                if (img_cubo) {
                    al_draw_bitmap(img_cubo, cubo_certo_x, cubo_certo_y, 0);
                    al_draw_bitmap(img_cubo, cubo_errado_x, cubo_errado_y, 0);

                    if (resposta_certa_na_esquerda) {
                        al_draw_text(font_cubo, al_map_rgb(0, 0, 0), cubo_certo_x + cubo_largura / 2, cubo_certo_y + (cubo_altura - al_get_font_line_height(font_cubo)) / 2 - 1, ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_certa);
                        al_draw_text(font_cubo, al_map_rgb(0, 0, 0), cubo_errado_x + cubo_largura / 2, cubo_errado_y + (cubo_altura - al_get_font_line_height(font_cubo)) / 2 - 1, ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_errada);
                    }
                    else {
                        al_draw_text(font_cubo, al_map_rgb(0, 0, 0), cubo_certo_x + cubo_largura / 2, cubo_certo_y + (cubo_altura - al_get_font_line_height(font_cubo)) / 2 - 1, ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_errada);
                        al_draw_text(font_cubo, al_map_rgb(0, 0, 0), cubo_errado_x + cubo_largura / 2, cubo_errado_y + (cubo_altura - al_get_font_line_height(font_cubo)) / 2 - 1, ALLEGRO_ALIGN_CENTER, problema_atual.texto_resposta_certa);
                    }
                }
            }
            else {
                al_draw_filled_rectangle(cubo_certo_x, cubo_certo_y, cubo_certo_x + cubo_largura, cubo_certo_y + cubo_altura, al_map_rgb(20, 20, 20));
                al_draw_filled_rectangle(cubo_errado_x, cubo_errado_y, cubo_errado_x + cubo_largura, cubo_errado_y + cubo_altura, al_map_rgb(20, 20, 20));
            }

            // Desenha Inimigo
            if (morcego.esta_vivo) {
                int flags_inimigo = (morcego.vel_x < 0) ? ALLEGRO_FLIP_HORIZONTAL : 0;
                al_draw_bitmap_region(morcego.spritesheet, morcego.frame_atual * morcego.frame_largura, 0, morcego.frame_largura, morcego.frame_altura, morcego.x, morcego.y, flags_inimigo);
            }

            // Desenha Personagem
            if (estado_atual == ANDANDO) {
                if (direcao_personagem == DIREITA) {
                    al_draw_scaled_bitmap(sprite_andar_direita,
                        32 * (int)frame, 0, 32, 32,  // Origem (corte de 32x32 na imagem)
                        pos_x, pos_y, PLAYER_W, PLAYER_H, // Destino (desenha 64x64 na tela)
                        0);
                }
                else {
                    al_draw_scaled_bitmap(sprite_andar_esquerda,
                        32 * (int)frame, 0, 32, 32,
                        pos_x, pos_y, PLAYER_W, PLAYER_H,
                        0);
                }
            }
            else { // PARADO
                if (direcao_personagem == DIREITA) {
                    // Lembre-se que seu sprite de parado tem 128 pixels de largura total e 4 frames
                    // Então cada frame tem 32 de largura (128/4 = 32)
                    al_draw_scaled_bitmap(sprite_parado_direita,
                        32 * (int)frame, 0, 32, 32,
                        pos_x, pos_y, PLAYER_W, PLAYER_H,
                        0);
                }
                else {
                    al_draw_scaled_bitmap(sprite_parado_esquerda,
                        32 * (int)frame, 0, 32, 32,
                        pos_x, pos_y, PLAYER_W, PLAYER_H,
                        0);
                }
            }
            // Desenha bloco de teste
            al_draw_filled_rectangle(bloco_x, bloco_y, bloco_x + bloco_largura, bloco_y + bloco_altura, al_map_rgb(255, 255, 0));
            if (tutorial_ativo) {
                // Configurações da caixa
                float caixa_x = 20;
                float caixa_y = ALTURA_TELA - 150; // Fica na parte de baixo
                float caixa_w = LARGURA_TELA - 40;
                float caixa_h = 130;

                // 1. Desenha o fundo PRETO (opaco)
                al_draw_filled_rectangle(caixa_x, caixa_y, caixa_x + caixa_w, caixa_y + caixa_h, al_map_rgba(0, 0, 0,170));

                // 2. Desenha a borda BRANCA (grossa, estilo retro)
                al_draw_rectangle(caixa_x, caixa_y, caixa_x + caixa_w, caixa_y + caixa_h, al_map_rgb(255, 255, 255), 4);

                // 3. Desenha o texto da fala atual
                // Usamos al_draw_multiline_text para ele quebrar a linha automaticamente se o texto for longo
                al_draw_multiline_text(font2, al_map_rgb(255, 255, 255),
                    caixa_x + 20, caixa_y + 20,
                    caixa_w - 20,
                    30, // Altura da linha
                    0,
                    falas_tutorial[fala_atual]);

                // 4. (Opcional) Uma seta piscando ou texto indicando para apertar Enter
                al_draw_text(font_cubo, al_map_rgb(255, 255, 255),
                    caixa_x + caixa_w - 20, caixa_y + caixa_h - 20,
                    ALLEGRO_ALIGN_RIGHT, "Aperte ENTER >");
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

    return 0;
}