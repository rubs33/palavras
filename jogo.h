#ifndef JOGO_H
#define JOGO_H

#include <allegro.h>
#include <filesystem>
#include <vector>
#include "tipos.h"
#include "menu.h"
#include "menuopcoes.h"

class Jogo {
public:
    Jogo();
    ~Jogo();
    int carregar_recursos(std::filesystem::path caminhoBase);
    int get_operacao();
    void tocar_musica(int musica);
    void apresentacao();
    void exibe_menu();
    void apresentacao_fase(Fase &fase);
    void iniciar();
    void mudar_fase();
    void instrucoes();
    void creditos();
    void final();
    void opcoes();
    void encerrar();

private:
    Menu menu;                            // Menu Principal
    Menuopcoes menuopcoes;                // Menu de Opcoes
    Personagem personagem;                // Personagem Principal
    Fase fase;                            // Fase atual
    std::vector<std::vector<Fase>> fases; // Matriz de fases (niveis x fases)
    Habilitadas habilitadas;              // Fases habilitadas
    std::vector<Objeto> baloes;           // Vetor de baloes (final)
    BITMAP *buffer;                       // Buffer da tela
    DATAFILE *imgs, *sons;                // Arquivos de dados
    SAMPLE *musicaAtual;                  // Musica atual
    bool comSom;                          // Jogo com som
    bool comUpscale;                      // Jogo com upscale
    unsigned int realWidth;               // Largura suportada em fullscreen
    unsigned int realHeight;              // Altura suportada em fullscreen
    bool zerou;                           // Zerou o jogo
    unsigned int velPasso;                // Velocidade do personagem (pixels)
    int volMusica;                        // Volume da Musica
    int volSample;                        // Volume do Som
    int musicaTocando;                    // Musica que esta tocando
    int inicioOcioso;                     // Inicio da ociosidade
    int secreto;                          // Auxiliar que controla codigo secreto
    int tipoVideo;                        // Tipo de modo grafico usado
    int operacao;                         // Status do jogo

    void aguardar();
    bool ler_fase(std::filesystem::path caminho, Fase &fase);
    bool carregar_fases(std::filesystem::path caminhoBase);
    bool carregar_habilitadas();
    bool salvar_habilitadas();
    void alternar_tela();
    void desenhar_titulo();
    void desenhar_creditos();
    void desenhar_fundo_animado();
    void desenhar_menufases(int &faseinicio);
    void desenhar_fase();
    void desenhar_topo(unsigned int tempo, bool atualizar);
    void desenhar_personagem();
    void desenhar_baloes(std::vector<Objeto> &baloes);
    void desenhar_letra_final(BITMAP *letra, BITMAP *buffer, int pos, int i, bool terminou);
    void imprimir_tela();
    bool carregar_fase(Fase &fase, const std::vector<std::vector<Fase>> fases);
    void tocar(int som);
    bool verificar_erros();
    int tem_objeto(unsigned char blocoX, unsigned char blocoY);
    void pegar_objeto(unsigned char indice_objeto);
    bool deixar_objeto(unsigned char blocoX, unsigned char blocoY);
    bool passavel(int blocoX, int blocoY);
};

#endif
