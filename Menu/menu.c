#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <string.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 60

typedef enum {
    MENU_JOGAR,
    MENU_OPCOES,
    MENU_SAIR,
    MENU_NENHUM
} MenuEstado;

typedef enum {
    OPCOES_MUSICA,
    OPCOES_VOLUME,
    OPCOES_VOLTAR,
    OPCOES_NENHUM
} OpcoesEstado;

typedef struct {
    float x, y;
    float largura, altura;
    char texto[50];
    ALLEGRO_BITMAP* img_normal;
    ALLEGRO_BITMAP* img_selecionado;
    int selecionado;
} Botao;

typedef struct {
    float x, y;
    float largura, altura;
    char texto[50];
    int selecionado;
    float valor; // volume (0.0 a 1.0)
    int ativo;   // música (ligado/desligado)
} Opcao;

void desenhar_menu(ALLEGRO_BITMAP* bg, ALLEGRO_FONT* font, Botao* botao_jogar, Botao* botao_opcoes, Botao* botao_sair);
void processar_teclado(ALLEGRO_EVENT* evento, MenuEstado* estado_menu, Botao* botao_jogar, Botao* botao_opcoes, Botao* botao_sair);
int executar_acao_menu(MenuEstado estado_menu);
void inicializar_botoes(Botao* b_jogar, Botao* b_opcoes, Botao* b_sair, MenuEstado estado_inicial,
    ALLEGRO_BITMAP* ij_n, ALLEGRO_BITMAP* ij_s,
    ALLEGRO_BITMAP* io_n, ALLEGRO_BITMAP* io_s,
    ALLEGRO_BITMAP* is_n, ALLEGRO_BITMAP* is_s);

// Funcoes pra tela de opcoes
void desenhar_tela_opcoes(ALLEGRO_BITMAP* bg, ALLEGRO_FONT* font, Opcao* musica, Opcao* volume, Opcao* voltar);
void processar_teclado_opcoes(ALLEGRO_EVENT* evento, OpcoesEstado* estado_opcoes, Opcao* musica, Opcao* volume, Opcao* voltar);
void inicializar_opcoes(Opcao* musica, Opcao* volume, Opcao* voltar, OpcoesEstado estado_inicial, int musica_ativa, float volume_valor);

