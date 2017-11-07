//
//  Construindo Palavras
//  Descrição: Código Principal.
//  Plataformas: Windows e Linux
//  Versão: 1.1.0.5
//  Data de Criação: 05/03/2005
//  Última Modificação: 27/10/2005
//  Autores: Otávio Nery Cipriani  &&  Rubens Takiguti Ribeiro
//
//#define WINDOWS          // descomentar para compilar em windows
#include <allegro.h>
#include "constantes.h"
#include "funcoes.h"
#include "jogo.h"
using namespace std;

int main() {

    // Carregando o jogo
    Jogo jogo; 
    
    jogo.tocar_musica(Intro);
    
    jogo.apresentacao();
    do {
        switch (jogo.get_operacao()) {
        case JOGAR:       jogo.iniciar();     break;
        case MUDAR_FASE:  jogo.mudar_fase();  break;
        case INSTRUCOES:  jogo.instrucoes();  break;
        case CREDITOS:    jogo.creditos();    break;
        case OPCOES:      jogo.opcoes();      break;
        default:          jogo.exibe_menu();  break;
        }
    } while (jogo.get_operacao() != SAIR);
    
    jogo.sair();
    
    return OK;
}
END_OF_MAIN();

