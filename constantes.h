//
//  Construindo Palavras
//  Descri��o: Constantes do jogo.
//  Vers�o: 1.1.0.5
//
#ifndef CONSTANTES_H
#define CONSTANTES_H

static const int TILE_SIZE     =    24, // Tamanho dos lados das pe�as
                 SCREEN_WIDTH  =   640, // Largura da tela
                 SCREEN_HEIGHT =   480, // Altura da tela
                 MAX_BG_SPEED  =   187, // Maior bg_speed
                 OCIOSO        = 10000; // Tempo considerado ocioso

#ifdef WINDOWS
static const char BARRA = '\\';
#else
static const char BARRA = '/';
#endif
                 

// Retorno do Programa
enum {OK, ERRO_VIDEO, ERRO_IMAGENS, ERRO_FASES};

// Op��es do menu principal
enum {JOGAR, MUDAR_FASE, INSTRUCOES, CREDITOS, OPCOES, SAIR};

// Op��es do menu de op��es
enum {VOL_SOM, VOL_MUSICA, VEL_JOGO, OPSAIR};

// A��es do Personagem
enum {PARADO, ANDANDO, VITORIA, DORMINDO};
    
#endif