int run_menu_screen(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font, ALLEGRO_TIMER* timer, ALLEGRO_BITMAP* cursor_img) {
    int mouse_x = 0, mouse_y = 0;
    al_set_window_title(display, "MathMonster");

    //Variaveis para controle de audio 
    static int musica_ativa = 1;
    static float volume_valor = 0.4f;

    ALLEGRO_SAMPLE* musica_fundo = al_load_sample("musica_menu.ogg");
    ALLEGRO_BITMAP* background_menu = al_load_bitmap("menu_background.png");
    ALLEGRO_BITMAP* img_jogar_normal = al_load_bitmap("botao_jogar_normal.png");
    ALLEGRO_BITMAP* img_jogar_selecionado = al_load_bitmap("botao_jogar_selecionado.png");
    ALLEGRO_BITMAP* img_sair_normal = al_load_bitmap("botao_sair_normal.png");
    ALLEGRO_BITMAP* img_sair_selecionado = al_load_bitmap("botao_sair_selecionado.png");
    ALLEGRO_BITMAP* img_opcoes_normal = al_load_bitmap("botao_opcoes_normal.png");
    ALLEGRO_BITMAP* img_opcoes_selecionado = al_load_bitmap("botao_opcoes_selecionado.png");
    ALLEGRO_BITMAP* icone = al_load_bitmap("logol.png");

    if (!musica_fundo) {
        fprintf(stderr, "Erro ao carregar a amostra de áudio 'musica_menu.ogg'\n");
    }
    else if (musica_ativa) {
        // aqui usa o volume_valor em vez de valor fixo
        al_play_sample(musica_fundo, volume_valor, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    }

    al_set_display_icon(display, icone);
    Botao botao_jogar, botao_opcoes, botao_sair;
    MenuEstado estado_menu = MENU_JOGAR;
    inicializar_botoes(&botao_jogar, &botao_opcoes, &botao_sair, estado_menu,
        img_jogar_normal, img_jogar_selecionado,
        img_opcoes_normal, img_opcoes_selecionado,
        img_sair_normal, img_sair_selecionado);

    //Variaveis para tela de opcoes
    int em_opcoes = 0;
    Opcao opcao_musica, opcao_volume, opcao_voltar;
    OpcoesEstado estado_opcoes = OPCOES_MUSICA;

    int executando = 1;
    int redesenhar = 1;
    ALLEGRO_EVENT evento;

    al_flush_event_queue(event_queue);

    while (executando) {
        al_wait_for_event(event_queue, &evento);

        if (!em_opcoes) {
            // ---------- LÓGICA DO MENU PRINCIPAL ------------
            switch (evento.type) {
            case ALLEGRO_EVENT_MOUSE_AXES:
                mouse_x = evento.mouse.x;
                mouse_y = evento.mouse.y;

                if (evento.mouse.x >= botao_jogar.x && evento.mouse.x <= botao_jogar.x + botao_jogar.largura &&
                    evento.mouse.y >= botao_jogar.y && evento.mouse.y <= botao_jogar.y + botao_jogar.altura) {
                    estado_menu = MENU_JOGAR;
                }
                else if (evento.mouse.x >= botao_opcoes.x && evento.mouse.x <= botao_opcoes.x + botao_opcoes.largura &&
                    evento.mouse.y >= botao_opcoes.y && evento.mouse.y <= botao_opcoes.y + botao_opcoes.altura) {
                    estado_menu = MENU_OPCOES;
                }
                else if (evento.mouse.x >= botao_sair.x && evento.mouse.x <= botao_sair.x + botao_sair.largura &&
                    evento.mouse.y >= botao_sair.y && evento.mouse.y <= botao_sair.y + botao_sair.altura) {
                    estado_menu = MENU_SAIR;
                }
                else {
                    estado_menu = MENU_NENHUM;
                }

                botao_jogar.selecionado = (estado_menu == MENU_JOGAR);
                botao_opcoes.selecionado = (estado_menu == MENU_OPCOES);
                botao_sair.selecionado = (estado_menu == MENU_SAIR);
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (estado_menu == MENU_JOGAR) {
                    al_destroy_sample(musica_fundo);
                    return 1; // jogar
                }
                else if (estado_menu == MENU_OPCOES) {
                    // entrar na tela de opções
                    em_opcoes = 1;
                    estado_opcoes = OPCOES_MUSICA;
                    inicializar_opcoes(&opcao_musica, &opcao_volume, &opcao_voltar,
                        estado_opcoes, musica_ativa, volume_valor);
                }
                else if (estado_menu == MENU_SAIR) {
                    al_destroy_sample(musica_fundo);
                    return 2; // sair
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                al_destroy_sample(musica_fundo);
                return 2;

            case ALLEGRO_EVENT_TIMER:
                redesenhar = 1;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                processar_teclado(&evento, &estado_menu, &botao_jogar, &botao_opcoes, &botao_sair);

                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    if (estado_menu == MENU_JOGAR) {
                        al_destroy_sample(musica_fundo);
                        return 1;
                    }
                    else if (estado_menu == MENU_OPCOES) {
                        // entra na tela de opções
                        em_opcoes = 1;
                        estado_opcoes = OPCOES_MUSICA;
                        inicializar_opcoes(&opcao_musica, &opcao_volume, &opcao_voltar,
                            estado_opcoes, musica_ativa, volume_valor);
                    }
                    else if (estado_menu == MENU_SAIR) {
                        al_destroy_sample(musica_fundo);
                        return 2;
                    }
                }
                else if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    al_destroy_sample(musica_fundo);
                    return 2;
                }
                break;
            }
        }
        else {
            // ----------------- LÓGICA DA TELA DE OPÇÕES -----------------
            switch (evento.type) {
            case ALLEGRO_EVENT_MOUSE_AXES:
                mouse_x = evento.mouse.x;
                mouse_y = evento.mouse.y;

                if (evento.mouse.x >= opcao_musica.x && evento.mouse.x <= opcao_musica.x + opcao_musica.largura &&
                    evento.mouse.y >= opcao_musica.y && evento.mouse.y <= opcao_musica.y + opcao_musica.altura) {
                    estado_opcoes = OPCOES_MUSICA;
                }
                else if (evento.mouse.x >= opcao_volume.x && evento.mouse.x <= opcao_volume.x + opcao_volume.largura &&
                    evento.mouse.y >= opcao_volume.y && evento.mouse.y <= opcao_volume.y + opcao_volume.altura) {
                    estado_opcoes = OPCOES_VOLUME;
                }
                else if (evento.mouse.x >= opcao_voltar.x && evento.mouse.x <= opcao_voltar.x + opcao_voltar.largura &&
                    evento.mouse.y >= opcao_voltar.y && evento.mouse.y <= opcao_voltar.y + opcao_voltar.altura) {
                    estado_opcoes = OPCOES_VOLTAR;
                }
                else {
                    estado_opcoes = OPCOES_NENHUM;
                }

                opcao_musica.selecionado = (estado_opcoes == OPCOES_MUSICA);
                opcao_volume.selecionado = (estado_opcoes == OPCOES_VOLUME);
                opcao_voltar.selecionado = (estado_opcoes == OPCOES_VOLTAR);
                break;

            case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
                if (estado_opcoes == OPCOES_MUSICA) {
                    musica_ativa = !musica_ativa;
                    opcao_musica.ativo = musica_ativa;

                    // aplica a mudança na música
                    if (musica_ativa) {
                        al_stop_samples();
                        al_play_sample(musica_fundo, volume_valor, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
                    }
                    else {
                        al_stop_samples();
                    }
                }
                else if (estado_opcoes == OPCOES_VOLTAR) {
                    em_opcoes = 0; // volta para o menu
                }
                break;

            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                al_destroy_sample(musica_fundo);
                return 2;

            case ALLEGRO_EVENT_TIMER:
                redesenhar = 1;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                processar_teclado_opcoes(&evento, &estado_opcoes, &opcao_musica, &opcao_volume, &opcao_voltar);

                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    if (estado_opcoes == OPCOES_MUSICA) {
                        musica_ativa = !musica_ativa;
                        opcao_musica.ativo = musica_ativa;

                        if (musica_ativa) {
                            al_stop_samples();
                            al_play_sample(musica_fundo, volume_valor, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
                        }
                        else {
                            al_stop_samples();
                        }
                    }
                    else if (estado_opcoes == OPCOES_VOLTAR) {
                        em_opcoes = 0; // volta ao menu
                    }
                }
                else if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    em_opcoes = 0; // volta pro menu com ESC
                }
                break;
            }

            // atualiza volume global da musica
            volume_valor = opcao_volume.valor;
        }

        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = 0;

            if (!em_opcoes) {
                // Desenha o menu principal
                desenhar_menu(background_menu, font, &botao_jogar, &botao_opcoes, &botao_sair);
            }
            else {
                //Desenha a tela de opcoes
                desenhar_tela_opcoes(background_menu, font, &opcao_musica, &opcao_volume, &opcao_voltar);
            }

            al_draw_bitmap(cursor_img, mouse_x, mouse_y, 0);
            al_flip_display();
        }
    }

    al_destroy_sample(musica_fundo);
    al_destroy_bitmap(img_sair_normal);
    al_destroy_bitmap(img_sair_selecionado);
    al_destroy_bitmap(background_menu);
    al_destroy_bitmap(img_jogar_normal);
    al_destroy_bitmap(img_jogar_selecionado);
    al_destroy_bitmap(img_opcoes_normal);
    al_destroy_bitmap(img_opcoes_selecionado);
    al_destroy_bitmap(icone);
    return 2;
}

