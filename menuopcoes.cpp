/**
 * Construindo Palavras
 * Descricao: Classe de controle do Menu de Opcoes.
 */
#include "menuopcoes.h"
#include "tipos.h"
#include "constantes.h"
#include "imagens.h"

/**
 * Construtor Padrao
 */
Menuopcoes::Menuopcoes() {
    opcao = VOL_SOM;
    iniciar();
}

/**
 * Define o menu em modo de espera
 */
void Menuopcoes::iniciar() {
    sair  = false;
    cx    = ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 7;
    cy    = MENU_Y + 18;
    opcao = 0;
}

/**
 * Retorna se a opcao de sair foi escolhida
 */
bool Menuopcoes::get_sair() {
    return sair;
}

/**
 * Marca que foi escolhida a opcao de sair
 */
void Menuopcoes::set_sair() {
    sair = true;
}

/**
 * Retorna a opcao escolhida
 */
int Menuopcoes::get_opcao() {
    return opcao;
}

/**
 * Sobe uma unidade no menu
 */
void Menuopcoes::subir() {
    if (opcao == 0) {
        opcao = NUM_OPCOES - 1;
    } else {
        opcao -= 1;
    }
    cy = MENU_Y + 18 + (opcao * MENU_UNIDADE);
}

/**
 * Desce uma unidade no menu
 */
void Menuopcoes::descer() {
    if (opcao == (NUM_OPCOES - 1)) {
        opcao = 0;
    } else {
        opcao += 1;
    }
    cy = MENU_Y + 18 + (opcao * MENU_UNIDADE);
}

/**
 * Aumenta uma unidade na opcao escolhida
 */
void Menuopcoes::aumentar(int &volSample, int &volMusica, unsigned int &velJogo) {
    switch (opcao) {
    case VOL_SOM:
        if (volSample / 51 < 5) {
            volSample += 51;
        }
        break;
    case VOL_MUSICA:
        if (volMusica / 51 < 5) {
            volMusica += 51;
        }
        break;
    case VEL_JOGO:
        if (velJogo < 4) {
            velJogo *= 2;
        }
        break;
    }
    set_volume(volSample, volMusica);
}

/**
 * Diminui uma unidade na opcao escolhida
 */
void Menuopcoes::diminuir(int &volSample, int &volMusica, unsigned int &velJogo) {
    switch (opcao) {
    case VOL_SOM:
        if (volSample / 51 > 0) {
            volSample -= 51;
        }
        break;
    case VOL_MUSICA:
        if (volMusica / 51 > 0) {
            volMusica -= 51;
        }
        break;
    case VEL_JOGO:
        if (velJogo > 1) {
            velJogo /= 2;
        }
        break;
    }
    set_volume(volSample, volMusica);
}

/**
 * Desenha o menu
 */
void Menuopcoes::desenhar(DATAFILE *imgs, BITMAP *buffer, bool comSom,
    int &volSample, int &volMusica, unsigned int &velJogo
) {
    // Fundo
    masked_blit((BITMAP *)imgs[Opcoes].dat, buffer, 0, 0,
         (SCREEN_WIDTH - MENU_WIDTH) / 2, MENU_Y, MENU_WIDTH, MENU_HEIGHT);

    // Desenhar volume do Som
    if (comSom) {
        if (volSample > 0) {
            for (int j = 0; j < (volSample / 51); j++) {
                masked_blit((BITMAP *)imgs[Bola].dat, buffer, 0, 0,
                    ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159 + (30 * j),
                    MENU_Y + 18, 22, 22);
            }
        }
    } else {
        textout_ex(buffer, font, " Sem Som ",
            ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159, MENU_Y + 25,
            makecol(255,255,255), 0);
    }

    // Desenhar volume da Musica
    if (comSom) {
        if (volMusica > 0) {
            for (int j = 0; j < (volMusica / 51); j++) {
                masked_blit((BITMAP *)imgs[Bola].dat, buffer, 0, 0,
                     ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159 + (30 * j),
                     MENU_Y + 18 + MENU_UNIDADE, 22, 22);
            }
        }
    } else {
        textout_ex(buffer, font, " Sem Som ",
            ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159,
            MENU_Y + MENU_UNIDADE + 25, makecol(255,255,255), 0);
    }

    // Desenhar velocidade
    for (int j = 0; j <= (int)(velJogo / 2); j++) {
        masked_blit((BITMAP *)imgs[Bola].dat, buffer, 0, 0,
            ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 159 + (30 * j),
            MENU_Y + 18 + (2 * MENU_UNIDADE), 22, 22);
    }
    masked_blit((BITMAP *)imgs[Ness2D1].dat, buffer, 0, 0,
         cx, cy, TILE_SIZE, TILE_SIZE);
}
