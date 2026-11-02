#ifndef MENUOPCOES_H
#define MENUOPCOES_H

#include <allegro.h>

class Menuopcoes {
    public:
    Menuopcoes();
    ~Menuopcoes() {}
    void iniciar();
    bool get_sair();
    void set_sair();
    int get_opcao();
    void subir();
    void descer();
    void aumentar(int &volSample, int &volMusica, unsigned int &velJogo);
    void diminuir(int &volSample, int &volMusica, unsigned int &velJogo);
    void desenhar(DATAFILE *imgs, BITMAP *buffer, bool comSom,
                  int &volSample, int &volMusica, unsigned int &velJogo);

    private:
    unsigned char opcao;
    int cx, cy;                    // Coordenada do cursor
    bool sair;                     // Saiu do menu

    static const int MENU_Y = 200, // Posicao Y do Menu
        MENU_WIDTH          = 320, // Largura do Menu
        MENU_HEIGHT         = 144, // Altura do Menu
        MENU_UNIDADE        =  28, // Distancia entre os itens do Menu
        NUM_OPCOES          =   4; // Numero de opcoes do Menu
};

#endif
