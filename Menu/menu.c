#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>

#define LARGURA_TELA 800
#define ALTURA_TELA 600
#define FPS 60

typedef enum {
    MENU_JOGAR,
    MENU_OPCOES, 
    MENU_SAIR,
    MENU_NENHUM
} MenuEstado;

typedef struct {
    float x, y;
    float largura, altura;
    char texto[50];
    ALLEGRO_BITMAP* img_normal;
    ALLEGRO_BITMAP* img_selecionado;
    int selecionado;
} Botao;


void desenhar_menu(ALLEGRO_BITMAP* bg, ALLEGRO_FONT* font, Botao* botao_jogar, Botao* botao_opcoes, Botao* botao_sair);
void processar_teclado(ALLEGRO_EVENT* evento, MenuEstado* estado_menu, Botao* botao_jogar, Botao* botao_opcoes, Botao* botao_sair);
int executar_acao_menu(MenuEstado estado_menu);
void inicializar_botoes(Botao* b_jogar, Botao* b_opcoes, Botao* b_sair, MenuEstado estado_inicial,
    ALLEGRO_BITMAP* ij_n, ALLEGRO_BITMAP* ij_s,
    ALLEGRO_BITMAP* io_n, ALLEGRO_BITMAP* io_s,
    ALLEGRO_BITMAP* is_n, ALLEGRO_BITMAP* is_s);

// TROCADO O MAIN () POR UMA FUNCAO DO TIPO INTEIRO, USADO O TIPO ''INT'' POIS O MAIN PRECISA SABER QUANDO TROCAR DE TELA E PARA ISSO ATRIBUIMOS UM RETURN COM 0 OU 1 POR EXEMPLO
// ACREDITO QUE EU TENHA QUE POLIR MAIS UM POUCO ESSE CODIGO PARA SE ADAPATAR MELHOR AO QUE EU QUERO, MAS JA ESTA FUNCIONANDO

