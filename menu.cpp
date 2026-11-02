/**
 *  Construindo Palavras
 *  Descricao: Classe de controle do Menu Principal.
 */
#include "menu.h"
#include "tipos.h"
#include "imagens.h"
#include "constantes.h"

/**
 * Construtor Padrao
 */
Menu::Menu() {
    opcao = JOGAR;
    iniciar();
}

/**
 * Define o menu em modo de espera
 */
void Menu::iniciar() {
    escolheu = false;
    cx = ((SCREEN_WIDTH - MENU_WIDTH) / 2) + 7;
    cy = MENU_Y + 18 + (opcao * MENU_UNIDADE);
}

/**
 * Retorna se ja foi escolhida uma opcao
 */
bool Menu::definido() {
    return escolheu;
}

/**
 * Obtem a opcao selecionada
 */
int Menu::get_opcao() {
    return opcao;
}

/**
 * Marca que uma opcao foi seleciona
 */
void Menu::selecionar() {
    escolheu = true;
}

/**
 * Define uma opcao ao menu
 */
void Menu::set_opcao(unsigned char op) {
    opcao = op;
}

/**
 * Sobe uma unidade no menu
 */
void Menu::subir() {
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
void Menu::descer() {
    if (opcao == (NUM_OPCOES - 1)) {
        opcao = 0;
    } else {
        opcao += 1;
    }
    cy = MENU_Y + 18 + (opcao * MENU_UNIDADE);
}

/**
 * Desenha o menu deslocado de x e y
 */
void Menu::desenhar(int x, int y, DATAFILE *imgs, BITMAP *buffer) {
    // Fundo do menu
    masked_blit((BITMAP *)imgs[Quadro].dat, buffer, 0, 0,
         ((SCREEN_WIDTH - MENU_WIDTH) / 2) + x, MENU_Y + y,
         MENU_WIDTH, MENU_HEIGHT);

    // Cursor da opcao selecionada
    masked_blit((BITMAP *)imgs[Ness2D1].dat, buffer, 0, 0,
         cx + x, cy + y,
         MENU_WIDTH, MENU_HEIGHT);
}