// --------------- NOVAS FUNÇÕES PARA TELA DE OPÇÕES ----------------

void inicializar_opcoes(Opcao* musica, Opcao* volume, Opcao* voltar,
    OpcoesEstado estado_inicial, int musica_ativa, float volume_valor) {
    float largura = 400;
    float altura = 50;
    float espacamento = 20;
    float y_inicial = 200;

    // Musica
    musica->x = (LARGURA_TELA - largura) / 2;
    musica->y = y_inicial;
    musica->largura = largura;
    musica->altura = altura;
    strcpy(musica->texto, "Musica");
    musica->selecionado = (estado_inicial == OPCOES_MUSICA);
    musica->ativo = musica_ativa;
    musica->valor = 0.0f;

    // Volume
    volume->x = (LARGURA_TELA - largura) / 2;
    volume->y = y_inicial + altura + espacamento;
    volume->largura = largura;
    volume->altura = altura;
    strcpy(volume->texto, "Volume");
    volume->selecionado = (estado_inicial == OPCOES_VOLUME);
    volume->valor = volume_valor;
    volume->ativo = 0;

    // Voltar
    voltar->x = (LARGURA_TELA - largura) / 2;
    voltar->y = y_inicial + 2 * (altura + espacamento);
    voltar->largura = largura;
    voltar->altura = altura;
    strcpy(voltar->texto, "Voltar ao Menu");
    voltar->selecionado = (estado_inicial == OPCOES_VOLTAR);
    voltar->valor = 0.0f;
    voltar->ativo = 0;
}

