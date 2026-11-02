//
//  Construindo Palavras
//  Descricao: Funcoes gerais.
//  Versao: 1.2.0.0
//
#ifndef FUNCOES_H
#define FUNCOES_H

#include <fstream>
#include "tipos.h"

void incrementar_game_speed();
void decrementar_bg_speed();
void incrementar_clock_speed();
unsigned int wlclock();
bool passou(unsigned int t, unsigned int n);
char *converter_tempo(int intTempo, bool atualizar);
int wlrand();
bool passou(unsigned int t, unsigned int n);
unsigned int get_imagem(const char *img);
bool parede(const char *imagem);
unsigned char XtoBlocoX(unsigned int X, const Fase fase);
unsigned char YtoBlocoY(unsigned int Y, const Fase fase);
char *converter_tempo(unsigned int intTempo, bool atualizar);

#endif
