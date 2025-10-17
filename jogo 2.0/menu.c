#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 60

typedef enum {
    MENU_JOGAR,
    MENU_SAIR
} MenuEstado;

typedef struct {
    float x, y;
    float largura, altura;
    char texto[50];
    ALLEGRO_COLOR cor_normal;
    ALLEGRO_COLOR cor_selecionado;
    int selecionado;
} Botao;


void desenhar_menu(ALLEGRO_FONT* font, Botao* botao_jogar, Botao* botao_sair);
void processar_teclado(ALLEGRO_EVENT* evento, MenuEstado* estado_menu, Botao* botao_jogar, Botao* botao_sair);
int executar_acao_menu(MenuEstado estado_menu);
void inicializar_botoes(Botao* botao_jogar, Botao* botao_sair, MenuEstado estado_inicial);

// Em menu.c, SUBSTITUA a antiga int main() por esta:

int run_menu_screen(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font, ALLEGRO_TIMER* timer, ALLEGRO_BITMAP* cursor_img); {
    
    Botao botao_jogar, botao_sair;
    MenuEstado estado_menu = MENU_JOGAR;
    inicializar_botoes(&botao_jogar, &botao_sair, estado_menu);

    int executando = 1;
    int redesenhar = 1;
    ALLEGRO_EVENT evento;

    // Limpa a fila de eventos de qualquer "sujeira" de telas anteriores
    al_flush_event_queue(event_queue);

    while (executando) {
        al_wait_for_event(event_queue, &evento);

        switch (evento.type) {
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                return 2; // Retorna 2 (SAIR) se fechar a janela

            case ALLEGRO_EVENT_TIMER:
                redesenhar = 1;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                processar_teclado(&evento, &estado_menu, &botao_jogar, &botao_sair);

                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    return executar_acao_menu(estado_menu); // Retorna 1 para JOGAR ou 2 para SAIR
                }
                else if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    return 2; // Retorna 2 (SAIR) se apertar ESC
                }
                break;
        }

        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = 0;
            desenhar_menu(font, &botao_jogar, &botao_sair);
            al_flip_display();
        }
    }
    
    return 2; // Caso saia do loop por outro motivo, retorna SAIR por segurança
}

int inicializar_allegro(ALLEGRO_DISPLAY** display, ALLEGRO_EVENT_QUEUE** event_queue,
    ALLEGRO_FONT** font, ALLEGRO_TIMER** timer) {
    // Inicializar Allegro
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar Allegro.\n");
        return 0;
    }

    al_init_font_addon();
    al_init_ttf_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_keyboard();

    *display = al_create_display(LARGURA_TELA, ALTURA_TELA);
    if (!*display) {
        fprintf(stderr, "Falha ao criar display.\n");
        return 0;
    }

    *timer = al_create_timer(1.0 / FPS);
    if (!*timer) {
        fprintf(stderr, "Falha ao criar timer.\n");
        return 0;
    }

    *event_queue = al_create_event_queue();
    if (!*event_queue) {
        fprintf(stderr, "Falha ao criar fila de eventos.\n");
        return 0;
    }

    *font = al_load_ttf_font("arial.ttf", 48, 0);
    if (!*font) {

        *font = al_create_builtin_font();
        if (!*font) {
            fprintf(stderr, "Falha ao carregar fonte.\n");
            return 0;
        }
    }

    al_register_event_source(*event_queue, al_get_display_event_source(*display));
    al_register_event_source(*event_queue, al_get_timer_event_source(*timer));
    al_register_event_source(*event_queue, al_get_keyboard_event_source());

    al_start_timer(*timer);

    return 1;
}

void finalizar_allegro(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue,
    ALLEGRO_FONT* font, ALLEGRO_TIMER* timer) {
    if (font) al_destroy_font(font);
    if (timer) al_destroy_timer(timer);
    if (event_queue) al_destroy_event_queue(event_queue);
    if (display) al_destroy_display(display);

    al_uninstall_keyboard();
}

void inicializar_botoes(Botao* botao_jogar, Botao* botao_sair, MenuEstado estado_inicial) {
    // Bot�o Jogar
    botao_jogar->largura = 250;
    botao_jogar->altura = 60;
    botao_jogar->x = (LARGURA_TELA - botao_jogar->largura) / 2;
    botao_jogar->y = ALTURA_TELA / 2 - 40;
    strcpy_s(botao_jogar->texto, sizeof(botao_jogar->texto), "JOGAR");
    botao_jogar->cor_normal = al_map_rgb(70, 130, 180);
    botao_jogar->cor_selecionado = al_map_rgb(100, 160, 210);
    botao_jogar->selecionado = (estado_inicial == MENU_JOGAR);

    // Bot�o Sair
    botao_sair->largura = 250;
    botao_sair->altura = 60;
    botao_sair->x = (LARGURA_TELA - botao_sair->largura) / 2;
    botao_sair->y = ALTURA_TELA / 2 + 40;
    strcpy_s(botao_sair->texto, sizeof(botao_sair->texto), "SAIR");
    botao_sair->cor_normal = al_map_rgb(220, 20, 60);
    botao_sair->cor_selecionado = al_map_rgb(255, 60, 100);
    botao_sair->selecionado = (estado_inicial == MENU_SAIR);
}