void desenhar_tela_opcoes(ALLEGRO_BITMAP* bg, ALLEGRO_FONT* font,
    Opcao* musica, Opcao* volume, Opcao* voltar) {
    al_draw_bitmap(bg, 0, 0, 0);

    ALLEGRO_COLOR cor_fundo_cinza = al_map_rgb(120, 120, 120);

    // Título
    al_draw_text(font, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 25,
        ALLEGRO_ALIGN_CENTER, "CONFIGURACOES");

    ALLEGRO_COLOR cor_normal = al_map_rgb(200, 200, 200);
    ALLEGRO_COLOR cor_selecionado = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR cor_verde = al_map_rgb(0, 255, 0);

    ALLEGRO_FONT* font_pequena = al_load_ttf_font("joystix.ttf", 14, 0);
    if (!font_pequena) {
        font_pequena = font;
    }

    // Desenhar opcao Música
    ALLEGRO_COLOR cor_musica = musica->selecionado ? cor_selecionado : cor_normal;
    al_draw_rectangle(musica->x, musica->y, musica->x + musica->largura,
        musica->y + musica->altura, cor_musica, 2);

    // cor cinza do fundo
    al_draw_filled_rectangle(musica->x, musica->y,
        musica->x + musica->largura,
        musica->y + musica->altura, cor_fundo_cinza);

    char texto_musica[100];
    sprintf(texto_musica, "Musica: %s", musica->ativo ? "LIGADA" : "DESLIGADA");
    al_draw_text(font_pequena, cor_texto, musica->x + 10,
        musica->y + musica->altura / 2 - 12, ALLEGRO_ALIGN_LEFT, texto_musica);

    if (musica->selecionado) {
        al_draw_text(font_pequena, cor_selecionado, musica->x - 30,
            musica->y + musica->altura / 2 - 12, ALLEGRO_ALIGN_CENTER, ">");
    }

    // Desenhar opcao Volume
    ALLEGRO_COLOR cor_volume = volume->selecionado ? cor_selecionado : cor_normal;
    al_draw_rectangle(volume->x, volume->y, volume->x + volume->largura,
        volume->y + volume->altura, cor_volume, 2);

    // cor cinza do fundo
    al_draw_filled_rectangle(volume->x, volume->y,
        volume->x + volume->largura,
        volume->y + volume->altura, cor_fundo_cinza);

    // Barra de volume
    float barra_largura = 200;
    float barra_x = volume->x + 165;
    float barra_y = volume->y + volume->altura / 2 - 5;

    al_draw_rectangle(barra_x, barra_y, barra_x + barra_largura, barra_y + 10, cor_texto, 1);
    al_draw_filled_rectangle(barra_x, barra_y, barra_x + (barra_largura * volume->valor),
        barra_y + 10, cor_verde);

    char texto_volume[50];
    sprintf(texto_volume, "Volume: %d%%", (int)(volume->valor * 100));
    al_draw_text(font_pequena, cor_texto, volume->x + 10,
        volume->y + volume->altura / 2 - 12, ALLEGRO_ALIGN_LEFT, texto_volume);

    if (volume->selecionado) {
        al_draw_text(font_pequena, cor_selecionado, volume->x - 30,
            volume->y + volume->altura / 2 - 12, ALLEGRO_ALIGN_CENTER, ">");
    }

    // desenha opcao "Voltar"
    ALLEGRO_COLOR cor_voltar = voltar->selecionado ? cor_selecionado : cor_normal;
    al_draw_rectangle(voltar->x, voltar->y, voltar->x + voltar->largura,
        voltar->y + voltar->altura, cor_voltar, 2);

    // cor cinza do fundo
    al_draw_filled_rectangle(voltar->x, voltar->y,
        voltar->x + voltar->largura,
        voltar->y + voltar->altura, cor_fundo_cinza);

    al_draw_text(font_pequena, cor_texto, voltar->x + voltar->largura / 2,
        voltar->y + voltar->altura / 2 - 12, ALLEGRO_ALIGN_CENTER, voltar->texto);

    if (voltar->selecionado) {
        al_draw_text(font_pequena, cor_selecionado, voltar->x - 30,
            voltar->y + voltar->altura / 2 - 12, ALLEGRO_ALIGN_CENTER, ">");
    }

    // Instrucoes
    al_draw_text(font_pequena, al_map_rgb(150, 150, 150), LARGURA_TELA / 2, ALTURA_TELA - 80,
        ALLEGRO_ALIGN_CENTER, "Use SETAS para navegar, ENTER para selecionar");
    al_draw_text(font_pequena, al_map_rgb(150, 150, 150), LARGURA_TELA / 2, ALTURA_TELA - 50,
        ALLEGRO_ALIGN_CENTER, "A/D ou <-/-> para ajustar volume, ESC para voltar");

    if (font_pequena != font) {
        al_destroy_font(font_pequena);
    }
}

