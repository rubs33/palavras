//
//  Construindo Palavras
//  Descricao: Classe de controle do Menu Principal.
//  Versao: 1.2.0.0
//
#ifndef MENU_H
#define MENU_H

#include <allegro.h>

class Menu {
    public:
    Menu();
    ~Menu() {}
    void iniciar();
    bool definido();
    int get_opcao();
    void selecionar();
    void set_opcao(unsigned char op);
    void subir();
    void descer();
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

#endif
