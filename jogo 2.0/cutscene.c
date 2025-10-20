#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h> 
#include <stdio.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "cutscene.h"
#define LARGURA_TELA 800
#define ALTURA_TELA 600
void run_cutscene_screen(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font) {
    printf("Cutscene: Carregando recursos...\n");

    // --- 1. CARREGAR RECURSOS ---
    ALLEGRO_BITMAP* slides[4];
    slides[0] = al_load_bitmap("cut1.png");
    slides[1] = al_load_bitmap("cut2.png");
    slides[2] = al_load_bitmap("cut3.png");
    slides[3] = al_load_bitmap("cut4.png");

    const char* textos[4] = {
        "Havia uma vila de monstros que amavam os numeros e os usavam para criar arte.",
        "Um dia, um forasteiro chegou... O Senhor Aritmetica, da Ordem Absoluta da Logica, ele viu os monstros usando os numeros de forma \"impura\" para sua Ordem e, enfurecido, baniu todos para o Subsolo Logico.",
        "Um jovem monstro, que colhia \"frutas matematicas\" na floresta, escapou por pouco. Ao retornar, percebeu que era o unico sobrevivente.",
        "Ele entao decide entrar no Subsolo Logico para resgatar seus amigos!"
    };

    ALLEGRO_SAMPLE* musica_cutscene = al_load_sample("cutscene_music.ogg");
    ALLEGRO_SAMPLE* sfx_letra = al_load_sample("typewrite.ogg");

    // --- 2. LÓGICA PRINCIPAL DA CUTSCENE ---

    if (musica_cutscene) {
        al_play_sample(musica_cutscene, 0.7, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    }

    bool rodando_cutscene = true;

    al_flush_event_queue(event_queue);

    int slide_atual = 0;
    bool sair_cutscene = false;
    bool redesenhar = true;
    al_flush_event_queue(event_queue);

    // O loop continua enquanto não tivermos chegado ao fim dos slides e o jogador não fechar a janela
    while (slide_atual < 4 && !sair_cutscene) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            sair_cutscene = true; // Sai da cutscene
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                slide_atual++; // AVANÇA PARA O PRÓXIMO SLIDE!
            }
        }

        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = false;

            // --- Seção de Desenho ---
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Desenha a imagem do SLIDE ATUAL
            al_draw_bitmap(slides[slide_atual], 0, 0, 0);

            // Desenha a caixa de texto
            al_draw_filled_rectangle(20, ALTURA_TELA - 120, LARGURA_TELA - 20, ALTURA_TELA - 20, al_map_rgba(0, 0, 0, 150));

            // Desenha o texto do SLIDE ATUAL
            al_draw_multiline_text(font, al_map_rgb(255, 255, 255), 40, ALTURA_TELA - 240, LARGURA_TELA - 60, al_get_font_line_height(font), 0, textos[slide_atual]);

            al_flip_display();
        }
    }

    // Para a música da cutscene antes de sair
    al_stop_samples();


    // --- 3. DESTRUIR RECURSOS ---
    // (código de limpeza que você já tem)
    for (int i = 0; i < 4; i++) {
        if (slides[i]) al_destroy_bitmap(slides[i]);
    }
    if (musica_cutscene) al_destroy_sample(musica_cutscene);
    if (sfx_letra) al_destroy_sample(sfx_letra);
}