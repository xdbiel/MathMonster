#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/keyboard.h>
#include <stdio.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
enum Direcao { ESQUERDA, DIREITA };
enum EstadoPersonagem { PARADO, ANDANDO };

void run_fase2_screen(ALLEGRO_DISPLAY* display) {
    // ---INICIAR OS ADDONS PARA UTILIZAR NO CODIGO----
    al_init();
    al_init_font_addon(); //addon fonte
    al_init_ttf_addon(); //addon para fonte formato TTF
    al_init_image_addon(); //addon para imagens
    al_install_keyboard(); //addon para funcionalidades do teclado₢

    al_set_window_position(display, 300, 300);
    al_set_window_title(display, "World traveler");
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

    float vel_y = 0.0f; // Velocidade vertical, começa em 0
    const float GRAVIDADE = 0.5f; // Força da gravidade. Ajuste esse valor para uma queda mais rápida ou lenta.

    const float FORCA_PULO = -8.0f; // A força do pulo. É NEGATIVA porque o eixo Y cresce para baixo.
    bool esta_no_chao = false;       // Começa como 'falso' porque o personagem começa caindo.

    bool tecla_direita_pressionada = false;
    bool tecla_esquerda_pressionada = false;

    bool rodando = true;
    bool redraw = true; // Para controlar quando desenhar

    while (rodando) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }
        // Verificamos se o evento é de uma tecla SENDO PRESSIONADA
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {

            // Agora, aqui dentro, verificamos QUAL tecla foi
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                tecla_direita_pressionada = true;

            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                tecla_esquerda_pressionada = true;

            }

            // O pulo também é um evento de tecla pressionada
            else if (event.keyboard.keycode == ALLEGRO_KEY_UP) { // Usando a seta para cima para pular
                if (esta_no_chao) {
                    vel_y = FORCA_PULO;
                    esta_no_chao = false;
                }
            }
        }
        // Verificamos se o evento é de uma tecla SENDO SOLTA
        else if (event.type == ALLEGRO_EVENT_KEY_UP) {

            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                tecla_direita_pressionada = false;
            }
            else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                tecla_esquerda_pressionada = false;
            }
            // --- LÓGICA DE MOVIMENTO CONTÍNUO E ESTADO ---
// Esta seção roda 60 vezes por segundo

// Se QUALQUER tecla de movimento estiver pressionada

        }
        if (tecla_direita_pressionada || tecla_esquerda_pressionada) {
            estado_atual = ANDANDO;

            // Move o personagem de acordo com a tecla
            if (tecla_direita_pressionada) {
                pos_x += 3;
                direcao_personagem = DIREITA;
            }
            if (tecla_esquerda_pressionada) {
                pos_x -= 3;
                direcao_personagem = ESQUERDA;
            }
        }
        // SENÃO (se nenhuma tecla de movimento estiver pressionada)
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
        // --- LÓGICA DE MOVIMENTO CONTÍNUO (baseado no estado das teclas) ---
// Esta seção roda 60 vezes por segundo, lendo o estado dos "interruptores"


        // --- LÓGICA DA GRAVIDADE ---
        // 1. A gravidade afeta a velocidade vertical
        vel_y += GRAVIDADE;

        // 2. A velocidade vertical afeta a posição do personagem
        pos_y += vel_y;
        // -------------------------

        // --- LÓGICA DE DESENHO ---
        pos_y += vel_y;

        // --- CHÃO PROVISÓRIO ---
        // Se o personagem atingir ou passar da altura do chão (ex: 400 pixels)
        if (pos_y >= 400) {
            vel_y = 0;    // Zera a velocidade vertical para ele parar de acelerar
            pos_y = 400;  // Garante que ele fique exatamente no chão, e não um pouco abaixo
            esta_no_chao = true; // NOVO: Agora o programa sabe que o personagem está no chão
        }
        // ----------------------
        // ADICIONE ESTA LINHA PARA DEPURAR
        printf("Estado Atual: %d\n", estado_atual);

        al_clear_to_color(al_map_rgb(173, 216, 216));
        al_clear_to_color(al_map_rgb(173, 216, 216));
        al_clear_to_color(al_map_rgb(173, 216, 216));
        al_draw_bitmap(bg, 0, 0, 0);
        al_draw_text(font1, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA / 2) - (al_get_font_line_height(font1) / 2), ALLEGRO_ALIGN_CENTER, "MOVIMENTACAO EM DESENVOLVIMENTO!!");
        al_draw_text(font2, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, (ALTURA_TELA / 4) - (al_get_font_line_height(font2) / 2), ALLEGRO_ALIGN_CENTER, "TENTE USAR SETA PARA DIREITA E SETA PARA ESQUERDA!");
        // --- LÓGICA DE DESENHO DO PERSONAGEM ---

// PRIMEIRO, verificamos se o personagem está ANDANDO
        if (estado_atual == ANDANDO) {
            // Se ele está andando, fazemos o que já fazíamos antes:
            // verificamos a direção para desenhar o sprite de CAMINHADA correto.
            frame += 0.08f; // Velocidade da caminhada
            if (frame >= 6) frame = 0;

            if (direcao_personagem == DIREITA) {
                al_draw_bitmap_region(sprite_andar_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
            }
            else {
                al_draw_bitmap_region(sprite_andar_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
            }
        }
        // SENÃO, se o personagem estiver PARADO
        else if (estado_atual == PARADO) {
            // Aqui, usamos os novos sprites que você carregou!
            // A lógica de animação com a variável 'frame' funcionará aqui também.
            frame += 0.01f; // Pode ter uma velocidade mais lenta, fica mais natural
            if (frame >= 4) frame = 0; // O LIMITE AQUI É 4!

            if (direcao_personagem == DIREITA) {
                al_draw_bitmap_region(sprite_parado_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
            }
            else {
                // Se quiser usar a inversão por código que conversamos, seria aqui:
                // al_draw_bitmap_region(sprite_parado_direita, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, ALLEGRO_FLIP_HORIZONTAL);

                // Ou usando o segundo arquivo que você carregou:
                al_draw_bitmap_region(sprite_parado_esquerda, 32 * (int)frame, 0, 32, 32, pos_x, pos_y, 0);
            }
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

    return 0;
}