void processar_teclado_opcoes(ALLEGRO_EVENT* evento, OpcoesEstado* estado_opcoes,
    Opcao* musica, Opcao* volume, Opcao* voltar) {
    if (evento->keyboard.keycode == ALLEGRO_KEY_UP || evento->keyboard.keycode == ALLEGRO_KEY_W) {
        if (*estado_opcoes == OPCOES_VOLTAR) *estado_opcoes = OPCOES_VOLUME;
        else if (*estado_opcoes == OPCOES_VOLUME) *estado_opcoes = OPCOES_MUSICA;
    }
    else if (evento->keyboard.keycode == ALLEGRO_KEY_DOWN || evento->keyboard.keycode == ALLEGRO_KEY_S) {
        if (*estado_opcoes == OPCOES_MUSICA) *estado_opcoes = OPCOES_VOLUME;
        else if (*estado_opcoes == OPCOES_VOLUME) *estado_opcoes = OPCOES_VOLTAR;
    }
    else if ((evento->keyboard.keycode == ALLEGRO_KEY_LEFT || evento->keyboard.keycode == ALLEGRO_KEY_A) &&
        *estado_opcoes == OPCOES_VOLUME) {
        // Diminui o volume
        volume->valor -= 0.1f;
        if (volume->valor < 0.0f) volume->valor = 0.0f;
    }
    else if ((evento->keyboard.keycode == ALLEGRO_KEY_RIGHT || evento->keyboard.keycode == ALLEGRO_KEY_D) &&
        *estado_opcoes == OPCOES_VOLUME) {
        // Aumenta o volume
        volume->valor += 0.1f;
        if (volume->valor > 1.0f) volume->valor = 1.0f;
    }

    // atualiza o estado visual
    musica->selecionado = (*estado_opcoes == OPCOES_MUSICA);
    volume->selecionado = (*estado_opcoes == OPCOES_VOLUME);
    voltar->selecionado = (*estado_opcoes == OPCOES_VOLTAR);
}

// ------------- FUNÇÕES ORIGINAIS DO MENU -------------

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
    al_install_mouse();


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
        fprintf(stderr, "Falha ao carregar fonte.\n");
        return 0;
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