int run_menu_screen(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font, ALLEGRO_TIMER* timer) {
    al_set_window_title(display, "Math Monster");
    ALLEGRO_SAMPLE* musica_fundo = al_load_sample("musica_menu.ogg");
    ALLEGRO_BITMAP* background_menu = al_load_bitmap("menu_background.png");
    ALLEGRO_BITMAP* img_jogar_normal = al_load_bitmap("botao_jogar_normal.png");
    ALLEGRO_BITMAP* img_jogar_selecionado = al_load_bitmap("botao_jogar_selecionado.png");
    ALLEGRO_BITMAP* img_sair_normal = al_load_bitmap("botao_sair_normal.png");
    ALLEGRO_BITMAP* img_sair_selecionado = al_load_bitmap("botao_sair_selecionado.png");
    ALLEGRO_BITMAP* img_opcoes_normal = al_load_bitmap("botao_opcoes_normal.png");
    ALLEGRO_BITMAP* img_opcoes_selecionado = al_load_bitmap("botao_opcoes_selecionado.png");
    if (!musica_fundo) {
        fprintf(stderr, "Erro ao carregar a amostra de áudio 'musica_menu.ogg'\n");
        // apenas para resolver o erro de acarregamento da musiva( apagar futuramente0
    }
    else {
        // tocar a musica em loop
        al_play_sample(musica_fundo, 0.05, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
        //usado o 0.05 de volume, quanto mais maior o som e quanto menos menor o som
    }

    Botao botao_jogar, botao_opcoes, botao_sair; 
    MenuEstado estado_menu = MENU_JOGAR;
    inicializar_botoes(&botao_jogar, &botao_opcoes, &botao_sair, estado_menu,
        img_jogar_normal, img_jogar_selecionado,
        img_opcoes_normal, img_opcoes_selecionado,
        img_sair_normal, img_sair_selecionado);

    int executando = 1;
    int redesenhar = 1;
    ALLEGRO_EVENT evento;

    // verifica e limpa a event queue
    al_flush_event_queue(event_queue);

    while (executando) {
        al_wait_for_event(event_queue, &evento);

        switch (evento.type) {

        case ALLEGRO_EVENT_MOUSE_AXES:
            // verifica se o mouse esta sobre o botao jogar 
            if (evento.mouse.x >= botao_jogar.x && evento.mouse.x <= botao_jogar.x + botao_jogar.largura &&
                evento.mouse.y >= botao_jogar.y && evento.mouse.y <= botao_jogar.y + botao_jogar.altura)
            {
                estado_menu = MENU_JOGAR;
            }
            // verifica se o mouse esta sobre o botao opcao 
            else if (evento.mouse.x >= botao_opcoes.x && evento.mouse.x <= botao_opcoes.x + botao_opcoes.largura &&
                evento.mouse.y >= botao_opcoes.y && evento.mouse.y <= botao_opcoes.y + botao_opcoes.altura)
            {
                estado_menu = MENU_OPCOES;
            }
            // verifica se o mouse esta sobre o botao sair
            else if (evento.mouse.x >= botao_sair.x && evento.mouse.x <= botao_sair.x + botao_sair.largura &&
                evento.mouse.y >= botao_sair.y && evento.mouse.y <= botao_sair.y + botao_sair.altura)
            {
                estado_menu = MENU_SAIR;
            }
            else
            {
                estado_menu = MENU_NENHUM; // feito para remover o efeito de hover do botao quando o mouse nao estiver mais sobre o botao selecionado
            }

			// muda o estado dos botoes com base no estado_menu
            botao_jogar.selecionado = (estado_menu == MENU_JOGAR);
            botao_opcoes.selecionado = (estado_menu == MENU_OPCOES);
            botao_sair.selecionado = (estado_menu == MENU_SAIR);
            break;
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            // verificador de clique
            if (estado_menu == MENU_JOGAR) {
                
                return executar_acao_menu(estado_menu); // eetorna 1
            }
            
            else if (estado_menu == MENU_OPCOES) {
                //alterar aqui no futuro, por em quanto so adicionei para entregar um menu mais completo no marco 1
            }
            
            else if (estado_menu == MENU_SAIR) {
                return executar_acao_menu(estado_menu); // retorna 2
            }
            break; 



            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                return 2; // retorna 2

            case ALLEGRO_EVENT_TIMER:
                redesenhar = 1;
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                processar_teclado(&evento, &estado_menu, &botao_jogar, &botao_opcoes, &botao_sair);

                if (evento.keyboard.keycode == ALLEGRO_KEY_ENTER) {
                    return executar_acao_menu(estado_menu); // o estado menu vai retornar 1 ou 2, ou vai para o jogo ou vai fechar a janela
                }
                else if (evento.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
					return 2; // aparentemente ele colocou para apertar esc para sair do menu
                }
                break;
        }
       

        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = 0;
            desenhar_menu(background_menu, font, &botao_jogar, &botao_opcoes, &botao_sair);
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
    return 2; 
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

    //al_draw_text(font, al_map_rgb(255, 255, 255), LARGURA_TELA / 2, 100, removi para ficar apenas a imagem, deixei comentado pois nao quero apgar seu codigo tbm, a intencao é mostrar que isso nao deveria ser dessa forma
      //  ALLEGRO_ALIGN_CENTER, "WORLD TRAVELER");

    //ALLEGRO_FONT* font_pequena = al_create_builtin_font();
    //if (font_pequena) {
       // al_draw_text(font_pequena, al_map_rgb(200, 200, 200), LARGURA_TELA / 2, 150,
       //     ALLEGRO_ALIGN_CENTER, "Use SETAS para navegar e ENTER para selecionar");
      //  al_draw_text(font_pequena, al_map_rgb(200, 200, 200), LARGURA_TELA / 2, 170,
      //      ALLEGRO_ALIGN_CENTER, "Precione ESC para sair");
   // }

    //ALLEGRO_COLOR cor_jogar = botao_jogar->selecionado ? botao_jogar->cor_selecionado : botao_jogar->cor_normal;
    //al_draw_filled_rectangle(botao_jogar->x, botao_jogar->y,
      //  botao_jogar->x + botao_jogar->largura,
      //  botao_jogar->y + botao_jogar->altura, cor_jogar);
   // al_draw_rectangle(botao_jogar->x, botao_jogar->y,
//botao_jogar->x + botao_jogar->largura,
   //     botao_jogar->y + botao_jogar->altura,                            esse bloco tam´bem é inutil agora
    //    al_map_rgb(255, 255, 255), 2);
   // al_draw_text(font, al_map_rgb(255, 255, 255),
     //   botao_jogar->x + botao_jogar->largura / 2,
     //  botao_jogar->y + botao_jogar->altura / 2 - 12,
     //   ALLEGRO_ALIGN_CENTER, botao_jogar->texto);
     // 
     // 
     // 
    
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

    //ALLEGRO_COLOR cor_sair = botao_sair->selecionado ? botao_sair->cor_selecionado : botao_sair->cor_normal;
   // al_draw_filled_rectangle(botao_sair->x, botao_sair->y,
    //    botao_sair->x + botao_sair->largura,
    //    botao_sair->y + botao_sair->altura, cor_sair);
  //  al_draw_rectangle(botao_sair->x, botao_sair->y,
    //    botao_sair->x + botao_sair->largura,
    //    botao_sair->y + botao_sair->altura,
     //   al_map_rgb(255, 255, 255), 2);
  //  al_draw_text(font, al_map_rgb(255, 255, 255),
       // botao_sair->x + botao_sair->largura / 2,
       // botao_sair->y + botao_sair->altura / 2 - 12,
       // ALLEGRO_ALIGN_CENTER, botao_sair->texto);

   // if (botao_sair->selecionado) {
      //  al_draw_text(font, al_map_rgb(255, 255, 0),
      //      botao_sair->x - 30,
      //      botao_sair->y + botao_sair->altura / 2 - 12,
      //      ALLEGRO_ALIGN_CENTER, ">");
   // }

    //if (font_pequena) {
        //al_destroy_font(font_pequena); linha desnecessaria pois apaguei 
   // }
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
		return 3; // futuramente colocar a tela opcoes
    default:
        return 0;
    }
}