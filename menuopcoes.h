//
//  Construindo Palavras
//  Descrição: Classe de controle do Menu de Opções.
//  Versão: 1.1.0.5
//
#ifndef MENUOPCOES_H
#define MENUOPCOES_H

#include <allegro.h>
#include "constantes.h"
#include "tipos.h"
using namespace std;

class Menuopcoes {
    public:
    Menuopcoes();
    ~Menuopcoes() {}
    inline void iniciar();
    inline bool get_sair();
    inline void set_sair();
    inline int get_opcao();
    inline void subir();
    inline void descer();
    inline void aumentar(int &volSample, int &volMidi, unsigned int &velJogo);
    inline void diminuir(int &volSample, int &volMidi, unsigned int &velJogo);
    void desenhar(DATAFILE *imgs, BITMAP *buffer, bool comSom,
                  int &volSample, int &volMidi, unsigned int &velJogo);
    
    private:
    unsigned char opcao;
    int cx, cy;                          // Coordenada do cursor
    bool sair;                           // Selecionou uma opção
    
    static const int MENU_Y       = 200, // Posição Y do Menu
                     MENU_WIDTH   = 320, // Largura do Menu
                     MENU_HEIGHT  = 144, // Altura do Menu
                     MENU_UNIDADE =  28, // Distancia entre os itens do Menu
                     NUM_OPCOES   =   4; // Número de opções do Menu
};


//
//     Construtor Padrão
//
Menuopcoes::Menuopcoes() {
    opcao = VOL_SOM;
    iniciar();
}


//
//     Define o menu em modo de espera
//
inline void Menuopcoes::iniciar() {
    sair  = false;
    cx    = ((SCREEN_WIDTH - 320) / 2) + 7;
    cy    = 200 + 18;
    opcao = 0;
}


//
//     Retorna se já foi escolhida uma opção
//
inline bool Menuopcoes::get_sair() {
    return sair;
}


//
//     Retorna se já foi escolhida uma opção
//
inline void Menuopcoes::set_sair() {
    sair = true;
}


//
//
//
inline int Menuopcoes::get_opcao() {
    return opcao;
}


//
//     Sobe uma unidade no menu
//
inline void Menuopcoes::subir() {
    if (opcao == 0) {
        opcao = NUM_OPCOES - 1;
    } else {
        opcao -= 1;
    }
    cy = MENU_Y + 18 + (opcao * MENU_UNIDADE);
}


//
//     Desce uma unidade no menu
//
inline void Menuopcoes::descer() {
    if (opcao == (NUM_OPCOES - 1)) {
        opcao = 0;
    } else {
        opcao += 1;
    }
    cy = MENU_Y + 18 + (opcao * MENU_UNIDADE);
}


//
//     Aumenta uma unidade
//
inline void Menuopcoes::aumentar(int &volSample, int &volMidi,
                                 unsigned int &velJogo) {
    switch (opcao) {
    case VOL_SOM:
        if (volSample / 51 < 5) {
            volSample += 51;
        }
        break;
    case VOL_MUSICA:
        if (volMidi / 51 < 5) {
            volMidi += 51;
        }
        break;
    case VEL_JOGO:
        if (velJogo < 4) {
            velJogo *= 2;
        }
        break;
    }
    set_volume(volSample, volMidi);
}


//
//
//
void Menuopcoes::diminuir(int &volSample, int &volMidi, unsigned int &velJogo) {
    switch (opcao) {
    case VOL_SOM:
        if (volSample / 51 > 0) {
            volSample -= 51;
        }
        break;
    case VOL_MUSICA:
        if (volMidi / 51 > 0) {
            volMidi -= 51;
        }
        break;
    case VEL_JOGO:
        if (velJogo > 1) {
            velJogo /= 2;
        }
        break;
    }
    set_volume(volSample, volMidi);
}

//
//     Desenha o menu
//
void Menuopcoes::desenhar(DATAFILE *imgs, BITMAP *buffer, bool comSom,
                          int &volSample, int &volMidi, unsigned int &velJogo) {
    masked_blit((BITMAP *)imgs[Opcoes].dat, buffer, 0, 0,
         (SCREEN_WIDTH - MENU_WIDTH) / 2, 200, MENU_WIDTH, MENU_HEIGHT);
                 
    // Desenhar volume do Som
    if (comSom) {
        if (volSample > 0) {
            for (int j = 0; j < (volSample / 51); j++) {
                masked_blit((BITMAP *)imgs[Bola].dat, buffer, 0, 0,
                     ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159 + (30 * j),
                     200 + 18, 22, 22);
            }
        }
    } else {
        textout_ex(buffer, font, " Sem Som ",
                   ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159, 200 + 25,
                   makecol(255,255,255), 0);
    }

    // Desenhar volume da Música
     if (comSom) {
        if (volMidi > 0) {
            for (int j = 0; j < (volMidi / 51); j++) {
                masked_blit((BITMAP *)imgs[Bola].dat, buffer, 0, 0,
                     ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159 + (30 * j), 
                     200 + 18 + MENU_UNIDADE, 22, 22);
            }
        }
    } else {
        textout_ex(buffer, font, " Sem Som ",
                   ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159,
                   200 + MENU_UNIDADE + 25, makecol(255,255,255), 0);
        }

    // Desenhar velocidade
    for (int j = 0; j <= (int)(velJogo / 2); j++) {
        masked_blit((BITMAP *)imgs[Bola].dat, buffer, 0, 0,
             ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159 + (30 * j),
             200 + 18 + (2 * MENU_UNIDADE), 22, 22);
    }
    masked_blit((BITMAP *)imgs[Ness2D1].dat, buffer, 0, 0,
         cx, cy, TILE_SIZE, TILE_SIZE);
}

#endif
