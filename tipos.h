//
//  Construindo Palavras
//  Descri��o: Tipos criados para o jogo.
//  Vers�o: 1.1.0.5
//
#ifndef TIPOS_H
#define TIPOS_H

#include <vector>
using namespace std;

//
//     OBJETO: Coordenada, Conte�do (letra) e Imagem
//
struct Objeto {
    int blocoX, blocoY;
    unsigned char conteudo, imagem;
};


//
//     PERSONAGEM
//
struct Personagem {
    static const unsigned char NUM_QUADROS = 2;
    int X, Y;

    // 0 = Parado
    // 1 = Andando
    // 2 = Vit�ria
    // 3 = Dormindo
    int acao;

    char sentido,
         passo;

    Objeto mala;
};


//
//     FASE
//
struct Fase {
    char dica[3][53];              // 3 linhas e 53 caracteres por linha
    int X, Y,                      // coordenadas do ponto inicial da fase
        personagemX, personagemY,  // coordenadas iniciais do personagem
        num, nivel;                // fase e n�vel
    vector< vector<unsigned char> > piso;
    vector< vector<bool> > passavel;
    vector<Objeto> objetos, destinos;
};


//
//     FASES HABILITADAS
//
struct Habilitadas {
    int nivel[3];
}; 

#endif
