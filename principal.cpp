//
//  Construindo Palavras
//  Descricao: Codigo Principal.
//  Plataformas: Windows e Linux
//  Versao: 1.2.0.0
//  Data de Criacao: 05/03/2005
//  Ultima Modificacao: 01/11/2026
//  Autores: Otavio Nery Cipriani  &&  Rubens Takiguti Ribeiro
//
#include "principal.h"
#include "constantes.h"
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

