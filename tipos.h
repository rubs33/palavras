#ifndef TIPOS_H
#define TIPOS_H

#include <vector>

/**
 * OBJETO: Coordenada, Conteudo (letra) e Imagem
 */
struct Objeto {
    int blocoX, blocoY;
    unsigned char conteudo, imagem;
};

/**
 * PERSONAGEM
 */
struct Personagem {
    static const unsigned char NUM_QUADROS = 2;
    int X, Y;

    // 0 = Parado
    // 1 = Andando
    // 2 = Vitoria
    // 3 = Dormindo
    int acao;

    char sentido,
        passo;

    Objeto mala;
};

/**
 * FASE
 */
struct Fase {
    char dica[3][53];                             // 3 linhas e 52 caracteres por linha + \0
    int X, Y,                                     // coordenadas do ponto inicial da fase
        personagemX, personagemY,                 // coordenadas iniciais do personagem
        num, nivel;                               // numero da fase e nivel (1, 2 ou 3)
    std::vector<std::vector<unsigned char>> piso; // Pisos da fase
    std::vector<std::vector<bool>> passavel;      // matriz indicando onde eh passavel
    std::vector<Objeto> objetos,                  // objetos da fase (letras)
        destinos;                                 // campos verdes da fase
};


/**
 * FASES HABILITADAS
 */
struct Habilitadas {
    int nivel[3];
};

#endif
