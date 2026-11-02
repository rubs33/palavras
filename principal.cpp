/**
 *  Construindo Palavras
 *  Descricao: Codigo Principal.
 *  Plataformas: Windows e Linux
 *  Versao: 1.2.0.0
 *  Data de Criacao: 05/03/2005
 *  Autores: Otavio Nery Cipriani  &&  Rubens Takiguti Ribeiro
 */
#include "constantes.h"
#include "jogo.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    Jogo jogo;

    // Preparar caminho base
    if (argc == 0) {
        std::cout << "[LOG] Falha ao obter caminho do executavel" << std::endl;
        return static_cast<int>(ERRO_CHAMADA);
    }
    fs::path exe_path = fs::absolute(argv[0]);
    fs::path exe_dir = exe_path.parent_path();

    std::cout << "[LOG] Caminho completo do executavel: " << exe_path << std::endl;
    std::cout << "[LOG] Diretorio de trabalho identificado: " << exe_dir << std::endl;

    fs::current_path(exe_dir);

    int exitStatus = jogo.carregar_recursos(exe_dir);
    if (exitStatus) {
        std::cout << "[LOG] Erro ao carregar recursos" << std::endl;
        return exitStatus;
    }

    std::cout << "[LOG] Iniciando o jogo" << std::endl;

    // Executar
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

    std::cout << "[LOG] Finalizando com sucesso" << std::endl;

    return static_cast<int>(OK);
}
END_OF_MAIN();