void inicializar_botoes(Botao* b_jogar, Botao* b_opcoes, Botao* b_sair, MenuEstado estado_inicial,
    ALLEGRO_BITMAP* ij_n, ALLEGRO_BITMAP* ij_s,
    ALLEGRO_BITMAP* io_n, ALLEGRO_BITMAP* io_s,
    ALLEGRO_BITMAP* is_n, ALLEGRO_BITMAP* is_s)
{
    float largura_botao = 180; // largura
    float altura_botao = 180;  // altura
    float espacamento = 30;    // espacamento entre os botões
    float y_pos = 500;         // y
    // botao opcao
    b_opcoes->largura = largura_botao;
    b_opcoes->altura = altura_botao;
    b_opcoes->x = (LARGURA_TELA / 2) - (largura_botao / 2);
    b_opcoes->y = y_pos;
    b_opcoes->img_normal = io_n;
    b_opcoes->img_selecionado = io_s;

    // botao jogar
    b_jogar->largura = largura_botao;
    b_jogar->altura = altura_botao;
    b_jogar->x = b_opcoes->x - largura_botao - espacamento;
    b_jogar->y = y_pos;
    b_jogar->img_normal = ij_n;
    b_jogar->img_selecionado = ij_s;

    // botao sair
    b_sair->largura = largura_botao;
    b_sair->altura = altura_botao;
    b_sair->x = b_opcoes->x + largura_botao + espacamento;
    b_sair->y = y_pos;
    b_sair->img_normal = is_n;
    b_sair->img_selecionado = is_s;


    b_jogar->selecionado = (estado_inicial == MENU_JOGAR);
    b_opcoes->selecionado = (estado_inicial == MENU_OPCOES);
    b_sair->selecionado = (estado_inicial == MENU_SAIR);
}

void desenhar_menu(ALLEGRO_BITMAP* bg, ALLEGRO_FONT* font, Botao* botao_jogar, Botao* botao_opcoes, Botao* botao_sair) {

    al_draw_bitmap(bg, 0, 0, 0);

    if (botao_jogar->selecionado) {
        // efeito hover
        al_draw_bitmap(botao_jogar->img_selecionado, botao_jogar->x, botao_jogar->y, 0);
    }
    else {
        // carregar sem o efeito hover (normal)
        al_draw_bitmap(botao_jogar->img_normal, botao_jogar->x, botao_jogar->y, 0);
    }
    if (botao_opcoes->selecionado) {
        al_draw_bitmap(botao_opcoes->img_selecionado, botao_opcoes->x, botao_opcoes->y, 0);
    }
    else {
        al_draw_bitmap(botao_opcoes->img_normal, botao_opcoes->x, botao_opcoes->y, 0);
    }
    if (botao_jogar->selecionado) {
        al_draw_text(font, al_map_rgb(255, 255, 0),
            botao_jogar->x - 30,
            botao_jogar->y + botao_jogar->altura / 2 - 12,
            ALLEGRO_ALIGN_CENTER, ">");
    }

    if (botao_sair->selecionado) {
        al_draw_bitmap(botao_sair->img_selecionado, botao_sair->x, botao_sair->y, 0);
    }
    else {
        al_draw_bitmap(botao_sair->img_normal, botao_sair->x, botao_sair->y, 0);
    }
}

void processar_teclado(ALLEGRO_EVENT* evento, MenuEstado* estado_menu, Botao* botao_jogar, Botao* botao_opcoes, Botao* botao_sair) {

    if (evento->keyboard.keycode == ALLEGRO_KEY_LEFT || evento->keyboard.keycode == ALLEGRO_KEY_A) {
        if (*estado_menu == MENU_SAIR) *estado_menu = MENU_OPCOES;
        else if (*estado_menu == MENU_OPCOES) *estado_menu = MENU_JOGAR;
    }
    else if (evento->keyboard.keycode == ALLEGRO_KEY_RIGHT || evento->keyboard.keycode == ALLEGRO_KEY_D) {
        if (*estado_menu == MENU_JOGAR) *estado_menu = MENU_OPCOES;
        else if (*estado_menu == MENU_OPCOES) *estado_menu = MENU_SAIR;
    }

    //att o visu dos botoes
    botao_jogar->selecionado = (*estado_menu == MENU_JOGAR);
    botao_opcoes->selecionado = (*estado_menu == MENU_OPCOES);
    botao_sair->selecionado = (*estado_menu == MENU_SAIR);
}

int executar_acao_menu(MenuEstado estado_menu) {
    switch (estado_menu) {
    case MENU_JOGAR:
        return 1; // jogar
    case MENU_SAIR:
        return 2; // sair
    case MENU_OPCOES:
        return 3; // tela opcoes
    default:
        return 0;
    }
}