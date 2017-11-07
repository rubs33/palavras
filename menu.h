//
//  Construindo Palavras
//  Descrição: Classe de controle do Menu Principal.
//  Versão: 1.1.0.5
//
#ifndef MENU_H
#define MENU_H

#include <allegro.h>
#include "tipos.h"
using namespace std;

class Menu {
    public:
    Menu();
    ~Menu() {}
    inline void iniciar();
    inline bool definido();
    inline int get_opcao();
    inline void selecionar();
    inline void set_opcao(unsigned char op);
    inline void subir();
    inline void descer();
    void desenhar(int x, int y, DATAFILE *imgs, BITMAP *buffer);
    
    private:
    unsigned char opcao;
    int cx, cy;                          // Coordenada do cursor
    bool escolheu;                       // Selecionou uma opção
    
    static const int MENU_Y       = 180, // Posição Y do Menu
                     MENU_WIDTH   = 190, // Largura do Menu
                     MENU_HEIGHT  = 193, // Altura do Menu
                     MENU_UNIDADE =  28, // Distancia entre os itens do Menu
                     NUM_OPCOES   =   6; // Número de opções do Menu Principal
};


//
//     Construtor Padrão
//
Menu::Menu() {
    opcao = JOGAR;
    iniciar();
}


//
//     Define o menu em modo de espera
//
inline void Menu::iniciar() {
    escolheu = false;
    cx = ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 7;
    cy = MENU_Y + 18 + (opcao * MENU_UNIDADE);
}


//
//     Retorna se já foi escolhida uma opção
//
inline bool Menu::definido() {
    return escolheu;
}


//
//
//
inline int Menu::get_opcao() {
    return opcao;
}


//
//     Seleciona uma opção;
//
inline void Menu::selecionar() {
    escolheu = true;
}


//
//     Define uma opção ao menu
//
inline void Menu::set_opcao(unsigned char op) {
    opcao = op;
}


//
//     Sobe uma unidade no menu
//
inline void Menu::subir() {
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
inline void Menu::descer() {
    if (opcao == (NUM_OPCOES - 1)) {
        opcao = 0;
    } else {
        opcao += 1;
    }
    cy = MENU_Y + 18 + (opcao * MENU_UNIDADE);
}


//
//     Desenha o menu deslocado de x e y
//
void Menu::desenhar(int x, int y, DATAFILE *imgs, BITMAP *buffer) {
    masked_blit((BITMAP *)imgs[Quadro].dat, buffer, 0, 0,
         ((SCREEN_WIDTH - MENU_WIDTH) / 2) + x, MENU_Y + y,
         MENU_WIDTH, MENU_HEIGHT);
    masked_blit((BITMAP *)imgs[Ness2D1].dat, buffer, 0, 0, cx + x, cy + y,
         MENU_WIDTH, MENU_HEIGHT);
}

#endif
