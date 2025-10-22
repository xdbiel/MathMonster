#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdbool.h>

// Constantes
const int LARGURA = 800;
const int ALTURA = 600;
const char* CAMINHO_IMAGEM_FUNDO = "jogoimagem.png";
const float BOTAO_ESCALA = 1.0;
const float BOTAO_PADDING = 10;

// =========================================================================
// NOVA ESTRUTURA DE DADOS (struct)
// =========================================================================
// Agrupa todas as variáveis que definem o estado do nosso programa/jogo.
// Isso substitui todas as variáveis globais.
typedef struct {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT* font_titulo;
    ALLEGRO_FONT* font_instrucao;
    ALLEGRO_BITMAP* imagem_fundo;
    ALLEGRO_BITMAP* botao_voltar_normal;
    ALLEGRO_BITMAP* botao_voltar_selecionado;

    ALLEGRO_TIMER* timer;
    ALLEGRO_EVENT_QUEUE* event_queue;
    bool done;

    // Variáveis de estado para o botão
    bool mouse_sobre_botao;
    float botao_x;
    float botao_y;
    float botao_w; // Largura desenhada (escalada)
    float botao_h; // Altura desenhada (escalada)
    float botao_orig_w; // Largura original da imagem
    float botao_orig_h; // Altura original da imagem
} EstadoJogo;


// =========================================================================
// DECLARAÇÕES DE FUNÇÕES
// =========================================================================
// Note que agora as funções recebem um ponteiro para o EstadoJogo
int inicializar_allegro(EstadoJogo* estado);
void finalizar_allegro(EstadoJogo* estado);
void desenhar_tela_instrucoes(const EstadoJogo* estado); // const = "esta função não vai modificar o estado"
void processar_eventos(ALLEGRO_EVENT* ev, EstadoJogo* estado);


// =========================================================================
// FUNÇÃO PRINCIPAL (main)
// =========================================================================
int main(int argc, char** argv)
{
    // 1. Criamos a nossa estrutura de estado
    EstadoJogo estado;

    // 2. Inicializamos os valores padrão
    // (Ponteiros são inicializados dentro de inicializar_allegro)
    estado.done = false;
    estado.mouse_sobre_botao = false;


    // 3. Passamos um ponteiro (&estado) para a função de inicialização
    if (!inicializar_allegro(&estado)) {
        return -1;
    }

    ALLEGRO_EVENT ev;
    bool redraw = true;

    desenhar_tela_instrucoes(&estado);
    al_flip_display();

    while (!estado.done) { // Usa a variável de dentro da struct
        al_wait_for_event(estado.event_queue, &ev);

        // 4. Passamos o estado para ser modificado pelos eventos
        processar_eventos(&ev, &estado);

        // 5. Redesenha a tela
        if (redraw && al_is_event_queue_empty(estado.event_queue)) {
            redraw = false;

            // Passa o estado para a função de desenho
            desenhar_tela_instrucoes(&estado);

            al_flip_display();
        }
    } // Fim do while (!done)

    // 6. Passa o estado para a função de finalização
    finalizar_allegro(&estado);
    return 0;
}

// =========================================================================
// IMPLEMENTAÇÃO DAS FUNÇÕES
// =========================================================================

void processar_eventos(ALLEGRO_EVENT* ev, EstadoJogo* estado) {
    if (ev->type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
        estado->done = true;
    }
    else if (ev->type == ALLEGRO_EVENT_TIMER) {
        // Não precisamos fazer nada aqui, mas o loop externo vai
        // saber que precisa redesenhar (redraw = true)
        // No entanto, como a flag 'redraw' está na main,
        // vamos simplificar e já verificar por ela no loop principal.
        // O timer só serve para marcar o 'redraw' na main.
        // Esta função de evento só precisa lidar com mouse e fechar.
    }
    else if (ev->type == ALLEGRO_EVENT_MOUSE_AXES) {
        int mouse_x = ev->mouse.x;
        int mouse_y = ev->mouse.y;

        // Usa as variáveis de botão de dentro da struct 'estado'
        if (mouse_x >= (estado->botao_x + 10) &&
            mouse_x <= (estado->botao_x + estado->botao_w - 10) &&
            mouse_y >= (estado->botao_y + 10) &&
            mouse_y <= (estado->botao_y + estado->botao_h - 10))
        {
            estado->mouse_sobre_botao = true;
        }
        else {
            estado->mouse_sobre_botao = false;
        }
    }
    else if (ev->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        if (ev->mouse.button == 1 && estado->mouse_sobre_botao) {
            printf("Botao VOLTAR clicado!\n");
            estado->done = true;
        }
    }
}


