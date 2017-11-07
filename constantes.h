//
//  Construindo Palavras
//  Descrição: Constantes do jogo.
//  Versão: 1.1.0.5
//
#ifndef CONSTANTES_H
#define CONSTANTES_H

static const int TILE_SIZE     =    24, // Tamanho dos lados das peças
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

// Opções do menu principal
enum {JOGAR, MUDAR_FASE, INSTRUCOES, CREDITOS, OPCOES, SAIR};

// Opções do menu de opções
enum {VOL_SOM, VOL_MUSICA, VEL_JOGO, OPSAIR};

// Ações do Personagem
enum {PARADO, ANDANDO, VITORIA, DORMINDO};
    
#endif
