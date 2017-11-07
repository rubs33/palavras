//
//  Construindo Palavras
//  Descrição: Classe de controle do jogo.
//  Versão: 1.1.0.5
//
#ifndef JOGO_H
#define JOGO_Hd

#include <allegro.h>
#include "tipos.h"
#include "menu.h"
#include "menuopcoes.h"
#include "funcoes.h"
#include "imagens.h"
#include "sons.h"
using namespace std;

class Jogo {
    public:
    Jogo();
    ~Jogo() {}
    inline int get_operacao();
    inline void tocar_musica(int musica);
    void apresentacao();
    void exibe_menu();
    void apresentacao_fase(Fase &fase);
    void iniciar();
    void mudar_fase();
    void instrucoes();
    void creditos();
    void final();
    void opcoes();
    inline void sair();
    
    private:
    Menu menu;                          // Menu Principal
    Menuopcoes menuopcoes;              // Menu de Opções
    Personagem personagem;              // Personagem Principal
    Fase fase;                          // Fase atual
    vector< vector<Fase> > fases;       // Matriz de fases (niveis x fases)
    Habilitadas habilitadas;            // Fases habilitadas
    vector <Objeto> baloes;             // Vetor de balões
    BITMAP *buffer;                     // Buffer da tela
    DATAFILE *imgs, *sons;              // Arquivos de dados
    bool comSom;                        // Jogo com som
    bool zerou;                         // Zerou o jogo
    unsigned int velPasso;              // Velocidade do personagem (pixels)
    int volMidi;                        // Volume da Música
    int volSample;                      // Volume do Som
    int midiTocando;                    // Música que está tocando
    int tempoOcioso;                    // Tempo osioso no menu ou no jogo
    int inicioOcioso;                   // Inicio da ociosidade
    int secreto;                        // Auxiliar que controla códigos
    int tipoVideo;                      // Tipo de modo gráfico usado
    int operacao;                       // Status do jogo

    bool ler_fase(const char *arq, Fase &fase);
    bool carregar_fases();
    bool carregar_habilitadas();
    bool salvar_habilitadas();
    void alterna_tela();
    void desenhar_titulo();
    void desenhar_creditos();
    void desenhar_fundo_animado();
    void desenhar_menufases(int &faseinicio);
    void desenhar_fase();
    void desenhar_topo(int tempo, bool atualizar);
    void desenhar_personagem();
    void desenhar_baloes(vector<Objeto> &baloes);
    bool carregar_fase(Fase &fase, const vector< vector<Fase> > fases);
    inline void tocar(int som);
    bool verificar_erros();
    int tem_objeto(unsigned char blocoX, unsigned char blocoY);
    inline void pegar_objeto(unsigned char indice_objeto);
    bool deixar_objeto(unsigned char blocoX, unsigned char blocoY);
    bool passavel(int blocoX, int blocoY);
    
};


volatile unsigned int game_speed,   // Controle de velocidade do jogo
                      bg_speed;     // Controle de velocidade do fundo