// A função de desenho agora é 'const', pois ela só *lê* o estado.
void desenhar_tela_instrucoes(const EstadoJogo* estado) {
    // Acessa as variáveis usando o ponteiro 'estado->'
    if (estado->imagem_fundo) {
        al_draw_bitmap(estado->imagem_fundo, 0, 0, 0);
    }
    else {
        al_clear_to_color(al_map_rgb(0, 0, 0));
    }

    if (estado->mouse_sobre_botao) {
        al_draw_scaled_bitmap(estado->botao_voltar_selecionado,
            0, 0, estado->botao_orig_w, estado->botao_orig_h,
            estado->botao_x, estado->botao_y, estado->botao_w, estado->botao_h,
            0);
    }
    else {
        al_draw_scaled_bitmap(estado->botao_voltar_normal,
            0, 0, estado->botao_orig_w, estado->botao_orig_h,
            estado->botao_x, estado->botao_y, estado->botao_w, estado->botao_h,
            0);
    }
}


int inicializar_allegro(EstadoJogo* estado) {
    if (!al_init()) {
        fprintf(stderr, "Falha ao inicializar o Allegro!\n");
        return 0;
    }

    // (Omitindo as outras verificações de addons para encurtar...)
    al_init_font_addon();
    al_init_primitives_addon();
    al_init_image_addon();
    al_install_mouse();


    // Salva tudo dentro da struct 'estado'
    estado->display = al_create_display(LARGURA, ALTURA);
    if (!estado->display) {
        fprintf(stderr, "Falha ao criar o display!\n");
        al_uninstall_system(); return 0;
    }

    estado->timer = al_create_timer(1.0 / 60.0);
    if (!estado->timer) {
        fprintf(stderr, "Falha ao criar o timer!\n");
        finalizar_allegro(estado); return 0;
    }

    estado->event_queue = al_create_event_queue();
    if (!estado->event_queue) {
        fprintf(stderr, "Falha ao criar a fila de eventos!\n");
        finalizar_allegro(estado); return 0;
    }

    // Registra as fontes de evento
    al_register_event_source(estado->event_queue, al_get_display_event_source(estado->display));
    al_register_event_source(estado->event_queue, al_get_timer_event_source(estado->timer));
    al_register_event_source(estado->event_queue, al_get_mouse_event_source());

    al_start_timer(estado->timer);

    estado->font_titulo = al_create_builtin_font();
    estado->font_instrucao = al_create_builtin_font();

    estado->imagem_fundo = al_load_bitmap(CAMINHO_IMAGEM_FUNDO);
    if (!estado->imagem_fundo) {
        fprintf(stderr, "AVISO: Falha ao carregar a imagem de fundo '%s'.\n", CAMINHO_IMAGEM_FUNDO);
    }

    estado->botao_voltar_normal = al_load_bitmap("botao_normal.png");
    if (!estado->botao_voltar_normal) {
        fprintf(stderr, "Falha ao carregar botao_normal.png!\n");
        finalizar_allegro(estado); return 0;
    }
    estado->botao_voltar_selecionado = al_load_bitmap("botao_selecionado.png");
    if (!estado->botao_voltar_selecionado) {
        fprintf(stderr, "Falha ao carregar botao_selecionado.png!\n");
        finalizar_allegro(estado); return 0;
    }

    // Calcula a posição e tamanho do botão e salva no 'estado'
    estado->botao_orig_w = al_get_bitmap_width(estado->botao_voltar_normal);
    estado->botao_orig_h = al_get_bitmap_height(estado->botao_voltar_normal);

    estado->botao_w = estado->botao_orig_w * BOTAO_ESCALA;
    estado->botao_h = estado->botao_orig_h * BOTAO_ESCALA;

    estado->botao_x = LARGURA - estado->botao_w - BOTAO_PADDING;
    estado->botao_y = ALTURA - estado->botao_h - BOTAO_PADDING + 50;

    return 1; // Sucesso
}


void finalizar_allegro(EstadoJogo* estado) {
    // Destrói tudo usando os ponteiros de dentro do 'estado'
    if (estado->font_titulo) al_destroy_font(estado->font_titulo);
    if (estado->font_instrucao) al_destroy_font(estado->font_instrucao);
    if (estado->imagem_fundo) al_destroy_bitmap(estado->imagem_fundo);

    if (estado->botao_voltar_normal) al_destroy_bitmap(estado->botao_voltar_normal);
    if (estado->botao_voltar_selecionado) al_destroy_bitmap(estado->botao_voltar_selecionado);

    if (estado->display) al_destroy_display(estado->display);
    if (estado->timer) al_destroy_timer(estado->timer);
    if (estado->event_queue) al_destroy_event_queue(estado->event_queue);

    al_shutdown_primitives_addon();
    al_shutdown_font_addon();
    al_shutdown_image_addon();
    al_uninstall_system();
}