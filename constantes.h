#ifndef CONSTANTES_H
#define CONSTANTES_H

#include "imagens.h"
#include "sons.h"

static const int TILE_SIZE =    24, // Tamanho dos lados das pecas
    SCREEN_WIDTH           =   960, // Largura da tela
    SCREEN_HEIGHT          =   540, // Altura da tela
    SCREEN_WIDTH_UPSCALE   =  1920, // Largura da tela em upscale
    SCREEN_HEIGHT_UPSCALE  =  1080, // Altura da tela em upscale
    MAX_BG_SPEED           =   187, // Maior bg_speed
    OCIOSO                 =    10, // Tempo considerado ocioso (segundos)
    FPS                    =    60; // Frames por segundo

#ifdef WINDOWS
static const char BARRA = '\\';
#else
static const char BARRA = '/';
#endif

// Retorno do Programa
enum {OK, ERRO_CHAMADA, ERRO_VIDEO, ERRO_IMAGENS, ERRO_FASES};

// Opcoes do menu principal
enum {JOGAR, MUDAR_FASE, INSTRUCOES, CREDITOS, OPCOES, SAIR};

// Opcoes do menu de opcoes
enum {VOL_SOM, VOL_MUSICA, VEL_JOGO, OPSAIR};

// Acoes do Personagem
enum {PARADO, ANDANDO, VITORIA, DORMINDO};

#endif
