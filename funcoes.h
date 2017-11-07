//
//  Construindo Palavras
//  Descrição: Funções gerais.
//  Versão: 1.1.0.5
//
#ifndef FUNCOES_H
#define FUNCOES_H

#include <fstream>
#include "constantes.h"
#include "tipos.h"
#include "imagens.h"
#include "sons.h"
using namespace std;

extern volatile unsigned int game_speed,   // Controle de velocidade do jogo
                             bg_speed;     // Controle de velocidade do fundo

//
//     Incrementa o contador de velocidade do jogo
//
inline void incrementar_game_speed() {
    game_speed++;
}
END_OF_FUNCTION(incrementar_game_speed);


//
//     Decrementa o contador de velocidade do fundo
//
inline void decrementar_bg_speed() {
    bg_speed = bg_speed ? (bg_speed - 1) : MAX_BG_SPEED;
}
END_OF_FUNCTION(decrementar_bg_speed);


//
//     Retorna o clock de acordo com o SO
//
unsigned int wlclock() {
    #ifndef WINDOWS
        return clock() / 650;
    #else
        return clock();
    #endif
}


//
//     Retorna um valor randomico entre 0 e 1000
//
int wlrand() {
    return abs(rand() % 1000);
}


//
//     Retorna se já passou n msegundos a partir de t
//
inline bool passou(unsigned int t, unsigned int n) {
    return (wlclock() > t + n);
}


//
//     Retorna o índice de uma imagem
//
unsigned int get_imagem(const char *img) {
    if (img[0] == 'F') {
        switch (img[1]) {
            case '0': return F0;
            case '1': return F1;
            case '2': return F2;
            case '3': return F3;
            case '4': return F4;
            case '5': return F5;
            case '6': return F6;
        }
    } else if (img[0] == 'N') {
        if (img[1] == '1') {
            if (!strcmp(img, "N1U1")) return Ness1U1;
            if (!strcmp(img, "N1D1")) return Ness1D1;
            if (!strcmp(img, "N1L1")) return Ness1L1;
            if (!strcmp(img, "N1R1")) return Ness1R1;
            if (!strcmp(img, "N1U2")) return Ness1U2;
            if (!strcmp(img, "N1D2")) return Ness1D2;
            if (!strcmp(img, "N1L2")) return Ness1L2;
            if (!strcmp(img, "N1R2")) return Ness1R2;
        } else if (img[1] == '0' ) {
            if (!strcmp(img, "N0U1")) return Ness0U1;
            if (!strcmp(img, "N0D1")) return Ness0D1;
            if (!strcmp(img, "N0L1")) return Ness0L1;
            if (!strcmp(img, "N0R1")) return Ness0R1;
        } else if (img[1] == '2') {
            if (!strcmp(img, "N2D1")) return Ness2D1;
        } else if (img[1] == '3') {
            if (!strcmp(img, "N3D1")) return Ness3D1;
        }
    } else if (img[0] == 'L') {
        switch (img[1]) {
            case 'A': return LA;
            case 'Á': return LA1;
            case 'Â': return LA2;
            case 'Ã': return LA3;
            case 'E': return LE;
            case 'É': return LE1;
            case 'Ê': return LE2;
            case 'I': return LI;
            case 'Í': return LI1;
            case 'O': return LO;
            case 'Ó': return LO1;
            case 'Ô': return LO2;
            case 'Õ': return LO3;
            case 'U': return LU;
            case 'Ú': return LU1;
            case 'Ü': return LU2;
            case 'B': return LB;
            case 'C': return LC;
            case 'Ç': return LC1;
            case 'D': return LD;
            case 'F': return LF;
            case 'G': return LG;
            case 'H': return LH;
            case 'J': return LJ;
            case 'K': return LK;
            case 'L': return LL;
            case 'M': return LM;
            case 'N': return LN;
            case 'P': return LP;
            case 'Q': return LQ;
            case 'R': return LR;
            case 'S': return LS;
            case 'T': return LT;
            case 'V': return LV;
            case 'X': return LX;
            case 'W': return LW;
            case 'Y': return LY;
            case 'Z': return LZ;
        }
    }
    return 0;
}


//
//     Retorna se uma imagem é parede ou chão
//
inline bool parede(const char *imagem) {
    if (imagem[0] == 'F') {
        return ((imagem[1] == '1') ||
                (imagem[1] == '2') ||
                (imagem[1] == '3') ||
                (imagem[1] == '4') ||
                (imagem[1] == '5'));
    }
    return false;
}


//
//     Converte a coordenada X (pixels) para blocos
//
inline unsigned char XtoBlocoX(unsigned int X, const Fase fase) {
    return (X - fase.X) / TILE_SIZE;
}


//
//     Converte a coordenada Y (pixels) para blocos
//
inline unsigned char YtoBlocoY(unsigned int Y, const Fase fase) {
    return (Y - fase.Y) / TILE_SIZE;
}


//
//     Converte o tempo de segundos para minutos e segundos
//
char *converter_tempo(int intTempo, bool atualizar = true) {
    static char tempo[6];

    if (!atualizar) {
        return tempo;
    }

    int seg = (wlclock() - intTempo) / 1000;
    int min = seg / 60;
    seg %= 60;

    sprintf(tempo, "%02d:%02d", min, seg);

    return tempo;
}


//
//     Imprime uma letra durante a exibição do final do jogo
//
inline void imprime_letra(BITMAP *letra, BITMAP *buffer, int pos, int i) {
    if (i > 100 + (pos * TILE_SIZE)) {
        masked_blit(letra, buffer, 0, 0, 100 + (pos * TILE_SIZE),
             400, TILE_SIZE, TILE_SIZE);
    }
}


#endif
