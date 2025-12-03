// ARQUIVO: tela_final.c

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include "tela_final.h"

#define LARGURA_TELA 1366
#define ALTURA_TELA 768

int run_tela_final(ALLEGRO_DISPLAY* display) {

    // --- CARREGAMENTO DE FONTES ---
    // Usamos tamanhos diferentes para hierarquia
    ALLEGRO_FONT* font_titulo = al_load_ttf_font("joystix.ttf", 80, 0);
    ALLEGRO_FONT* font_subtitulo = al_load_ttf_font("joystix.ttf", 40, 0);
    ALLEGRO_FONT* font_pequena = al_load_ttf_font("joystix.ttf", 20, 0);

    if (!font_titulo || !font_subtitulo) {
        printf("Erro ao carregar fontes da tela final.\n");
        return 0;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();

    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_keyboard_event_source());

    al_start_timer(timer);

    bool rodando = true;
    bool redesenhar = true;
    int timer_blink = 0;

    // Efeito de cor arco-íris para o "Parabéns"
    float hue = 0;

    int retorno = 1; // 1 = Voltar ao Menu

    while (rodando) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            redesenhar = true;
            timer_blink++;
            hue += 0.01f;
            if (hue > 1.0f) hue = 0.0f;
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            retorno = 0; // Fechar jogo
            rodando = false;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER ||
                event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                retorno = 1; // Voltar ao Menu
                rodando = false;
            }
        }

        if (redesenhar && al_is_event_queue_empty(queue)) {
            redesenhar = false;

            // Fundo Verde Escuro (Sensação de Vitória/Sucesso)
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Título "PARABÉNS"
            // Usei amarelo dourado, mas você pode mudar
            al_draw_text(font_titulo, al_map_rgb(255, 215, 0), LARGURA_TELA / 2, 200, ALLEGRO_ALIGN_CENTER, "PARABENS!");

            // Subtítulo
            al_draw_text(font_subtitulo, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 350, ALLEGRO_ALIGN_CENTER, "VOCE LIBERTOU SEUS AMIGOS");
            al_draw_text(font_pequena, al_map_rgb(200, 200, 200), LARGURA_TELA / 2, 420, ALLEGRO_ALIGN_CENTER, "O Senhor aritmetica foi eliminado!");

            // Texto Piscante
            if (timer_blink % 60 < 35) {
                al_draw_text(font_pequena, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 600, ALLEGRO_ALIGN_CENTER, "APERTE ENTER PARA VOLTAR AO MENU");
            }

            al_flip_display();
        }
    }

    // Cleanup
    al_destroy_font(font_titulo);
    al_destroy_font(font_subtitulo);
    al_destroy_font(font_pequena);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return retorno;
}