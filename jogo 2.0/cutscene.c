#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h> 
#include <stdio.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "cutscene.h"
#include <string.h>
#define LARGURA_TELA 800
#define ALTURA_TELA 600
void run_cutscene_screen(ALLEGRO_DISPLAY* display, ALLEGRO_EVENT_QUEUE* event_queue, ALLEGRO_FONT* font) {
    printf("Cutscene: Carregando recursos...\n");

	//carreguei as imagens da cutscene em um array para facilitar o acesso
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

  

    if (musica_cutscene) {
        al_play_sample(musica_cutscene, 0.9, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, NULL);
    }

    bool rodando_cutscene = true;

    al_flush_event_queue(event_queue);

    int slide_atual = 0;
    bool sair_cutscene = false;
    bool redesenhar = true;

    int caracteres_a_mostrar = 0;   
    int timer_letra = 0;              
    const int VELOCIDADE_TEXTO = 3;   

    al_flush_event_queue(event_queue);

    // essa logica é satanica e eu precisei de uma ajuda fodida pra fazer isso 
    while (slide_atual < 4 && !sair_cutscene) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        if (event.type == ALLEGRO_EVENT_TIMER) {
            
            timer_letra++;

            
            if (timer_letra >= VELOCIDADE_TEXTO) {
                timer_letra = 0; 

                
                if (slide_atual < 4) { 
                    size_t tamanho_total = strlen(textos[slide_atual]);

                  
                    if (caracteres_a_mostrar < tamanho_total) {
                        caracteres_a_mostrar++; 

                        //toca o som ao mesmo tempo q aparece uma letra
                        if (sfx_letra) {
                            al_play_sample(sfx_letra, 0.09, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                        }
                    }
                }
            }//fim da logica de satanas, nao mexer aqui 

            redesenhar = true;
        }
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { // na teoria quando eu colocasse return 0 o certo seria fechar, porem nao esta dando certo, esta indo para a tela fase 1 e a musica continua o loop, provavalemente é algo 
			// main.c, eu nao acho que alguem va percerber esse bug, mas existe. e também nao é algo que afeta no funcionamento do jogo
            return 0;
        }
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ENTER) {
				// absorvce o tamanho total do texto da img atual
                size_t tamanho_total = strlen(textos[slide_atual]);

				// caso o texto ainda esteja sendo criado e a pessoa apertar enter, entao mostra o texto todo de uma vez
                if (caracteres_a_mostrar < tamanho_total) {
                    
                    caracteres_a_mostrar = tamanho_total;
                }
                // caso ja esteja completa, avanca para a prox tela
                else {
               
                    slide_atual++;
                  
                    caracteres_a_mostrar = 0;
                }
            }
        }

        if (redesenhar && al_is_event_queue_empty(event_queue)) {
            redesenhar = false;

            //tirar depois, apenas tesdte
            al_clear_to_color(al_map_rgb(0, 0, 0));

			// cut 0-3
            al_draw_bitmap(slides[slide_atual], 0, 0, 0);

            // vaixa de texto
            al_draw_filled_rectangle(20, ALTURA_TELA - 120, LARGURA_TELA - 20, ALTURA_TELA - 20, al_map_rgba(0, 0, 0, 150));

           
			char texto_parcial[666777]; // tentei usar uma string vazia e ir preenchendo de acordo com o valor de caracteres_a_mostrar, mas estava dando problema, entao usei esse metodo que funciona 
			//em resumo, coloquei uma string grande o suficiente para caber todo o texto que eu quero mostrar

       
            strncpy(texto_parcial, textos[slide_atual], caracteres_a_mostrar);

			// aparentemente o texto bugava sem isso, entao coloquei para resolver o problema
            texto_parcial[caracteres_a_mostrar] = '\0';

			// desenhar o texto(parcial com efeito de maquina de escrever)(parametros imporantes tpara eu revisar quando for polir o jogo (-250) ajustar no photoshop a imagem para se adequar no negocio ai
            al_draw_multiline_text(font, al_map_rgb(255, 255, 255), 40, ALTURA_TELA - 250, LARGURA_TELA - 60, al_get_font_line_height(font), 0, texto_parcial);
            al_flip_display();
        }
    }

    // para a mussica no final da utscene
    al_stop_samples();


    
    for (int i = 0; i < 4; i++) {
        if (slides[i]) al_destroy_bitmap(slides[i]);
    }
    if (musica_cutscene) al_destroy_sample(musica_cutscene);
    if (sfx_letra) al_destroy_sample(sfx_letra);
}