void desenhar_menu(ALLEGRO_FONT* font, Botao* botao_jogar, Botao* botao_sair) {

    al_clear_to_color(al_map_rgb(30, 30, 40));

    al_draw_text(font, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 100,
        ALLEGRO_ALIGN_CENTER, "WORLD TRAVELER");

    ALLEGRO_FONT* font_pequena = al_create_builtin_font();
    if (font_pequena) {
        al_draw_text(font_pequena, al_map_rgb(200, 200, 200), LARGURA_TELA / 2, 150,
            ALLEGRO_ALIGN_CENTER, "Use SETAS para navegar e ENTER para selecionar");
        al_draw_text(font_pequena, al_map_rgb(200, 200, 200), LARGURA_TELA / 2, 170,
            ALLEGRO_ALIGN_CENTER, "Precione ESC para sair");
    }

    ALLEGRO_COLOR cor_jogar = botao_jogar->selecionado ? botao_jogar->cor_selecionado : botao_jogar->cor_normal;
    al_draw_filled_rectangle(botao_jogar->x, botao_jogar->y,
        botao_jogar->x + botao_jogar->largura,
        botao_jogar->y + botao_jogar->altura, cor_jogar);
    al_draw_rectangle(botao_jogar->x, botao_jogar->y,
        botao_jogar->x + botao_jogar->largura,
        botao_jogar->y + botao_jogar->altura,
        al_map_rgb(255, 255, 255), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255),
        botao_jogar->x + botao_jogar->largura / 2,
        botao_jogar->y + botao_jogar->altura / 2 - 12,
        ALLEGRO_ALIGN_CENTER, botao_jogar->texto);

    if (botao_jogar->selecionado) {
        al_draw_text(font, al_map_rgb(255, 255, 0),
            botao_jogar->x - 30,
            botao_jogar->y + botao_jogar->altura / 2 - 12,
            ALLEGRO_ALIGN_CENTER, ">");
    }

    ALLEGRO_COLOR cor_sair = botao_sair->selecionado ? botao_sair->cor_selecionado : botao_sair->cor_normal;
    al_draw_filled_rectangle(botao_sair->x, botao_sair->y,
        botao_sair->x + botao_sair->largura,
        botao_sair->y + botao_sair->altura, cor_sair);
    al_draw_rectangle(botao_sair->x, botao_sair->y,
        botao_sair->x + botao_sair->largura,
        botao_sair->y + botao_sair->altura,
        al_map_rgb(255, 255, 255), 2);
    al_draw_text(font, al_map_rgb(255, 255, 255),
        botao_sair->x + botao_sair->largura / 2,
        botao_sair->y + botao_sair->altura / 2 - 12,
        ALLEGRO_ALIGN_CENTER, botao_sair->texto);

    if (botao_sair->selecionado) {
        al_draw_text(font, al_map_rgb(255, 255, 0),
            botao_sair->x - 30,
            botao_sair->y + botao_sair->altura / 2 - 12,
            ALLEGRO_ALIGN_CENTER, ">");
    }

    if (font_pequena) {
        al_destroy_font(font_pequena);
    }
}

void processar_teclado(ALLEGRO_EVENT* evento, MenuEstado* estado_menu, Botao* botao_jogar, Botao* botao_sair) {
    switch (evento->keyboard.keycode) {
    case ALLEGRO_KEY_UP:
        if (*estado_menu == MENU_SAIR) {
            *estado_menu = MENU_JOGAR;
            botao_jogar->selecionado = 1;
            botao_sair->selecionado = 0;
        }
        break;

    case ALLEGRO_KEY_DOWN:
        if (*estado_menu == MENU_JOGAR) {
            *estado_menu = MENU_SAIR;
            botao_jogar->selecionado = 0;
            botao_sair->selecionado = 1;
        }
        break;

    case ALLEGRO_KEY_W:
        if (*estado_menu == MENU_SAIR) {
            *estado_menu = MENU_JOGAR;
            botao_jogar->selecionado = 1;
            botao_sair->selecionado = 0;
        }
        break;

    case ALLEGRO_KEY_S:
        if (*estado_menu == MENU_JOGAR) {
            *estado_menu = MENU_SAIR;
            botao_jogar->selecionado = 0;
            botao_sair->selecionado = 1;
        }
        break;
    }
}

int executar_acao_menu(MenuEstado estado_menu) {
    switch (estado_menu) {
    case MENU_JOGAR:
        return 1; // Iniciar jogo
    case MENU_SAIR:
        return 2; // Sair do jogo
    default:
        return 0;
    }
}