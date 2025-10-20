#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "splash.h"

void run_splash_screen(ALLEGRO_DISPLAY* display) {

    // --- 1. CARREGAR RECURSOS ---
    ALLEGRO_BITMAP* frames_carregamento[4];
    frames_carregamento[0] = al_load_bitmap("menu_carregamento_0%.png");
    frames_carregamento[1] = al_load_bitmap("menu_carregamento_20%.png");
    frames_carregamento[2] = al_load_bitmap("menu_carregamento_70%.png");
    frames_carregamento[3] = al_load_bitmap("menu_carregamento_100%.png");

    for (int i = 0; i < 4; i++) {
        if (!frames_carregamento[i]) {
            fprintf(stderr, "Erro ao carregar um frame do splash. Verifique o nome do arquivo.\n");
            al_rest(2.0); // Pausa para ler o erro
            return;
        }
    }

    // --- 2. LÓGICA DE TEMPO E DESENHO ---
    double start_time = al_get_time();
    double elapsed_time = 0;
    int frame_index = 0;

    // Loop principal da tela de splash, dura 4 segundos
    while (elapsed_time < 4.0) {
        elapsed_time = al_get_time() - start_time;

        // Decide qual imagem mostrar
        if (elapsed_time < 1.0) {
            frame_index = 0; // 0 a 1 segundo: mostra 0%
        }
        else if (elapsed_time < 2.0) {
            frame_index = 1; // 1 a 2 segundos: mostra 20%
        }
        else if (elapsed_time < 3.0) {
            frame_index = 2; // 2 a 3 segundos: mostra 70%
        }
        else {
            frame_index = 3; // 3 a 4 segundos: mostra 100%
        }

        // Desenha a imagem correspondente
        al_draw_bitmap(frames_carregamento[frame_index], 0, 0, 0);
        al_flip_display();
    }

    // --- 3. LIMPAR A MEMÓRIA ---
    for (int i = 0; i < 4; i++) {
        al_destroy_bitmap(frames_carregamento[i]);
    }
}