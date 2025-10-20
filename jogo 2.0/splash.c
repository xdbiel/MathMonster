#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#include "splash.h"

void run_splash_screen(ALLEGRO_DISPLAY* display) {

 
    ALLEGRO_BITMAP* frames_carregamento[4];
    frames_carregamento[0] = al_load_bitmap("menu_carregamento_0%.png");
    frames_carregamento[1] = al_load_bitmap("menu_carregamento_20%.png");
    frames_carregamento[2] = al_load_bitmap("menu_carregamento_70%.png");
    frames_carregamento[3] = al_load_bitmap("menu_carregamento_100%.png");

    

	// algettime para verificar o tempo que passou
    double start_time = al_get_time();
    double elapsed_time = 0;
    int frame_index = 0;

    // o loop tem 4 sec de duracao, 1 sec cada imagem, fiz isso pra simular um carregamento rsrsrs so pra ficar daora
    while (elapsed_time < 4.0) {
        elapsed_time = al_get_time() - start_time;

        // Decide qual imagem mostrar
        if (elapsed_time < 1.0) {
            frame_index = 0; 
        }
        else if (elapsed_time < 2.0) {
            frame_index = 1; 
        }
        else if (elapsed_time < 3.0) {
            frame_index = 2; 
        }
        else {
            frame_index = 3; 
        }

        // desenha a tela
        al_draw_bitmap(frames_carregamento[frame_index], 0, 0, 0);
        al_flip_display();
    }

  
    for (int i = 0; i < 4; i++) {
        al_destroy_bitmap(frames_carregamento[i]);
    }
}