//
//     Construtor Padrão
//
Jogo::Jogo() {
    operacao         = -1;
    int erroPrograma = 0;
    int depth        = 0;
    bool comVideo    = false;
    
    // Inicializando a Allegro, Teclado e Timer
    set_uformat(U_ASCII);
    allegro_init();
    install_keyboard();
    install_timer();

    set_window_title("Construindo Palavras");

    // Travando as variáveis e funções usadas em timers
    LOCK_VARIABLE(game_speed);
    LOCK_FUNCTION(incrementar_game_speed);
    LOCK_VARIABLE(bg_speed);
    LOCK_FUNCTION(decrementar_bg_speed);

    // Instalando as interrupções necessárias
    install_int_ex(incrementar_game_speed, BPS_TO_TIMER(60));
    install_int_ex(decrementar_bg_speed, BPS_TO_TIMER(15));
    
    // Definindo o modo gráfico
    if ((depth = desktop_color_depth()) >= 16) {
        set_color_depth(depth);
    } else {
        set_color_depth(16);
    }

    // Tenta resolução 640 x 480 px (tela cheia)
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, // WINDOWED, //
                     SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0) {
        comVideo  = true;
        tipoVideo = GFX_AUTODETECT_FULLSCREEN;
    }

    // Tenta tela cheia no modo X
    #ifdef GFX_XWINDOWS_FULLSCREEN
    if (!comVideo) {
        if (set_gfx_mode(GFX_XWINDOWS_FULLSCREEN,
                         SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0) {
            comVideo  = true;
            tipoVideo = GFX_XWINDOWS_FULLSCREEN;
        }
    }
    #endif

    // Tenta modo VGA
    #ifdef GFX_VGA
    if (!comVideo) {
        if (set_gfx_mode(GFX_VGA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0) {
            comVideo  = true;
            tipoVideo = GFX_VGA;
        }
    }
    #endif

    // Tenta modo janela
    if (!comVideo) {
        if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,
                         SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0) {
            comVideo  = true;
            tipoVideo = GFX_AUTODETECT_WINDOWED;
        }
    }
    
    // Tenta resolução segura (melhor que nada...)
    if (!comVideo) {
        if (set_gfx_mode(GFX_SAFE, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0) {
            comVideo  = true;
            tipoVideo = GFX_SAFE;
        }
    }
    
    // Não deu...     :(
    if (!comVideo) {
        allegro_message("Erro ao definir modo grafico");
        erroPrograma = ERRO_VIDEO;
    }

    // Definir som
    int tipoMidi   = MIDI_AUTODETECT;
    int tipoSample = DIGI_AUTODETECT;

    // Detectar tipo de Midi
    if (detect_midi_driver(MIDI_AUTODETECT)) {
        tipoMidi = MIDI_AUTODETECT;
    }
    #ifdef MIDI_WIN32MAPPER
    else if (detect_midi_driver(MIDI_WIN32MAPPER)) {
        tipoMidi = MIDI_WIN32MAPPER;
    }
    #endif
    #ifdef MIDI_DIGMID
    else if (detect_midi_driver(MIDI_DIGMID)) {
        tipoMidi = MIDI_DIGMID;
    }
    #endif
    #ifdef MIDI_OSS
    else if (detect_midi_driver(MIDI_OSS)) {
        tipoMidi = MIDI_OSS;
    }
    #endif
    #ifdef MIDI_ALSA
    else if (detect_midi_driver(MIDI_ALSA)) {
        tipoMidi = MIDI_ALSA;
    }
    #endif
    
    // Detectar tipo de Sample
    if (detect_digi_driver(DIGI_AUTODETECT)) {
        tipoSample = DIGI_AUTODETECT;
    }
    #ifdef DIGI_OSS
    else if (detect_digi_driver(DIGI_OSS)) {
        tipoSample = DIGI_OSS;
    }
    #endif
    #ifdef DIGI_ESD
    else if (detect_digi_driver(DIGI_ESD)) {
        tipoSample = DIGI_ESD;
    }
    #endif
    #ifdef DIGI_ARTS
    else if (detect_digi_driver(DIGI_ARTS)) {
        tipoSample = DIGI_ARTS;
    }
    #endif
    #ifdef DIGI_ALSA
    else if (detect_digi_driver(DIGI_ALSA)) {
        tipoSample = DIGI_ALSA;
    }
    #endif
    #ifdef DIGI_JACK
    else if (detect_digi_driver(DIGI_JACK)) {
        tipoSample = DIGI_JACK;
    }
    #endif
    comSom = (install_sound(tipoSample, tipoMidi, 0) == 0);
    
    // Carregando o arquivo de imagens
    this->imgs = load_datafile("Imagens.dat");
    if (imgs == NULL) {
        allegro_message("Erro ao carregar o arquivo \"Imagens.dat\".");
        erroPrograma = ERRO_IMAGENS;
    }

    // Carregando o arquivo de sons
    this->sons = load_datafile("Sons.dat");
    if ((sons == NULL) && (comSom)) {
        volSample = 0;
        volMidi   = 0;
        comSom    = false;
    } else {
        volSample = 204;
        volMidi   = 204;
    }
    set_volume(volSample, volMidi);
    
    // Carregando o arquivo de fases
    if (!carregar_fases()) {
        allegro_message("Erro ao carregar o arquivo \"Fases.dat\".");
        erroPrograma = ERRO_FASES;
    }
    carregar_habilitadas();

    // Criando um buffer para guardar as imagens a serem exibidas na tela
    this->buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Inicializa o jogo
    fase.num   = 1;
    fase.nivel = 1;
    velPasso   = 1;
    zerou      = false;
}


//
//     Lê a fase do arquivo texto
//
bool Jogo::ler_fase(const char *arq, Fase &fase) {
    char aux[60];
    int largura, altura;
    ifstream arquivo_texto(arq);

    if (!arquivo_texto) {
        return false;
    }

    // Lendo o tamanho da fase
    arquivo_texto >> largura;
    arquivo_texto >> altura;

    // Redimensionando as matrizes
    fase.piso.resize(largura);
    fase.passavel.resize(largura);
    for (int j = largura - 1; j >= 0; j--) {
        fase.piso[j].resize(altura);
        fase.passavel[j].resize(altura);
    }

    // Lendo o layout da fase
    for (int h = 0; h < altura; h++) {
        for (int w = 0; w < largura; w++) {
            arquivo_texto >> aux;
            fase.piso[w][h] = get_imagem(aux);
            fase.passavel[w][h] = !parede(aux);
        }
    }

    // Calculando as coordenadas de origem da fase
    fase.X = ((SCREEN_WIDTH - (largura * TILE_SIZE)) / 2);
    fase.Y = ((SCREEN_HEIGHT - (altura * TILE_SIZE)) / 2) + 45;

    // Lê as coordenadas iniciais do personagem
    arquivo_texto >> aux;
    fase.personagemX = fase.X + atoi(aux) * TILE_SIZE;
    arquivo_texto >> aux;
    fase.personagemY = fase.Y + atoi(aux) * TILE_SIZE;

    // Lendo os objetos da fase
    arquivo_texto >> aux;
    fase.objetos.resize(atoi(aux));
    for (int j = (fase.objetos.size() - 1); j >= 0; j--) {
        arquivo_texto >> aux;
        fase.objetos[j].imagem = get_imagem(aux);
        fase.objetos[j].conteudo = aux[1];
        arquivo_texto >> fase.objetos[j].blocoX;
        arquivo_texto >> fase.objetos[j].blocoY;
    }

    // Lendo o conteúdo dos blocos verdes
    arquivo_texto >> aux;
    fase.destinos.resize(atoi(aux));
    for (int j = (fase.destinos.size() - 1); j >= 0; j--) {
        fase.destinos[j].imagem = F6;
        arquivo_texto >> fase.destinos[j].conteudo;
        arquivo_texto >> fase.destinos[j].blocoX;
        arquivo_texto >> fase.destinos[j].blocoY;
    }

    // Lendo a dica das palavras
    arquivo_texto.ignore();
    for (int j = 0; j < 3; j++ ) {
        aux[0] = '\0';
        if (!arquivo_texto.eof()) arquivo_texto.getline(aux, 60);
        strcpy(fase.dica[j], aux);
    }

    arquivo_texto.close();

    return true;
}


//
//     Lê os arquivos para montar os vetores de fases
//
bool Jogo::carregar_fases() {
    bool lendoFase = true;
    Fase fase;
    char nivel = '0', nomeArquivo[14] = "Fases|n00.txt";
    
    // Matriz 3 x N (3 níveis / N fases por nível)
    fases.resize(3);

    // Carregando fases fáceis, médias e difíceis
    for (int i = 1; i <= 3; i++) {
        fase.num   = 1;
        fase.nivel = i;
        switch (i) {
            case 1: nivel = 'f'; break;
            case 2: nivel = 'm'; break;
            case 3: nivel = 'd'; break;
        }
        do {
            sprintf(nomeArquivo, "Fases%c%c%02d.txt", BARRA, nivel, fase.num);
            if (lendoFase = ler_fase(nomeArquivo, fase)) {
                fases[fase.nivel - 1].push_back(fase);
                fase.num += 1;
            }
        } while(lendoFase);
    }

    return true;
}


//
//     Lê arquivo tipado de fases habilitadas
//
bool Jogo::carregar_habilitadas() {
    ifstream arquivoLeitura("Fases.dat", ios::binary);
    if (arquivoLeitura.good()) {
        arquivoLeitura.read(reinterpret_cast<char *>(&habilitadas),
             sizeof(habilitadas));
        arquivoLeitura.close();
        return true;
    }
    habilitadas.nivel[0] = 1;
    habilitadas.nivel[1] = 1;
    habilitadas.nivel[2] = 1;
    return false;
}


//
//     Salva fases habilitadas em arquivo tipado
//
bool Jogo::salvar_habilitadas() {
    ofstream arquivoEscrita("Fases.dat", ios::binary);
    if (arquivoEscrita.good()) {
        arquivoEscrita.write(reinterpret_cast<char *>(&habilitadas),
             sizeof(habilitadas));
        arquivoEscrita.close();
        return true;
    }
    return false;
}


//
//     Ajusta a tela para tela cheia ou janela
//
void Jogo::alterna_tela() {
    if (is_windowed_mode()) {
        set_gfx_mode(tipoVideo, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    } else {
        set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT,
             0, 0);
    }
}


//
//     Retorna a operação a ser realizada
//
inline int Jogo::get_operacao() {
    return operacao;
}


//
//     Toca uma música
//
inline void Jogo::tocar_musica(int musica) {
    if (midiTocando != musica) {
        midiTocando = musica;
        play_midi((MIDI *)sons[musica].dat, 1);
    }
}


//
//     Apresentação inicial
//
void Jogo::apresentacao() {
    clear_keybuf();
    int i = -144;
    game_speed = 0;
    while (i < 0) {

        // Desenhar Apresentação
        acquire_screen();
        desenhar_fundo_animado();
        blit((BITMAP *)imgs[Titulo].dat, buffer, 0, 0,
             (SCREEN_WIDTH - 360) / 2, i, 360, 144);
        desenhar_baloes(baloes);
        menu.desenhar(3 * i, -2 * i, imgs, buffer);
        masked_blit((BITMAP *)imgs[Creditos].dat, buffer, 0, 0,
             (SCREEN_WIDTH - 179) - i,
             (SCREEN_HEIGHT - 50) - (i / 2), 215, 50);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        release_screen();

        while (game_speed > 0) {
            i += 2;

            // Tratar Teclado
            if (keypressed()) {
                tocar(MoveMenu);
                switch (readkey() >> 8) {
                
                case KEY_UP:
                    menu.subir();
                    break;
                    
                case KEY_DOWN:
                    menu.descer();
                    break;
                
                case KEY_ENTER:
                    menu.selecionar();
                    operacao = menu.get_opcao();
                    return;
                    break;
                
                case KEY_ESC:
                    return;
                    break;
                }
            }

            game_speed--;
        } // while

    } // for
}


//
//     Imprime o menu enquanto nnão seleciona uma opção
//
void Jogo::exibe_menu() {
    bool exibindoDica = false;
    const int velDica = 6;
    int x = -180,
        y = SCREEN_HEIGHT - 50;
    menu.iniciar();
    inicioOcioso = wlclock();
    game_speed   = 0;
    
    // Enquanto não selecionou uma opção
    while (!menu.definido()) {
        tempoOcioso = wlclock() - inicioOcioso;
        if (tempoOcioso >= OCIOSO) {
            exibindoDica = true;
            if ((exibindoDica) && (x < 0)) {
                x += velDica;
            }
            if (x > 0) {
                x = 0;
            }
        } else {
            if ((exibindoDica) && (x > -180)) {
                x -= velDica;
            } else if (x < -180) {
                x = -180;
                exibindoDica = false;
            }
        }
        
        // Desenhar Menu
        acquire_screen();
        desenhar_fundo_animado();
        desenhar_titulo();
        desenhar_baloes(baloes);
        menu.desenhar(0, 0, imgs, buffer);
        desenhar_creditos();
        masked_blit((BITMAP *)imgs[DicaMenu].dat, buffer, 0, 0, x, y, 360, 168);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        release_screen();
        
        while (game_speed > 0) {
        
            // Tratar Teclado
            if (keypressed()) {
                inicioOcioso = wlclock();
                
                switch (readkey() >> 8) {
                case KEY_UP:
                    menu.subir();
                    tocar(MoveMenu);
                    break;
                    
                case KEY_DOWN:
                    menu.descer();
                    tocar(MoveMenu);
                    break;
                    
                case KEY_ENTER:
                    menu.selecionar();
                    tocar(MoveMenu);
                    break;
                    
                case KEY_F1:
                    alterna_tela();
                    break;
                    
                case KEY_F2:
                    menu.set_opcao(INSTRUCOES);
                    menu.selecionar();
                    tocar(MoveMenu);
                    break;
                default:
                    tocar(Erro);
                    break;
                }
            }
            
            game_speed--;
        } // while
    } // while

    operacao = menu.get_opcao();
}


//
//     Mostra a apresentação da fase
//
void Jogo::apresentacao_fase(Fase &fase) {
    const int vel        = 3 * velPasso, // Velocidade de descida
              atrasoGiro = 4,            // Atraso do giro
              idica      = (TILE_SIZE + fase.personagemY) / 3;
    int i = 0;
    
    personagem.acao          = PARADO;
    personagem.mala.conteudo = 0;       // Nada na mala
    personagem.mala.imagem   = 0;
    personagem.X             = fase.personagemX;

    personagem.Y = -1 * TILE_SIZE;
    game_speed   = 0;
    while (personagem.Y < fase.personagemY) {
    
        // Desenhar jogo
        acquire_screen();
        desenhar_fundo_animado();
        desenhar_fase();
        
        // Desenhar Topo
        masked_blit((BITMAP *)imgs[Topo].dat, buffer, 0, 0, 10, 10, 620, 82);

        // Imprimir Fase, Nível e Tempo
        textprintf_ex(buffer, font, 583, 34, makecol(255, 255, 0), 0,
                      "%02d", fase.num);
        textprintf_ex(buffer, font, 583, 52, makecol(255, 255, 0), 0,
                      "%d/3", fase.nivel);
        textout_ex(buffer, font, "00:00", 583, 71, makecol(255, 255, 0), 0);

        // Imprimir Dica
        for (int j = 0; j < (personagem.Y / idica); j++) {
            textout_centre_ex(buffer, font, fase.dica[j], 305,
                              45 + (j * 13), -1, 0);
        }
        
        desenhar_personagem();
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        release_screen();
        
        while (game_speed > 0) {
            personagem.Y += vel;
            i++;
                    
            switch ((i % (4 * atrasoGiro)) / atrasoGiro) {
            case 0: personagem.sentido = 'D'; break;
            case 1: personagem.sentido = 'R'; break;
            case 2: personagem.sentido = 'U'; break;
            case 3: personagem.sentido = 'L'; break;
            }
            
            if (keypressed()) {
                personagem.Y = fase.personagemY;
            }
            game_speed--;
        }
    }

}


//
//     Inicia uma fase
//
void Jogo::iniciar() {

    bool acabou        = false;   // Terminou a fase
    bool sair          = false;   // Sair da fase sem ter terminado
    bool comInstrucoes = false;   // Exibindo instruções
    bool errou         = false;   // Errou a palavra
    bool tocouErro     = false;   // Tocou o som de erro quando errou
    int tempoJogo      = 0;       // Tempo gasto na fase
    
    // Se não é uma fase habilitada, voltar pra primeira habilitada
    if (habilitadas.nivel[fase.nivel - 1] < fase.num) {
        fase.num = habilitadas.nivel[fase.nivel - 1];
    }
        
    // Se zerou, mostra os créditos e depois o final
    if (!carregar_fase(fase, fases)) {
        zerou = true;
        operacao = CREDITOS;
        return;
    }
    
    // Tocar a música do nível correspondente
    switch (fase.nivel) {
    case 1: tocar_musica(MF); break;
    case 2: tocar_musica(MM); break;
    case 3: tocar_musica(MD); break;
    }

    // Mostrar apresentação da fase
    apresentacao_fase(fase);
    
    // Inicializar personagem na fase
    personagem.acao          = PARADO;  // Fazendo nada
    personagem.sentido       = 'D';     // Olhando para baixo
    personagem.mala.conteudo = 0;       // Nada na mala
    personagem.mala.imagem   = 0;
    personagem.X             = fase.personagemX;
    personagem.Y             = fase.personagemY;
    personagem.passo         = TILE_SIZE;
    
    // Inicializar a fase
    tempoJogo      = wlclock();
    inicioOcioso   = wlclock();
    game_speed     = 0;
    tempoOcioso    = 0;
    secreto        = 0;
    comInstrucoes  = false;
    errou          = false;
    clear_keybuf();
    
    // Jogo propriamente dito
    while (!sair && !acabou) {
    
        // Desenhar jogo
        acquire_screen();
        desenhar_fundo_animado();
        desenhar_fase();
        desenhar_topo(tempoJogo, true);
        desenhar_personagem();

        // Se errou, informar para continuar tentando
        if (errou) {
            masked_blit((BITMAP *)imgs[Continue].dat, buffer, 0, 0,
                 (SCREEN_WIDTH - 214) / 2, 100, 214, 19);
            if (!tocouErro) {
                tocar(Erro);
                tocouErro = true;
            }
        }

        // "F2: Instruções" no canto direito
        if ((tempoOcioso < OCIOSO) || ((tempoOcioso / 500) % 2)) {
            masked_blit((BITMAP *)imgs[InstrucoesCanto].dat, buffer, 0, 0, 
                 SCREEN_WIDTH - (106 + 10), SCREEN_HEIGHT - (14 + 10),
                 106, 14);
        }

        // Instruções
        if (comInstrucoes) {
            masked_blit((BITMAP *)imgs[Instrucoes].dat, buffer, 0, 0,
                 (SCREEN_WIDTH - 339) / 2, (SCREEN_HEIGHT - 266) / 2,
                 339, 266);
        }

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        release_screen();
    
        while ((game_speed > 0)) {
            tempoOcioso = wlclock() - inicioOcioso;

            // Se o personagem está parado
            if (personagem.acao != ANDANDO) {

                personagem.passo = TILE_SIZE;

                // posBloco indica a coordenada a frente do personagem
                unsigned char posBlocoY = YtoBlocoY(personagem.Y, fase),
                              posBlocoX = XtoBlocoX(personagem.X, fase);

                // Acima
                if (personagem.sentido == 'U') {
                    posBlocoY -= 1;

                // Abaixo
                } else if (personagem.sentido == 'D') {
                    posBlocoY += 1;

                // À esquerda
                } else if (personagem.sentido == 'L') {
                    posBlocoX -= 1;

                // À direita
                } else if (personagem.sentido == 'R') {
                    posBlocoX += 1;
                }

                if (keypressed()) {
                    switch (readkey() >> 8) {

                    // Subir
                    case KEY_UP:
                        inicioOcioso = wlclock();
                        if (passavel(posBlocoX, posBlocoY)) {
                            personagem.acao = ANDANDO;
                            if (personagem.sentido == 'U') {
                                personagem.Y -= velPasso;
                            }
                        } else {
                            personagem.acao = PARADO;
                        }
                        personagem.sentido = 'U';
                        break;
                        
                    // Descer
                    case KEY_DOWN:
                        inicioOcioso = wlclock();
                        if (passavel(posBlocoX, posBlocoY)) {
                            personagem.acao = ANDANDO;
                            if (personagem.sentido == 'D') {
                                personagem.Y += velPasso;
                            }
                        } else {
                            personagem.acao = PARADO;
                        }
                        personagem.sentido = 'D';
                        break;
                        
                    // Esquerda
                    case KEY_LEFT:
                        inicioOcioso = wlclock();
                        if (passavel(posBlocoX, posBlocoY)) {
                            personagem.acao = ANDANDO;
                            if (personagem.sentido == 'L') {
                                personagem.X -= velPasso;
                            }
                        } else {
                            personagem.acao = PARADO;
                        }
                        personagem.sentido = 'L';
                        break;
                        
                    // Direita
                    case KEY_RIGHT:
                        inicioOcioso = wlclock();
                        if (passavel(posBlocoX, posBlocoY)) {
                            personagem.acao = ANDANDO;
                            if (personagem.sentido == 'R') {
                                personagem.X += velPasso;
                            }
                        } else {
                            personagem.acao = PARADO;
                        }
                        personagem.sentido = 'R';
                        break;
                        
                    // Pegar / Soltar
                    case KEY_SPACE:
                        inicioOcioso = wlclock();
                        personagem.acao = PARADO;
                        
                        // Se a mala está cheia
                        if (personagem.mala.conteudo) {

                            // Soltar objeto
                            if (passavel(posBlocoX, posBlocoY)) {
                                acabou = deixar_objeto(posBlocoX, posBlocoY);
                                     tocar(Toc);
                            } else {
                                tocar(Erro);
                            }
                                
                            // Verificar se errou a palavra
                            if (!acabou) {
                                errou     = verificar_erros();
                                tocouErro = false;
                            }

                        // Se a mala está vazia
                        } else {
                            char indice_objeto;
                                
                            // Pegar objeto
                            indice_objeto = tem_objeto(posBlocoX, posBlocoY);
                            if (indice_objeto >= 0) {
                                pegar_objeto(indice_objeto);
                                tocar(Toc);
                            } else {
                                tocar(Erro);
                            }

                        }
                        break;

                    // Alterna entre janela e tela cheia
                    case KEY_F1:
                        alterna_tela();
                        break;
                    
                    // Mostra instruções
                    case KEY_F2:
                        comInstrucoes = !comInstrucoes;
                        break;
                        
                    // Apara as instruções ou volta pro menu inicial
                    case KEY_ESC:
                        if (comInstrucoes) {
                            comInstrucoes = false;
                        } else {
                            operacao = -1;
                            sair     = true;
                        }
                        break;
                        
                    // Personagem faz "vitória"
                    case KEY_V:
                        inicioOcioso    = wlclock();
                        personagem.acao = VITORIA;
                        break;
                        
                    // Código secreto: "ZERA" durante o jogo
                    case KEY_Z:
                        secreto = 1;
                        break;
                    case KEY_E:
                        secreto = (secreto == 1) ? 2 : 0;
                        break;
                    case KEY_R:
                        secreto = (secreto == 2) ? 3 : 0;
                        break;
                    case KEY_A:
                        secreto = (secreto == 3) ? 4 : 0;
                        break;
                    
                    default:
                        secreto = 0;
                        break;
                    }
                    
                    clear_keybuf();
                    
                // Se não pressionou nenhuma tecla
                } else {
                
                    // Se ficou muito tempo parado
                    if (tempoOcioso > OCIOSO) {
                        personagem.acao = DORMINDO; // Dorme
                    }
                }
                                    
            // Se o personagem não está parado
            } else {
            
                clear_keybuf();
                    
                // Se não está em posição discreta, continuar andando
                if ((personagem.sentido == 'U') && 
                    ((personagem.Y - fase.Y) % TILE_SIZE)) {
                    personagem.Y     -= velPasso;
                    personagem.passo -= velPasso;
                } else if ((personagem.sentido == 'L') &&
                           ((personagem.X - fase.X) % TILE_SIZE)) {
                    personagem.X     -= velPasso;
                    personagem.passo -= velPasso;
                } else if ((personagem.sentido == 'R') &&
                           ((personagem.X - fase.X) % TILE_SIZE)) {
                    personagem.X     += velPasso;
                    personagem.passo -= velPasso;
                } else if ((personagem.Y - fase.Y) % TILE_SIZE) {
                    personagem.Y     += velPasso;
                    personagem.passo -= velPasso;

                // Se está em posição discreta, então parar
                } else {
                    personagem.acao = PARADO;
                    inicioOcioso    = wlclock();
                }
                
                if (personagem.passo <= (int)velPasso) {
                    personagem.passo = TILE_SIZE;
                    tocar(Passo);
                }
            }

            // Se digitou o código secreto
            if (secreto == 4) {
                tocar(Toc);
                        
                // Habilitar todas as fases e zerar
                habilitadas.nivel[0] = 99;
                habilitadas.nivel[1] = 99;
                habilitadas.nivel[2] = 99;
                zerou   = true;
                secreto = 0;
            }
            game_speed--;

        } // while game_speed
    } // while principal

    // Se terminou a fase
    if (acabou) {
            
        // Habilitar a fase seguinte, caso não esteja habilitada
        if (habilitadas.nivel[fase.nivel - 1] < (fase.num + 1)) {
            habilitadas.nivel[fase.nivel - 1] = fase.num + 1;
        }
        // Desenha Vitória
        personagem.acao = VITORIA;
        int t = wlclock();
        while (! passou(t, 1700)) {
            acquire_screen();
            desenhar_fundo_animado();
            desenhar_fase();
            desenhar_topo(tempoJogo, false);
            desenhar_personagem();
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            release_screen();
            if (keypressed()) {
                break;
            }
        }

        // Parabéns
        bool exibindo = true;
        int cor       = 0;
        inicioOcioso  = wlclock();
        tempoOcioso   = 0;
        while (exibindo) {
            tempoOcioso = wlclock() - inicioOcioso;

            // Desenha Parabéns
            acquire_screen();
            desenhar_fundo_animado();
            desenhar_topo(tempoJogo, false);
            masked_blit((BITMAP *)imgs[Parabens].dat, buffer, 0, 0,
                 (SCREEN_WIDTH - 342) / 2, 200, 342, 151);
            if ((tempoOcioso >= OCIOSO) && ((tempoOcioso / 500) % 2)) {
                cor = makecol(255, 255, 0);
            } else {
                cor = makecol(255, 255, 255);
            }
            textout_ex(buffer, font, "Aperte Enter para continuar",
                 ((SCREEN_WIDTH - 342) / 2) + 70 , 
                 200 + 125, cor, 0);
            blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            release_screen();

            if (keypressed()) {

                switch (readkey() >> 8) {

                // Esperar apertar Enter
                case KEY_ENTER:
                    fase.num += 1;
                    acabou   = false;
                    exibindo = false;
                    operacao = JOGAR;
                    break;
                    
                // Ou sair apertando Esc
                case KEY_ESC:
                    fase.num += 1;
                    acabou   = false;
                    exibindo = false;
                    operacao = -1;
                    break;

                case KEY_F1:
                    alterna_tela();
                    break;
                }
            }
        } // while
    }
}


//
//     Seleciona fases
//
void Jogo::mudar_fase() {
    bool selecionou = false;
    int faseinicio  = 0;
    operacao        = -1;
    game_speed      = 0;
    clear_keybuf();
    
    faseinicio = (fase.num < 17) ? 0 : fase.num - 17;
    
    // Desenhar menu de fases
    while (!selecionou) {

        // Desenhar
        acquire_screen();
        desenhar_fundo_animado();
        desenhar_titulo();
        desenhar_baloes(baloes);
        desenhar_menufases(faseinicio);
        desenhar_creditos();
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        release_screen();

        while (game_speed > 0) {

            // Tratar Teclado            
            if (keypressed()) {
                switch (readkey() >> 8) {

                case KEY_ENTER:
                    
                    // Se é uma fase habilitada: iniciar fase
                    if (fase.num <= habilitadas.nivel[fase.nivel - 1]) {
                        tocar(MoveMenu);
                        selecionou = true;
                        operacao   = JOGAR;
                        
                    // Se não é uma fase habilitada: bloquear
                    } else {
                        tocar(Erro);
                    }
                    break;
                
                case KEY_ESC:
                    tocar(MoveMenu);
                    selecionou = true;
                    operacao   = -1;
                    break;
                
                case KEY_UP:
                    if (fase.num > 1) {
                        tocar(MoveMenu);
                        fase.num -= 1;
                        if (fase.num == faseinicio) {
                            faseinicio -= 1;
                        }
                    } else {
                        tocar(Erro);
                    }
                    break;
                
                case KEY_PGUP:
                    if (fase.num > 1) {
                        tocar(MoveMenu);
                        fase.num   = 1;
                        faseinicio = 0;
                    } else {
                        tocar(Erro);
                    }
                    break;
                                
                case KEY_DOWN:
                    if (fase.num < (int)fases[fase.nivel - 1].size()) {
                        tocar(MoveMenu);
                        if (fase.num == faseinicio + 17) {
                            faseinicio += 1;
                        }
                        fase.num += 1;
                    } else {
                        tocar(Erro);
                    }
                    break;
                
                case KEY_PGDN:
                    if (fase.num < (int)fases[fase.nivel - 1].size()) {
                        tocar(MoveMenu);
                        fase.num   = fases[fase.nivel - 1].size();
                        faseinicio = (fase.num <= 17) ? 0 : fase.num % 17;
                    } else {
                        tocar(Erro);
                    }
                    break;
                
                case KEY_RIGHT:
                    if (fase.nivel < 3) {
                        tocar(MoveMenu);
                        fase.nivel += 1;
                        if ((int)fases[fase.nivel - 1].size() < fase.num) {
                            fase.num = fases[fase.nivel - 1].size();
                            faseinicio = (fase.num <= 17) ? 0 : fase.num % 17;
                        }
                    } else {
                        tocar(Erro);
                    }
                    break;
                
                case KEY_LEFT:
                    if (fase.nivel > 1) {
                        tocar(MoveMenu);
                        fase.nivel -= 1;
                        if ((int)fases[fase.nivel - 1].size() < fase.num) {
                            fase.num = fases[fase.nivel - 1].size();
                        }
                        faseinicio = (fase.num <= 17) ? 0 : fase.num % 17;
                    } else {
                        tocar(Erro);
                    }
                    break;
                }

            } // if
            game_speed--;
        } // while
    } // while
}


//
//     Exibe as instrucoes
//
void Jogo::instrucoes() {
    bool exibindo = true;
    operacao   = -1;
    game_speed = 0;
    clear_keybuf();
    
    // Desenhar Instruções
    while (exibindo) {
    
        // Desenhar
        acquire_screen();
        desenhar_fundo_animado();
        desenhar_titulo();
        desenhar_baloes(baloes);
        masked_blit((BITMAP *)imgs[Instrucoes].dat, buffer, 0, 0,
             (SCREEN_WIDTH - 339) / 2, 150, 339, 266);
        desenhar_creditos();
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        release_screen();
        
        // Tratar Teclado
        while (game_speed > 0) {
            if (keypressed()) {
                switch (readkey() >> 8) {
                case KEY_ENTER:
                case KEY_ESC:
                case KEY_F2:
                    tocar(MoveMenu);
                    exibindo = false;
                    break;
                case KEY_F1:
                    alterna_tela();
                    break;
                }
            }
            game_speed--;
        } // while
    } // while
}


//
//     Exibe os créditos
//
void Jogo::creditos() {
    operacao = -1;
    
    tocar_musica(MFinal);
    clear_keybuf();
       
    // Passa os créditos
    int auxy   = SCREEN_HEIGHT,
        i      = 0;
    game_speed = 0;
    while (i <= 716 + SCREEN_HEIGHT) {

        // Desenhar Créditos
        acquire_screen();
        desenhar_fundo_animado();
        masked_blit((BITMAP *)imgs[Final].dat, buffer, 0, 0,
             (SCREEN_WIDTH - 316) / 2, auxy, 316, 716);
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        release_screen();

        // Tratar Teclado
        while (game_speed > 0) {
            i++;
            auxy = SCREEN_HEIGHT - i;
            if (keypressed()) {

                switch (readkey() >> 8) {

                case KEY_ESC:
                case KEY_ENTER:
                    i = 716 + SCREEN_HEIGHT;
                    break;

                case KEY_DOWN:
                    i += 10;
                    break;
                    
                // Voltar os créditos
                case KEY_UP:
                    i -= 15;
                    break;
                }

            } // if
            game_speed--;
        } // while
    }
    
    if (zerou) {
        final();
    }
    
    apresentacao();
}


//
//     Exibe o final
//
void Jogo::final() {
    fase.num   = 1;
    fase.nivel = 1;
        
    // Mostra o Final
    personagem.acao    = 1;
    personagem.sentido = 'R';
    personagem.Y       = 400;
    personagem.passo   = TILE_SIZE;

    int i      = 0,
        s      = 0;
    game_speed = 0;
    while (!s) {
            
        // Criar balão em posição aleatória
        if  (baloes.size() < 15) {
            Objeto b;
            b.blocoX = (SCREEN_WIDTH / 15) * baloes.size();
            b.blocoY = (wlrand() % SCREEN_HEIGHT) + SCREEN_HEIGHT;

            if (baloes.size()) {
                b.imagem = baloes[baloes.size() - 1].imagem + 1;
                b.imagem = (b.imagem == 4) ? 1 : b.imagem;
                b.conteudo = (wlrand() % 3) + 1;
            } else {
                b.imagem = 1;
                b.conteudo = (wlrand() % 3) + 1;
            }
            baloes.push_back(b);
        }
            
        // Desenhar
        acquire_screen();
        desenhar_fundo_animado();
        masked_blit((BITMAP *)imgs[Fim].dat, buffer, 0, 0,
             (SCREEN_WIDTH - 223) / 2, 200, 223, 98);
        desenhar_baloes(baloes);
                
        imprime_letra((BITMAP *)imgs[LC].dat, buffer, 0 ,i);
        imprime_letra((BITMAP *)imgs[LO].dat, buffer, 1 ,i);
        imprime_letra((BITMAP *)imgs[LN].dat, buffer, 2 ,i);
        imprime_letra((BITMAP *)imgs[LS].dat, buffer, 3 ,i);
        imprime_letra((BITMAP *)imgs[LT].dat, buffer, 4 ,i);
        imprime_letra((BITMAP *)imgs[LR].dat, buffer, 5 ,i);
        imprime_letra((BITMAP *)imgs[LU].dat, buffer, 6 ,i);
        imprime_letra((BITMAP *)imgs[LI].dat, buffer, 7 ,i);
        imprime_letra((BITMAP *)imgs[LN].dat, buffer, 8 ,i);
        imprime_letra((BITMAP *)imgs[LD].dat, buffer, 9 ,i);
        imprime_letra((BITMAP *)imgs[LO].dat, buffer, 10 ,i);

        imprime_letra((BITMAP *)imgs[LP].dat, buffer, 12 ,i);
        imprime_letra((BITMAP *)imgs[LA].dat, buffer, 13 ,i);
        imprime_letra((BITMAP *)imgs[LL].dat, buffer, 14 ,i);
        imprime_letra((BITMAP *)imgs[LA].dat, buffer, 15 ,i);
        imprime_letra((BITMAP *)imgs[LV].dat, buffer, 16 ,i);
        imprime_letra((BITMAP *)imgs[LR].dat, buffer, 17 ,i);
        imprime_letra((BITMAP *)imgs[LA].dat, buffer, 18 ,i);
        imprime_letra((BITMAP *)imgs[LS].dat, buffer, 19 ,i);
        desenhar_personagem();

        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        release_screen();

        // Tratar Teclado
        while (game_speed > 0) {
            i += 2;
            
            // Ajustar passo do personagem
            if (i > SCREEN_WIDTH - (2 * TILE_SIZE)) {
                personagem.acao    = 2;
                personagem.sentido = 'D';
            } else {
                personagem.X = i;
                personagem.passo -= 1;
            }
            if (personagem.passo <= 0) {
                personagem.passo = TILE_SIZE;
                tocar(Passo);
            }
            
            if (keypressed()) {
                switch (readkey() >> 8) {
                case KEY_ESC:
                case KEY_ENTER:
                    tocar(Toc);
                    s = 1;
                    break;

                case KEY_F1:
                    alterna_tela();
                    break;
                }
       
            }
            game_speed--;
        } // while
    } // while
}


//
//     Exibe o menu de opções
//
void Jogo::opcoes() {
    operacao   = -1;
    game_speed = 0;
    clear_keybuf();
    menuopcoes.iniciar();

    // Desenhar menu de opções
    while (!menuopcoes.get_sair()) {
    
        // Desenhar
        acquire_screen();
        desenhar_fundo_animado();
        desenhar_titulo();
        desenhar_baloes(baloes);
        menuopcoes.desenhar(imgs, buffer, comSom, volSample, volMidi, velPasso);
        desenhar_creditos();
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        release_screen();
        
        // Tratar Teclado
        while (game_speed > 0) {
            if (keypressed()) {
                switch (readkey() >> 8) {
                case KEY_UP:
                    menuopcoes.subir();
                    tocar(MoveMenu);
                    break;
                
                case KEY_DOWN:
                    menuopcoes.descer();
                    tocar(MoveMenu);
                    break;
                
                case KEY_RIGHT:
                    menuopcoes.aumentar(volSample, volMidi, velPasso);
                    tocar(MoveMenu);
                    break;
                
                case KEY_LEFT:
                    menuopcoes.diminuir(volSample, volMidi, velPasso);
                    tocar(MoveMenu);
                    break;

                case KEY_ESC:
                    tocar(MoveMenu);
                    menuopcoes.set_sair();
                    break;

                case KEY_ENTER:
                    if (menuopcoes.get_opcao() == OPSAIR) {
                        tocar(MoveMenu);
                        menuopcoes.set_sair();
                    } else {
                        tocar(Erro);
                    }
                    break;

                case KEY_F1:
                    alterna_tela();
                    break;
                }
            }
            game_speed--;
        }
    }
}


//
//     Desenha nuvens no sentido noroeste/sudeste
//
void Jogo::desenhar_fundo_animado() {
    BITMAP *imagem = (BITMAP *)imgs[Nuvens1].dat;
    if (operacao == JOGAR) {
        switch (fase.nivel) {
        case 1:
            imagem = (BITMAP *)imgs[Nuvens1].dat;
            break;
        case 2:
            imagem = (BITMAP *)imgs[Nuvens2].dat;
            break;
        case 3:
            imagem = (BITMAP *)imgs[Nuvens3].dat;
            break;
        }
    }
    int srcX, srcY;

    const int half_width = (imagem->w / 2),
              half_height = (imagem->h / 2);

    srcX = bg_speed % half_width;
    srcY = bg_speed % half_height;

    for (int destX = 0; destX < SCREEN_WIDTH; destX += half_width) {
        for (int destY = 0; destY < SCREEN_HEIGHT; destY += half_height) {
            blit(imagem, buffer, srcX, srcY, destX, destY, 
                 half_width, half_height);
        }
    }
}


//
//     Desenha o título
//
void Jogo::desenhar_titulo() {
    blit((BITMAP *)imgs[Titulo].dat, buffer, 0, 0,
         (SCREEN_WIDTH - 360) / 2, 0, 360, 144);    
}


//
//     Desenha os créditos no canto
//
void Jogo::desenhar_creditos() {
    masked_blit((BITMAP *)imgs[Creditos].dat, buffer, 0, 0,
                SCREEN_WIDTH - 179, SCREEN_HEIGHT - 50, 215, 50);    
}


//
//     Desenha a lista de fases de cada nível
//
void Jogo::desenhar_menufases(int &faseinicio) {
    static int cor = 0;
    
    // Desenha a tabela
    masked_blit((BITMAP *)imgs[Niveis].dat, buffer, 0, 0,
         (SCREEN_WIDTH - 503) / 2, 150, 503, 264);                 

    // Para cada nível
    for (int i = 0; i < 3; i++) {

        // Desenhar setas quando necessário
                
        // Seta para baixo
        if ((faseinicio + 17 < (int)fases[fase.nivel - 1].size()) &&
            (fase.nivel - 1 == i)) {
            masked_blit((BITMAP *)imgs[Desce].dat, buffer, 0, 0,
                ((SCREEN_WIDTH - 503) / 2) + 130 + i * 165,
                150 + 35 + 190, 24, 24);
        }
                
        // Seta para cima
        if ((faseinicio > 0) && (fase.nivel - 1 == i)) {
            masked_blit((BITMAP *)imgs[Sobe].dat, buffer, 0, 0,
                ((SCREEN_WIDTH - 503) / 2) + 130 + i * 165,
                150 + 35, 24, 24);
        }

        // Para cada Fase do Nível
        for (int j = 0; j < (int)fases[i].size() && j < 17; j++) {
                    
            // Se é uma fase habilitada
            if (j + 1 + faseinicio <= habilitadas.nivel[i]) {
                        
                cor = ((j + 1 + faseinicio == fase.num) &&
                      (i + 1 == fase.nivel)) ?
                      makecol(0, 255, 0)     : // Fase selecionada: Verde claro
                      makecol(255, 255, 255);  // Fase não selecionada: Branco
                        
            // Se é uma fase desabilitada
            } else {
                        
                // Fase selecionada
                cor = ((j + 1 + faseinicio == fase.num) &&
                      (i + 1 == fase.nivel)) ?
                      makecol(0, 100, 0)     : // Fase selecionada: Verde escuro
                      makecol(100, 100, 100);  // Fase não selecionada: Cinza
            }
            int numfase = j + 1;
            numfase += (fase.nivel - 1 == i) ? faseinicio : 0;

            // Imprime a fase do nível
            textprintf_ex(buffer, font, 
                 ((SCREEN_WIDTH - 503) / 2) + 60 + i * 165,
                 150 + 35 + j * 13, cor, 0, "Fase %02d", numfase);
        }
    }
}


//
//     Desenha a fase
//
void Jogo::desenhar_fase() {
    int tile, tile2, w2, h2;

    // Desenhando a fase
    for (unsigned int w = 0; w < fase.piso.size(); w++) {
        for (unsigned int h = 0; h < fase.piso[0].size(); h++) {
            tile = fase.piso[w][h];
            if (tile) {
                w2 = -1;
                h2 = -1;
                
                // Se é um canto superior esquerdo arredondado
                if (tile == F2) {
                    w2 = w - 1;
                    h2 = h - 1;
                
                // Se é um canto superior direito arredondado
                } else if (tile == F3) {
                    w2 = w + 1;
                    h2 = h - 1;

                // Se é um canto inferior esquerdo arredondado
                } else if (tile == F4) {
                    w2 = w - 1;
                    h2 = h + 1;
                
                // Se é um canto inferior direito arredondado
                } else if (tile == F5) {
                    w2 = w + 1;
                    h2 = h + 1;
                }
                if ((w2 >= 0) && (w2 < (int)fase.piso.size()) &&
                    (h2 >= 0) && (h2 < (int)fase.piso[0].size())) {
                    tile2 = fase.piso[w2][h2];
                    if (tile2) {
                        masked_blit((BITMAP *)imgs[tile2].dat, buffer, 0, 0, 
                            fase.X + (w * TILE_SIZE), fase.Y + (h * TILE_SIZE),
                            TILE_SIZE, TILE_SIZE);
                    }
                }
                masked_blit((BITMAP *)imgs[tile].dat, buffer, 0, 0, 
                     fase.X + (w * TILE_SIZE), fase.Y + (h * TILE_SIZE),
                     TILE_SIZE, TILE_SIZE);
            }

        }
    }

    // Desenhando os blocos verdes
    int tam_palavra = (fase.destinos.size() - 1);
    for (int num_blocos = tam_palavra; num_blocos >= 0; num_blocos--) {
        masked_blit((BITMAP *)imgs[fase.destinos[num_blocos].imagem].dat,
             buffer,
             0, 0, fase.X + (fase.destinos[num_blocos].blocoX * TILE_SIZE),
             fase.Y + (fase.destinos[num_blocos].blocoY * TILE_SIZE),
             TILE_SIZE, TILE_SIZE);
    }

    // Colocando os objetos
    for (int num_obj = (fase.objetos.size() - 1); num_obj >= 0; num_obj--) {
       tile = fase.objetos[num_obj].imagem;
       if (tile) {
           masked_blit((BITMAP *)imgs[tile].dat, buffer, 0, 0, 
                fase.X + (fase.objetos[num_obj].blocoX * TILE_SIZE),
                fase.Y + (fase.objetos[num_obj].blocoY * TILE_SIZE),
                TILE_SIZE, TILE_SIZE);
        }
    }
}


//
//     Desenho Topo
//
void Jogo::desenhar_topo(int tempo, bool atualizar) {

    masked_blit((BITMAP *)imgs[Topo].dat, buffer, 0, 0, 10, 10, 620, 82);
    if (personagem.mala.conteudo) {
        masked_blit((BITMAP *)imgs[personagem.mala.imagem].dat, buffer, 
             0, 0, 33, 51, TILE_SIZE, TILE_SIZE);
    }

    // Imprimir Fase, Nível e Tempo
    textprintf_ex(buffer, font, 583, 34, makecol(255, 255, 0), 0,
                  "%02d", fase.num);
    textprintf_ex(buffer, font, 583, 52, makecol(255, 255, 0), 0,
                  "%d/3", fase.nivel);
    textout_ex(buffer, font, converter_tempo(tempo, atualizar), 583, 71, 
         makecol(255, 255, 0), 0);

    // Imprimir Dica
    for (int j = 0; j < 3; j++) {
        textout_centre_ex(buffer, font, fase.dica[j], 305,
             45 + (j * 13), -1, 0);
    }
}


//
//     Desenha o personagem
//
void Jogo::desenhar_personagem() {
    char aux[5];

    aux[0] = 'N'; // Ness
    aux[2] = personagem.sentido; // Sentido
    aux[4] = '\0';

    // Andando
    if (personagem.acao == 1) {
        aux[1] = '1'; // Andando

        if (((TILE_SIZE - 2)/ personagem.NUM_QUADROS) <= personagem.passo) {
            aux[3] = '1'; // Tipo
        } else {
            aux[3] = '2'; // Tipo
        }
        
    // Vitória
    } else if (personagem.acao == 2) {
        aux[1] = '2'; // Vitória
        aux[2] = 'D'; // Sentido
        aux[3] = '1'; // Tipo
        
    // Parado
    } else if (personagem.acao == 0) {
        aux[1] = '0'; // Parado
        aux[3] = '1'; // Tipo
    
    // Dormindo
    } else if (personagem.acao == 3) {
        aux[1] = '3'; // Dormindo
        aux[2] = 'D'; // Sentido
        aux[3] = '1'; // Tipo
    }

    masked_blit((BITMAP *)imgs[get_imagem(aux)].dat, buffer, 0, 0, personagem.X,
         personagem.Y, TILE_SIZE, TILE_SIZE);
         
    if (personagem.acao == 3) {
        masked_blit((BITMAP *)imgs[ZZZ].dat, buffer, 0, 0, personagem.X + 20,
         personagem.Y - 20, 98, 23);
    }
}


//
//     Desenha balões no final do jogo
//
void Jogo::desenhar_baloes(vector<Objeto> &baloes) {
    static int tempo = wlclock();
    bool subir = false;

    if (passou(tempo, 10)) {
        tempo = wlclock();
        subir = true;
    }

    for (int i = 0; i < (int)baloes.size(); i++) {

        if (subir) {
            baloes[i].blocoY -= baloes[i].conteudo;
            if ((baloes[i].blocoY + 219) <= 0) {
                baloes[i].blocoX = wlrand() % SCREEN_WIDTH;
                baloes[i].blocoY = wlrand() % SCREEN_HEIGHT;
                baloes[i].blocoY += SCREEN_HEIGHT;
            }
        }

        switch (baloes[i].imagem) {
            case 1:
                masked_blit((BITMAP *)imgs[Balao1].dat, buffer, 0, 0,
                     baloes[i].blocoX, baloes[i].blocoY , 100, 219);
                break;
            case 2:
                masked_blit((BITMAP *)imgs[Balao2].dat, buffer, 0, 0,
                     baloes[i].blocoX, baloes[i].blocoY , 100, 219);
                break;
            default:
                masked_blit((BITMAP *)imgs[Balao3].dat, buffer, 0, 0,
                     baloes[i].blocoX, baloes[i].blocoY , 100, 219);
                break;
        }
    }
}


//
//     Carrega fase ou retorna FALSE
//
bool Jogo::carregar_fase(Fase &fase, const vector< vector<Fase> > fases) {
    
    // Se não existe, passar para o próximo nível
    if (fases[fase.nivel - 1].size() < (unsigned int)fase.num) {
        
        // Se é o último nível, então acabaram as fases
        if (fase.nivel == 3) {
            return false;
            
        // Se não é o último nível
        } else {
            fase.num = 1;
            fase.nivel++;
            return carregar_fase(fase, fases);
        }
        
    // Se existe, carregar
    } else {
        fase  = fases[fase.nivel - 1][fase.num - 1];
    }
    return true;
}


//
//     Toca um som Wave
//
inline void Jogo::tocar(int som) {
    play_sample((SAMPLE *)sons[som].dat, 255, 127, 1000, 0);
}


//
//     Verifica se os campos verdes foram preenchidos incorretamente
//     FALSE: não foram todos preenchidos ou foram preenchidos corretamente
//     TRUE: Todos foram preenchidos, mas incorretamente
//
bool Jogo::verificar_erros() {
    bool retorno = false,
         preenchido = false;
         
    // Para cada campo verde
    for (int i = (fase.destinos.size() - 1); i >= 0; i--) {
        
        // Para cada letra da fase
        preenchido = false;
        for (int j = (fase.objetos.size() - 1); j >= 0; j--) {
            
            // Se a letra está no campo
            if ((fase.objetos[j].blocoX == fase.destinos[i].blocoX) &&
                (fase.objetos[j].blocoY == fase.destinos[i].blocoY)) {
                preenchido = true;
                
                // Se a letra está errada
                if (fase.objetos[j].conteudo != fase.destinos[i].conteudo) {
                    retorno = true;
                }
                break;
            }
        }
        if (!preenchido) {
            return false;
        }
    }
    return retorno;
}


//
//     Verifica se existe um objeto na posição indicada
//
int Jogo::tem_objeto(unsigned char blocoX, unsigned char blocoY) {
    for (int j = fase.objetos.size() - 1; j >= 0; j--) {
        if ((fase.objetos[j].blocoX == blocoX) && 
            (fase.objetos[j].blocoY == blocoY)) {
            return j;
        }
    }
    return -1;
}


//
//     Pega um objeto da fase
//
inline void Jogo::pegar_objeto(unsigned char indice_objeto) {
    personagem.mala = fase.objetos[indice_objeto];
    fase.objetos.erase(fase.objetos.begin() + indice_objeto);
}


//
//     Larga o objeto da mala na coordenada indicada
//     Retorna se acabou a fase ou não
//
bool Jogo::deixar_objeto(unsigned char blocoX, unsigned char blocoY) {
    bool podeContinuar;

    personagem.mala.blocoX = blocoX;
    personagem.mala.blocoY = blocoY;

    fase.objetos.push_back(personagem.mala);
    personagem.mala.conteudo = 0;

    for (int i = fase.destinos.size() - 1; i >= 0; i--) {
        podeContinuar = false;
        for (int j = fase.objetos.size() - 1; j >= 0; j--) {
            if ((fase.destinos[i].blocoX == fase.objetos[j].blocoX) &&
                (fase.destinos[i].blocoY == fase.objetos[j].blocoY)) {
                    
                // Se o conteúdo do bloco está errado
                if (fase.destinos[i].conteudo != fase.objetos[j].conteudo) {
                    return false;
                } else {
                    podeContinuar = true;
                    break;
                }
            } else {
                continue;
            }
        }
        
        // Se não há objetos neste bloco
        if (!podeContinuar) {
            return false;
        }
    }
    return true;
}


//
//     Retorna se uma posição da fase é passável ou não
//
bool Jogo:: passavel(int blocoX, int blocoY) {
    if ((blocoX >= 0) && (blocoX < (int)fase.piso.size()) &&
        (blocoY >= 0) && (blocoY < (int)fase.piso[0].size())) {
        if (!fase.passavel[blocoX][blocoY]) {
            return false;
        }
    } else {
        return false;
    }

    for (int i = fase.objetos.size() - 1; i >= 0; i--) {
        if ((fase.objetos[i].blocoX == blocoX) &&
            (fase.objetos[i].blocoY == blocoY)) {
            return false;
        }
    }

    return true;
}


//
//     Encerra as funcionalidades da Allegro
//
inline void Jogo::sair() {
    clear_keybuf();
    stop_midi();
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
    salvar_habilitadas();
    unload_datafile(imgs);
    unload_datafile(sons);
    destroy_bitmap(buffer);
    allegro_exit();
}
#endif

