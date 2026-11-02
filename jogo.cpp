/**
 * Construindo Palavras
 * Descricao: Classe de controle do jogo.
 */
#include <allegro.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include "jogo.h"
#include "tipos.h"
#include "constantes.h"
#include "funcoes.h"

using namespace std;
namespace fs = std::filesystem;

volatile unsigned int game_speed = 0,   // Controle de velocidade do jogo
    bg_speed = 0,                       // Controle de velocidade do fundo
    clock_speed = 0;                    // Controle de relogio

/**
 * Construtor Padrao
 */
Jogo::Jogo() {
    operacao         = -1;
    comSom           = false;
    comUpscale       = false;
    realWidth        = SCREEN_WIDTH;
    realHeight       = SCREEN_HEIGHT;
    musicaAtual      = NULL;
}

/**
 * Encerra o jogo
 */
Jogo::~Jogo() {
    encerrar();
}

/**
 * Carrega recursos para o jogo e retorna um exit status
 */
int Jogo::carregar_recursos(fs::path caminhoBase) {
    int depth        = 0;
    bool comVideo    = false;
    fs::path caminho_absoluto;

    // Inicializando a Allegro, Teclado e Timer
    std::cout << "[LOG] Iniciando recursos Allegro" << std::endl;
    set_uformat(U_ASCII);
    allegro_init();
    install_keyboard();
    install_timer();

    set_window_title("Construindo Palavras");

    // Travando as variaveis e funcoes usadas em timers
    LOCK_VARIABLE(game_speed);
    LOCK_FUNCTION(incrementar_game_speed);
    LOCK_VARIABLE(bg_speed);
    LOCK_FUNCTION(decrementar_bg_speed);
    LOCK_VARIABLE(clock_speed);
    LOCK_FUNCTION(incrementar_clock_speed);

    // Instalando as interrupcoes necessarias
    install_int_ex(incrementar_game_speed, BPS_TO_TIMER(FPS));
    install_int_ex(decrementar_bg_speed, BPS_TO_TIMER(15));
    install_int_ex(incrementar_clock_speed, BPS_TO_TIMER(1));

    // Definindo o modo grafico
    if ((depth = desktop_color_depth()) >= 16) {
        set_color_depth(depth);
    } else {
        set_color_depth(16);
    }

    std::cout << "[LOG] Iniciando modo grafico" << std::endl;

    // Tenta resolucao tela cheia com upscale
    if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,
        SCREEN_WIDTH_UPSCALE, SCREEN_HEIGHT_UPSCALE, 0, 0) == 0
    ) {
        comVideo   = true;
        tipoVideo  = GFX_AUTODETECT_FULLSCREEN;
        comUpscale = true;
        realWidth  = SCREEN_WIDTH_UPSCALE;
        realHeight = SCREEN_HEIGHT_UPSCALE;
    }

    // Tenta resolucao tela cheia sem upscale
    if (!comVideo) {
        if (set_gfx_mode(GFX_AUTODETECT_FULLSCREEN,
            SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0
        ) {
            comVideo  = true;
            tipoVideo = GFX_AUTODETECT_FULLSCREEN;
        }
    }

    // Tenta tela cheia no modo X com upscale e sem
    #ifdef GFX_XWINDOWS_FULLSCREEN
    if (!comVideo) {
        if (set_gfx_mode(GFX_XWINDOWS_FULLSCREEN,
            SCREEN_WIDTH_UPSCALE, SCREEN_HEIGHT_UPSCALE, 0, 0) == 0
        ) {
            comVideo   = true;
            tipoVideo  = GFX_XWINDOWS_FULLSCREEN;
            comUpscale = true;
            realWidth  = SCREEN_WIDTH_UPSCALE;
            realHeight = SCREEN_HEIGHT_UPSCALE;
        }
        if (set_gfx_mode(GFX_XWINDOWS_FULLSCREEN,
            SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0
        ) {
            comVideo  = true;
            tipoVideo = GFX_XWINDOWS_FULLSCREEN;
        }
    }
    #endif

    // Tenta modo VGA com upscale e sem
    #ifdef GFX_VGA
    if (!comVideo) {
        if (set_gfx_mode(GFX_VGA, SCREEN_WIDTH_UPSCALE, SCREEN_HEIGHT_UPSCALE, 0, 0) == 0) {
            comVideo   = true;
            tipoVideo  = GFX_VGA;
            comUpscale = true;
            realWidth  = SCREEN_WIDTH_UPSCALE;
            realHeight = SCREEN_HEIGHT_UPSCALE;
        }
        if (set_gfx_mode(GFX_VGA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0) {
            comVideo  = true;
            tipoVideo = GFX_VGA;
        }
    }
    #endif

    // Tenta modo janela (sem upscale)
    if (!comVideo) {
        if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,
            SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0
        ) {
            comVideo  = true;
            tipoVideo = GFX_AUTODETECT_WINDOWED;
        }
    }

    // Tenta resolucao segura (sem upscale)
    if (!comVideo) {
        if (set_gfx_mode(GFX_SAFE, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0) == 0) {
            comVideo  = true;
            tipoVideo = GFX_SAFE;
        }
    }

    if (!comVideo) {
        allegro_message("Erro ao definir modo grafico");
        return static_cast<int>(ERRO_VIDEO);
    }

    std::cout << "[LOG] Iniciando som" << std::endl;

    // Definir som
    int tipoSample = DIGI_AUTODETECT;

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
    comSom = (install_sound(tipoSample, MIDI_NONE, 0) == 0);

    // Carregando o arquivo de imagens
    caminho_absoluto = caminhoBase / "dados" / "Imagens.dat";
    std::cout << "[LOG] Carregando arquivo de imagens em: " << caminho_absoluto << std::endl;

    this->imgs = load_datafile(caminho_absoluto.string().c_str());
    if (imgs == NULL) {
        allegro_message("Erro ao carregar o arquivo \"Imagens.dat\".");
        return static_cast<int>(ERRO_IMAGENS);
    }

    // Carregando o arquivo de sons
    caminho_absoluto = caminhoBase / "dados" / "Sons.dat";
    std::cout << "[LOG] Carregando arquivo de sons em: " << caminho_absoluto << std::endl;

    this->sons = load_datafile(caminho_absoluto.string().c_str());
    if ((sons == NULL) && comSom) {
        volSample = 0;
        volMusica = 0;
        comSom    = false;
    } else {
        volSample = 204;
        volMusica = 204;
    }
    set_volume(volSample, volMusica);

    // Carregando o arquivo de fases
    std::cout << "[LOG] Carregando arquivos de fases" << std::endl;
    if (!carregar_fases(caminhoBase)) {
        allegro_message("Erro ao carregar as fases.");
        return static_cast<int>(ERRO_FASES);
    }

    std::cout << "[LOG] Carregando fases habilitadas" << std::endl;
    carregar_habilitadas();

    // Criando um buffer para guardar as imagens a serem exibidas na tela
    std::cout << "[LOG] Iniciando buffer de renderizacao" << std::endl;
    this->buffer = create_bitmap(SCREEN_WIDTH, SCREEN_HEIGHT);

    // Inicializa o jogo
    fase.num   = 1;
    fase.nivel = 1;
    velPasso   = 1;
    zerou      = false;

    std::cout << "[LOG] Tudo pronto para iniciar o jogo" << std::endl;
    return static_cast<int>(OK);
}

/**
 * Aguarda 10ms ate que o game_speed volte a zerar
 */
void Jogo::aguardar() {
    while (game_speed == 0) {
        rest(10);
    }
}

/**
 * Le a fase do arquivo texto
 */
bool Jogo::ler_fase(const fs::path caminho, Fase &fase) {
    char aux[60];
    int largura, altura;

    std::cout << "[LOG] Verificando arquivo de fase em: " << caminho << std::endl;

    if (!fs::exists(caminho) || !fs::is_regular_file(caminho)) {
        std::cerr << "[ERRO] Arquivo de fase nao encontrado ou invalido: " << caminho << std::endl;
        return false;
    }

    ifstream arquivo_texto(caminho.string().c_str());

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

    // L� as coordenadas iniciais do personagem
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

    // Lendo o conte�do dos blocos verdes
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

        int len = strlen(aux);
        while (len > 0 && (aux[len - 1] == '\r' || aux[len - 1] == '\n')) {
            aux[len - 1] = '\0';
            len--;
        }
        strcpy(fase.dica[j], aux);
    }

    arquivo_texto.close();

    return true;
}

/**
 * Le os arquivos para montar os vetores de fases
 */
bool Jogo::carregar_fases(fs::path caminhoBase) {
    bool lendoFase = true;
    Fase fase;
    char nivel = '0', nomeArquivo[14] = "Fases/x00.txt";
    fs::path caminho_absoluto;

    // Matriz 3 x N (3 niveis / N fases por nivel)
    fases.resize(3);

    // Carregando fases faceis, medias e dificeis
    for (int i = 1; i <= 3; i++) {
        fase.num   = 1;
        fase.nivel = i;
        switch (i) {
            case 1: nivel = 'f'; break;
            case 2: nivel = 'm'; break;
            case 3: nivel = 'd'; break;
        }
        do {
            sprintf(nomeArquivo, "%c%02d.txt", nivel, fase.num);
            caminho_absoluto = caminhoBase / "Fases" / nomeArquivo;
            std::cout << "[LOG] Tentando carregar arquivo de fase em: " << caminho_absoluto << std::endl;

            if (lendoFase = ler_fase(caminho_absoluto, fase)) {
                fases[fase.nivel - 1].push_back(fase);
                fase.num += 1;
            }
        } while(lendoFase);
    }

    return true;
}

/**
 * Le arquivo tipado de fases habilitadas
 */
bool Jogo::carregar_habilitadas() {
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) {
        // Fallback caso HOME não esteja definida
        homeDir = ".";
    }

    fs::path caminhoArquivo = fs::path(homeDir) / ".local" / "share" / "palavras" / "Fases.dat";

    std::ifstream arquivoLeitura(caminhoArquivo, std::ios::binary);
    if (arquivoLeitura.is_open() && arquivoLeitura.good()) {
        arquivoLeitura.read(reinterpret_cast<char *>(&habilitadas), sizeof(habilitadas));
        arquivoLeitura.close();
        return true;
    }

    habilitadas.nivel[0] = 1;
    habilitadas.nivel[1] = 1;
    habilitadas.nivel[2] = 1;
    return false;
}

/**
 * Salva fases habilitadas em arquivo tipado
 */
bool Jogo::salvar_habilitadas() {
    const char* homeDir = std::getenv("HOME");
    if (!homeDir) {
        homeDir = ".";
    }

    fs::path caminhoArquivo = fs::path(homeDir) / ".local" / "share" / "palavras" / "Fases.dat";
    fs::path diretorioDestino = caminhoArquivo.parent_path();

    if (!fs::exists(diretorioDestino)) {
        fs::create_directories(diretorioDestino);
    }

    std::ofstream arquivoEscrita(caminhoArquivo, std::ios::binary);
    if (arquivoEscrita.is_open() && arquivoEscrita.good()) {
        arquivoEscrita.write(reinterpret_cast<const char *>(&habilitadas), sizeof(habilitadas));
        arquivoEscrita.close();
        return true;
    }

    return false;
}

/**
 * Ajusta a tela para tela cheia ou janela
 */
void Jogo::alternar_tela() {
    if (is_windowed_mode()) {
        set_gfx_mode(tipoVideo, realWidth, realHeight, 0, 0);
    } else {
        set_gfx_mode(GFX_AUTODETECT_WINDOWED, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);
    }
}

/**
 * Retorna a operacao a ser realizada (situacao do jogo)
 */
int Jogo::get_operacao() {
    return operacao;
}

/**
 * Tocar uma musica a partir do numero
 */
void Jogo::tocar_musica(int musica) {
    if (musicaTocando == musica) {
        return;
    }
    musicaTocando = musica;

    // Interrompe a musica anterior se estiver tocando
    if (musicaAtual != NULL) {
        stop_sample(musicaAtual);
        destroy_sample(musicaAtual);
        musicaAtual = NULL;
    }

    // Monta o caminho dinamico do arquivo
    char caminho[64];
    sprintf(caminho, "dados/musicas/musica%d.wav", musica);

    // Carrega o audio
    musicaAtual = load_sample(caminho);
    if (musicaAtual != NULL) {
        // play_sample(sample, volume, pan, pitch, loop)
        // 255 = volume máximo, loop = 1
        play_sample(musicaAtual, volMusica, 127, 1000, 1);
    }
}

/**
 * Animacao da apresentacao inicial
 */
void Jogo::apresentacao() {
    clear_keybuf();
    int i = -144;
    game_speed = 0;
    while (i < 0) {
        desenhar_fundo_animado();
        blit((BITMAP *)imgs[Titulo].dat, buffer, 0, 0,
            (SCREEN_WIDTH - 360) / 2, i, 360, 144);
        desenhar_baloes(baloes);
        menu.desenhar(3 * i, -2 * i, imgs, buffer);
        masked_blit((BITMAP *)imgs[Creditos].dat, buffer, 0, 0,
            (SCREEN_WIDTH - 179) - i,
            (SCREEN_HEIGHT - 50) - (i / 2), 215, 50);

        imprimir_tela();

        aguardar();

        while (game_speed > 0) {
            i += 2;

            // Tratar Teclado
            if (keypressed()) {
                switch (readkey() >> 8) {
                case KEY_UP:
                case KEY_W:
                    menu.subir();
                    tocar(MoveMenu);
                    break;

                case KEY_DOWN:
                case KEY_S:
                    menu.descer();
                    tocar(MoveMenu);
                    break;

                case KEY_ENTER:
                    menu.selecionar();
                    tocar(MoveMenu);
                    operacao = menu.get_opcao();
                    return;

                case KEY_ESC:
                    return;

                default:
                    tocar(Erro);
                    break;
                }
            }

            game_speed--;
        } // while

    } // while
}


/**
 * Imprime o menu enquanto nao seleciona uma opcao
 */
void Jogo::exibe_menu() {
    bool exibindoDica = false;
    const int velDica = 6;
    int x = -180,
        y = SCREEN_HEIGHT - 50;
    menu.iniciar();
    inicioOcioso = clock_speed;
    game_speed   = 0;

    // Enquanto não selecionou uma opção
    while (!menu.definido()) {
        if (passou(inicioOcioso, OCIOSO)) {
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

        // Desenhar Menu (sem acquire_screen/release_screen)
        desenhar_fundo_animado();
        desenhar_titulo();
        desenhar_baloes(baloes);
        menu.desenhar(0, 0, imgs, buffer);
        desenhar_creditos();
        masked_blit((BITMAP *)imgs[DicaMenu].dat, buffer, 0, 0, x, y, 360, 168);
        imprimir_tela();

        aguardar();

        while (game_speed > 0) {

            // Tratar Teclado
            if (keypressed()) {
                inicioOcioso = clock_speed;

                switch (readkey() >> 8) {
                case KEY_UP:
                case KEY_W:
                    menu.subir();
                    tocar(MoveMenu);
                    break;

                case KEY_DOWN:
                case KEY_S:
                    menu.descer();
                    tocar(MoveMenu);
                    break;

                case KEY_ENTER:
                    menu.selecionar();
                    tocar(MoveMenu);
                    break;

                case KEY_F1:
                    alternar_tela();
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
        } // while game_speed
    } // while menu.definido

    operacao = menu.get_opcao();
}


/**
 * Mostra a animacao de inicializacao da fase
 */
void Jogo::apresentacao_fase(Fase &fase) {
    const int vel  = 3 * velPasso, // Velocidade de descida
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
        desenhar_fundo_animado();
        desenhar_fase();

        // Desenhar Topo
        int topoX = (SCREEN_WIDTH - 620) / 2;
        masked_blit((BITMAP *)imgs[Topo].dat, buffer, 0, 0, topoX, 10, 620, 82);

        // Imprimir Fase, Nivel e Tempo
        textprintf_ex(buffer, font, topoX + 573, 34, makecol(255, 255, 0), 0, "%02d", fase.num);
        textprintf_ex(buffer, font, topoX + 573, 52, makecol(255, 255, 0), 0, "%d/3", fase.nivel);
        textout_ex(buffer, font, "00:00", topoX + 573, 71, makecol(255, 255, 0), 0);

        // Imprimir Dica
        for (int j = 0; j < (personagem.Y / idica); j++) {
            textout_centre_ex(buffer, font, fase.dica[j], topoX + 295, 45 + (j * 13), -1, 0);
        }

        desenhar_personagem();
        imprimir_tela();

        aguardar();

        while (game_speed > 0) {
            personagem.Y += vel;
            i++;

            switch ((i % (4 * atrasoGiro)) / atrasoGiro) {
            case 0: personagem.sentido = 'D'; break;
            case 1: personagem.sentido = 'R'; break;
            case 2: personagem.sentido = 'U'; break;
            case 3: personagem.sentido = 'L'; break;
            }

            // saltar para a posicao final (ignorar animacao)
            if (keypressed()) {
                personagem.Y = fase.personagemY;
            }
            game_speed--;
        }
    }
}


/**
 * Inicia uma fase
 */
void Jogo::iniciar() {
    bool acabou            = false;   // Terminou a fase
    bool sair              = false;   // Sair da fase sem ter terminado
    bool comInstrucoes     = false;   // Exibindo instrucoes
    bool errou             = false;   // Errou a palavra
    bool tocouErro         = false;   // Tocou o som de erro quando errou
    unsigned int tempoJogo = 0;       // Tempo gasto na fase

    // Se nao eh uma fase habilitada, voltar pra primeira habilitada
    if (habilitadas.nivel[fase.nivel - 1] < fase.num) {
        fase.num = habilitadas.nivel[fase.nivel - 1];
    }

    // Se zerou, mostra os creditos e depois o final
    if (!carregar_fase(fase, fases)) {
        zerou = true;
        operacao = CREDITOS;
        return;
    }

    // Tocar a musica do nivel correspondente
    switch (fase.nivel) {
    case 1: tocar_musica(MF); break;
    case 2: tocar_musica(MM); break;
    case 3: tocar_musica(MD); break;
    }

    // Mostrar animacao de apresentacao da fase
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
    tempoJogo      = clock_speed;
    inicioOcioso   = clock_speed;
    game_speed     = 0;
    secreto        = 0;
    comInstrucoes  = false;
    errou          = false;
    clear_keybuf();

    // Jogo propriamente dito
    while (!sair && !acabou) {

        // Desenhar jogo
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

        // "F2: Instrucoes" no canto direito
        if (!passou(inicioOcioso, OCIOSO) || clock_speed % 2) {
            masked_blit((BITMAP *)imgs[InstrucoesCanto].dat, buffer, 0, 0, 
                SCREEN_WIDTH - (106 + 10), SCREEN_HEIGHT - (14 + 10),
                106, 14);
        }

        // Instrucoes
        if (comInstrucoes) {
            masked_blit((BITMAP *)imgs[Instrucoes].dat, buffer, 0, 0,
                (SCREEN_WIDTH - 339) / 2, (SCREEN_HEIGHT - 266) / 2,
                339, 266);
        }

        imprimir_tela();

        aguardar();

        while (game_speed > 0) {

            // Se o personagem esta parado
            if (personagem.acao != ANDANDO) {
                personagem.passo = TILE_SIZE;

                // posBloco indica a coordenada a frente do personagem
                unsigned char posBlocoY = YtoBlocoY(personagem.Y, fase),
                    posBlocoX = XtoBlocoX(personagem.X, fase);

                switch (personagem.sentido) {
                case 'U': posBlocoY -= 1; break;
                case 'D': posBlocoY += 1; break;
                case 'L': posBlocoX -= 1; break;
                case 'R': posBlocoX += 1; break;
                }

                if (keypressed()) {
                    inicioOcioso = clock_speed;

                    switch (readkey() >> 8) {

                    // Subir
                    case KEY_UP:
                    case KEY_W:
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
                    case KEY_S:
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
                    case KEY_A:
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
                    case KEY_D:
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
                        personagem.acao = PARADO;

                        // Se a mala esta cheia
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

                        // Se a mala esta vazia
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
                        alternar_tela();
                        break;

                    // Mostra instrucoes
                    case KEY_F2:
                        comInstrucoes = !comInstrucoes;
                        break;

                    // Apara as instrucoes ou volta pro menu inicial
                    case KEY_ESC:
                        if (comInstrucoes) {
                            comInstrucoes = false;
                        } else {
                            operacao = -1;
                            sair     = true;
                        }
                        break;

                    // Personagem faz "vitoria" (ester egg)
                    case KEY_V:
                        personagem.acao = VITORIA;
                        break;

                    // Codigo secreto: "ZERO" durante o jogo
                    case KEY_Z:
                        secreto = 1;
                        break;
                    case KEY_E:
                        secreto = (secreto == 1) ? 2 : 0;
                        break;
                    case KEY_R:
                        secreto = (secreto == 2) ? 3 : 0;
                        break;
                    case KEY_O:
                        secreto = (secreto == 3) ? 4 : 0;
                        break;

                    default:
                        secreto = 0;
                        break;
                    }

                    clear_keybuf();

                // Se nao pressionou nenhuma tecla
                } else {

                    // Se ficou muito tempo parado
                    if (passou(inicioOcioso, OCIOSO)) {
                        personagem.acao = DORMINDO;
                    }
                }

            // Se o personagem nao esta parado
            } else {
                clear_keybuf();

                // Se nao esta em posicao discreta, continuar andando
                if ((personagem.sentido == 'U') &&
                    ((personagem.Y - fase.Y) % TILE_SIZE)
                ) {
                    personagem.Y     -= velPasso;
                    personagem.passo -= velPasso;
                } else if ((personagem.sentido == 'L') &&
                    ((personagem.X - fase.X) % TILE_SIZE)
                ) {
                    personagem.X     -= velPasso;
                    personagem.passo -= velPasso;
                } else if ((personagem.sentido == 'R') &&
                    ((personagem.X - fase.X) % TILE_SIZE)
                ) {
                    personagem.X     += velPasso;
                    personagem.passo -= velPasso;
                } else if ((personagem.Y - fase.Y) % TILE_SIZE) {
                    personagem.Y     += velPasso;
                    personagem.passo -= velPasso;

                // Se esta em posicao discreta, entao parar
                } else {
                    personagem.acao = PARADO;
                    inicioOcioso    = clock_speed;
                }
                if (personagem.passo <= (int)velPasso) {
                    personagem.passo = TILE_SIZE;
                    tocar(Passo);
                }
            }

            // Se digitou o codigo secreto
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

        // Habilitar a fase seguinte, caso nao esteja habilitada
        if (habilitadas.nivel[fase.nivel - 1] < (fase.num + 1)) {
            habilitadas.nivel[fase.nivel - 1] = fase.num + 1;
        }
        // Desenha Vitoria
        personagem.acao = VITORIA;
        int t = clock_speed;
        while (!passou(t, 2)) {
            desenhar_fundo_animado();
            desenhar_fase();
            desenhar_topo(tempoJogo, false);
            desenhar_personagem();
            imprimir_tela();
            if (keypressed()) {
                break;
            }
        }

        // Parabens
        bool exibindo = true;
        int cor       = 0;
        inicioOcioso  = clock_speed;
        while (exibindo) {

            // Desenha Parabens
            desenhar_fundo_animado();
            desenhar_topo(tempoJogo, false);
            masked_blit((BITMAP *)imgs[Parabens].dat, buffer, 0, 0,
                (SCREEN_WIDTH - 342) / 2, 200, 342, 151);
            if (passou(inicioOcioso, OCIOSO) && (clock_speed % 2)) {
                cor = makecol(255, 255, 0); // amarelo
            } else {
                cor = makecol(255, 255, 255); // branco
            }
            textout_ex(buffer, font, "Aperte Enter para continuar",
                ((SCREEN_WIDTH - 342) / 2) + 70 ,
                200 + 125, cor, 0);
            imprimir_tela();

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
                    alternar_tela();
                    break;
                }
            }
        } // while
    }
}


/**
 * Tela de selecao de fases
 */
void Jogo::mudar_fase() {
    bool selecionou = false;
    int faseinicio  = 0;
    const char MAX_FASES_VISIVEIS = 17;

    operacao        = -1;
    game_speed      = 0;
    clear_keybuf();

    faseinicio = (fase.num < MAX_FASES_VISIVEIS) ? 0 : fase.num - MAX_FASES_VISIVEIS;

    // Desenhar menu de fases
    while (!selecionou) {

        // Desenhar
        desenhar_fundo_animado();
        desenhar_titulo();
        desenhar_baloes(baloes);
        desenhar_menufases(faseinicio);
        desenhar_creditos();
        imprimir_tela();

        aguardar();

        while (game_speed > 0) {

            // Tratar Teclado
            if (keypressed()) {
                switch (readkey() >> 8) {

                case KEY_ENTER:

                    // Se eh uma fase habilitada: iniciar fase
                    if (fase.num <= habilitadas.nivel[fase.nivel - 1]) {
                        tocar(MoveMenu);
                        selecionou = true;
                        operacao   = JOGAR;

                    // Se nao eh uma fase habilitada: bloquear
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
                case KEY_W:
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
                case KEY_S:
                    if (fase.num < (int)fases[fase.nivel - 1].size()) {
                        tocar(MoveMenu);
                        if (fase.num == faseinicio + MAX_FASES_VISIVEIS) {
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
                        faseinicio = (fase.num <= MAX_FASES_VISIVEIS) ? 0 : fase.num % MAX_FASES_VISIVEIS;
                    } else {
                        tocar(Erro);
                    }
                    break;

                case KEY_RIGHT:
                case KEY_D:
                    if (fase.nivel < 3) {
                        tocar(MoveMenu);
                        fase.nivel += 1;
                        if ((int)fases[fase.nivel - 1].size() < fase.num) {
                            fase.num = fases[fase.nivel - 1].size();
                            faseinicio = (fase.num <= MAX_FASES_VISIVEIS) ? 0 : fase.num % MAX_FASES_VISIVEIS;
                        }
                    } else {
                        tocar(Erro);
                    }
                    break;

                case KEY_LEFT:
                case KEY_A:
                    if (fase.nivel > 1) {
                        tocar(MoveMenu);
                        fase.nivel -= 1;
                        if ((int)fases[fase.nivel - 1].size() < fase.num) {
                            fase.num = fases[fase.nivel - 1].size();
                        }
                        faseinicio = (fase.num <= MAX_FASES_VISIVEIS) ? 0 : fase.num % MAX_FASES_VISIVEIS;
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


/**
 * Exibe as instrucoes
 */
void Jogo::instrucoes() {
    bool exibindo = true;
    operacao   = -1;
    game_speed = 0;
    clear_keybuf();

    // Desenhar Instrucoes
    while (exibindo) {

        // Desenhar
        desenhar_fundo_animado();
        desenhar_titulo();
        desenhar_baloes(baloes);
        masked_blit((BITMAP *)imgs[Instrucoes].dat, buffer, 0, 0,
            (SCREEN_WIDTH - 339) / 2, 150, 339, 266);
        desenhar_creditos();
        imprimir_tela();

        aguardar();

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
                    alternar_tela();
                    break;
                }
            }
            game_speed--;
        } // while
    } // while
}


/**
 * Exibe os creditos
 */
void Jogo::creditos() {
    operacao = -1;

    tocar_musica(MFinal);
    clear_keybuf();

    // Passa os creditos
    int auxy   = SCREEN_HEIGHT,
        i      = 0;
    game_speed = 0;
    while (i <= 716 + SCREEN_HEIGHT) {

        // Desenhar Creditos
        desenhar_fundo_animado();
        masked_blit((BITMAP *)imgs[Final].dat, buffer, 0, 0,
            (SCREEN_WIDTH - 316) / 2, auxy, 316, 716);
        imprimir_tela();

        aguardar();

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

                // Avancar os creditos
                case KEY_DOWN:
                case KEY_S:
                    i += 10;
                    break;

                // Voltar os creditos
                case KEY_UP:
                case KEY_W:
                    i -= 15;
                    if (i < 0) {
                        i = 0;
                    }
                    break;
                }

            } // if
            game_speed--;
        } // while
    }

    if (zerou) {
        final();
    }
}


//
//     Exibe o final
//
void Jogo::final() {
    const unsigned char TOTAL_BALOES = 15;
    fase.num   = 1;
    fase.nivel = 1;

    // Mostra o Final: personagem anda pra direita, aprarecendo o nome do jogo, depois ele faz sinal de vitoria
    personagem.acao    = ANDANDO;
    personagem.sentido = 'R';
    personagem.X       = -TILE_SIZE;
    personagem.Y       = 400 - (400 % TILE_SIZE);
    personagem.passo   = TILE_SIZE;

    clear_keybuf();

    bool sair     = false,  // Sair do final
         terminou = false;  // Flag indicando que o personagem ja chegou no destino
    game_speed = 0;
    while (!sair) {

        // Criar balao em posicao aleatoria
        if  (baloes.size() < TOTAL_BALOES) {
            Objeto b;
            b.blocoX = (SCREEN_WIDTH / TOTAL_BALOES) * baloes.size();
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
        desenhar_fundo_animado();
        masked_blit((BITMAP *)imgs[Fim].dat, buffer, 0, 0,
            (SCREEN_WIDTH - 223) / 2, 200, 223, 98);

        desenhar_baloes(baloes);

        desenhar_letra_final((BITMAP *)imgs[LC].dat, buffer, 0, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LO].dat, buffer, 1, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LN].dat, buffer, 2, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LS].dat, buffer, 3, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LT].dat, buffer, 4, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LR].dat, buffer, 5, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LU].dat, buffer, 6, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LI].dat, buffer, 7, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LN].dat, buffer, 8, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LD].dat, buffer, 9, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LO].dat, buffer, 10, personagem.X, terminou);

        desenhar_letra_final((BITMAP *)imgs[LP].dat, buffer, 12, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LA].dat, buffer, 13, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LL].dat, buffer, 14, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LA].dat, buffer, 15, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LV].dat, buffer, 16, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LR].dat, buffer, 17, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LA].dat, buffer, 18, personagem.X, terminou);
        desenhar_letra_final((BITMAP *)imgs[LS].dat, buffer, 19, personagem.X, terminou);

        desenhar_personagem();

        imprimir_tela();

        aguardar();

        // Tratar Teclado
        if (!terminou) {
            while (game_speed > 0) {

                // Exibir animacao
                personagem.X += 2;

                // Ajustar passo do personagem
                if (personagem.X > SCREEN_WIDTH - (4 * TILE_SIZE)) {
                    personagem.acao    = VITORIA;
                    personagem.sentido = 'D';
                    terminou = true;
                } else {
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
                        sair = true;
                        break;

                    case KEY_F1:
                        alternar_tela();
                        break;
                    }
                }
                game_speed--;
            }

        // Se terminou a animcacao, o personagem pode andar livremente
        } else {
            while (game_speed > 0) {

                // Se o personagem esta parado
                if (personagem.acao != ANDANDO) {
                    personagem.passo = TILE_SIZE;

                    // posBloco indica a coordenada a frente do personagem
                    unsigned char posBlocoY = YtoBlocoY(personagem.Y, fase),
                        posBlocoX = XtoBlocoX(personagem.X, fase);

                    switch (personagem.sentido) {
                    case 'U': posBlocoY -= 1; break;
                    case 'D': posBlocoY += 1; break;
                    case 'L': posBlocoX -= 1; break;
                    case 'R': posBlocoX += 1; break;
                    }

                    if (keypressed()) {
                        inicioOcioso = clock_speed;

                        switch (readkey() >> 8) {

                        // Subir
                        case KEY_UP:
                        case KEY_W:
                            personagem.acao = ANDANDO;
                            if (personagem.sentido == 'U') {
                                personagem.Y -= velPasso;
                            }
                            personagem.sentido = 'U';
                            break;

                        // Descer
                        case KEY_DOWN:
                        case KEY_S:
                            personagem.acao = ANDANDO;
                            if (personagem.sentido == 'D') {
                                personagem.Y += velPasso;
                            }
                            personagem.sentido = 'D';
                            break;

                        // Esquerda
                        case KEY_LEFT:
                        case KEY_A:
                            personagem.acao = ANDANDO;
                            if (personagem.sentido == 'L') {
                                personagem.X -= velPasso;
                            }
                            personagem.sentido = 'L';
                            break;

                        // Direita
                        case KEY_RIGHT:
                        case KEY_D:
                            personagem.acao = ANDANDO;
                            if (personagem.sentido == 'R') {
                                personagem.X += velPasso;
                            }
                            personagem.sentido = 'R';
                            break;

                        // Alterna entre janela e tela cheia
                        case KEY_F1:
                            alternar_tela();
                            break;

                        // Apara as instrucoes ou volta pro menu inicial
                        case KEY_ESC:
                            operacao = -1;
                            sair     = true;
                            break;

                        // Personagem faz "vitoria" (ester egg)
                        case KEY_V:
                            personagem.acao = VITORIA;
                            break;
                        }

                        clear_keybuf();

                    // Se nao pressionou nenhuma tecla
                    } else {

                        // Se ficou muito tempo parado
                        if (passou(inicioOcioso, OCIOSO)) {
                            personagem.acao = DORMINDO;
                        }
                    }

                // Se o personagem nao esta parado
                } else {
                    clear_keybuf();

                    // Se nao esta em posicao discreta, continuar andando
                    if ((personagem.sentido == 'U') &&
                        (personagem.Y % TILE_SIZE)
                    ) {
                        personagem.Y     -= velPasso;
                        personagem.passo -= velPasso;
                    } else if ((personagem.sentido == 'L') &&
                        (personagem.X % TILE_SIZE)
                    ) {
                        personagem.X     -= velPasso;
                        personagem.passo -= velPasso;
                    } else if ((personagem.sentido == 'R') &&
                        (personagem.X % TILE_SIZE)
                    ) {
                        personagem.X     += velPasso;
                        personagem.passo -= velPasso;
                    } else if (personagem.Y % TILE_SIZE) {
                        personagem.Y     += velPasso;
                        personagem.passo -= velPasso;

                    // Se esta em posicao discreta, entao parar
                    } else {
                        personagem.acao = PARADO;
                        inicioOcioso    = clock_speed;
                    }
                    if (personagem.passo <= (int)velPasso) {
                        personagem.passo = TILE_SIZE;
                        tocar(Passo);
                    }
                }

                game_speed--;
            }
        } // if
    } // while
}


/**
 * Exibe o menu de opcoes
 */
void Jogo::opcoes() {
    operacao   = -1;
    game_speed = 0;
    clear_keybuf();
    menuopcoes.iniciar();

    // Desenhar menu de opcoes
    while (!menuopcoes.get_sair()) {

        // Desenhar
        desenhar_fundo_animado();
        desenhar_titulo();
        desenhar_baloes(baloes);
        menuopcoes.desenhar(imgs, buffer, comSom, volSample, volMusica, velPasso);
        desenhar_creditos();
        imprimir_tela();

        aguardar();

        // Tratar Teclado
        while (game_speed > 0) {
            if (keypressed()) {
                switch (readkey() >> 8) {
                case KEY_UP:
                case KEY_W:
                    menuopcoes.subir();
                    tocar(MoveMenu);
                    break;

                case KEY_DOWN:
                case KEY_S:
                    menuopcoes.descer();
                    tocar(MoveMenu);
                    break;

                case KEY_RIGHT:
                case KEY_D:
                    menuopcoes.aumentar(volSample, volMusica, velPasso);
                    tocar(MoveMenu);
                    break;

                case KEY_LEFT:
                case KEY_A:
                    menuopcoes.diminuir(volSample, volMusica, velPasso);
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
                    alternar_tela();
                    break;
                }
            }
            game_speed--;
        }
    }
}


/**
 * Desenha nuvens no sentido noroeste/sudeste
 */
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

/**
 * Desenha o titulo do jogo no topo
 */
void Jogo::desenhar_titulo() {
    blit((BITMAP *)imgs[Titulo].dat, buffer, 0, 0,
        (SCREEN_WIDTH - 360) / 2, 0, 360, 144);
}

/**
 * Desenha os creditos no canto inferior direito
 */
void Jogo::desenhar_creditos() {
    masked_blit((BITMAP *)imgs[Creditos].dat, buffer, 0, 0,
        SCREEN_WIDTH - 179, SCREEN_HEIGHT - 50, 215, 50);
}

/**
 * Desenha a lista de fases de cada nivel
 */
void Jogo::desenhar_menufases(int &faseinicio) {
    static int cor = 0;
    const char MAX_FASES_VISIVEIS = 17;

    // Desenha a tabela
    masked_blit((BITMAP *)imgs[Niveis].dat, buffer, 0, 0,
        (SCREEN_WIDTH - 503) / 2, 150, 503, 264);

    // Para cada nivel
    for (int i = 0; i < 3; i++) {

        // Desenhar setas quando necessario

        // Seta para baixo
        if ((faseinicio + MAX_FASES_VISIVEIS < (int)fases[fase.nivel - 1].size()) &&
            (fase.nivel - 1 == i)
        ) {
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

        // Para cada Fase do Nivel
        for (int j = 0; j < (int)fases[i].size() && j < MAX_FASES_VISIVEIS; j++) {

            // Se eh uma fase habilitada
            if (j + 1 + faseinicio <= habilitadas.nivel[i]) {

                cor = ((j + 1 + faseinicio == fase.num) &&
                    (i + 1 == fase.nivel))
                    ? makecol(0, 255, 0)       // Fase selecionada: Verde claro
                    : makecol(255, 255, 255);  // Fase nao selecionada: Branco

            // Se eh uma fase desabilitada
            } else {

                // Fase selecionada
                cor = ((j + 1 + faseinicio == fase.num) &&
                    (i + 1 == fase.nivel))
                    ? makecol(0, 100, 0)       // Fase selecionada: Verde escuro
                    : makecol(100, 100, 100);  // Fase nao selecionada: Cinza
            }
            int numfase = j + 1;
            numfase += (fase.nivel - 1 == i) ? faseinicio : 0;

            // Imprime a fase do nivel
            textprintf_ex(buffer, font,
                ((SCREEN_WIDTH - 503) / 2) + 60 + i * 165,
                150 + 35 + j * 13, cor, 0, "Fase %02d", numfase);
        }
    }
}

/**
 * Desenha a fase
 */
void Jogo::desenhar_fase() {
    int tile, tile2, w2, h2;

    // Desenhando a fase
    for (unsigned int w = 0; w < fase.piso.size(); w++) {
        for (unsigned int h = 0; h < fase.piso[0].size(); h++) {
            tile = fase.piso[w][h];
            if (!tile) {
                continue;
            }
            w2 = -1;
            h2 = -1;

            // Se eh um canto superior esquerdo arredondado
            if (tile == F2) {
                w2 = w - 1;
                h2 = h - 1;

            // Se eh um canto superior direito arredondado
            } else if (tile == F3) {
                w2 = w + 1;
                h2 = h - 1;

            // Se eh um canto inferior esquerdo arredondado
            } else if (tile == F4) {
                w2 = w - 1;
                h2 = h + 1;

            // Se eh um canto inferior direito arredondado
            } else if (tile == F5) {
                w2 = w + 1;
                h2 = h + 1;
            }
            if ((w2 >= 0) && (w2 < (int)fase.piso.size()) &&
                (h2 >= 0) && (h2 < (int)fase.piso[0].size())
            ) {
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

/**
 * Desenho quadro de informacoes do topo da fase
 */
void Jogo::desenhar_topo(unsigned int tempo, bool atualizar) {
    int topoX = (SCREEN_WIDTH - 620) / 2;

    // Fundo
    masked_blit((BITMAP *)imgs[Topo].dat, buffer, 0, 0, topoX, 10, 620, 82);

    if (personagem.mala.conteudo) {
        masked_blit((BITMAP *)imgs[personagem.mala.imagem].dat, buffer, 0, 0, topoX + 23, 51, TILE_SIZE, TILE_SIZE);
    }

    // Imprimir Fase, Nivel e Tempo
    textprintf_ex(buffer, font, topoX + 573, 34, makecol(255, 255, 0), 0, "%02d", fase.num);
    textprintf_ex(buffer, font, topoX + 573, 52, makecol(255, 255, 0), 0, "%d/3", fase.nivel);
    textout_ex(buffer, font, converter_tempo(tempo, atualizar), topoX + 573, 71, makecol(255, 255, 0), 0);

    // Imprimir Dica
    for (int j = 0; j < 3; j++) {
        textout_centre_ex(buffer, font, fase.dica[j], topoX + 295, 45 + (j * 13), -1, 0);
    }
}

/**
 * Desenha o personagem
 */
void Jogo::desenhar_personagem() {
    char aux[5];

    aux[0] = 'N'; // Ness
    aux[2] = personagem.sentido; // Sentido
    aux[4] = '\0';

    switch (personagem.acao) {
    case ANDANDO:
        aux[1] = '1'; // Andando

        if (((TILE_SIZE - 2) / personagem.NUM_QUADROS) <= personagem.passo) {
            aux[3] = '1'; // Tipo
        } else {
            aux[3] = '2'; // Tipo
        }
        break;

    case VITORIA:
        aux[1] = '2'; // Vitoria
        aux[2] = 'D'; // Sentido
        aux[3] = '1'; // Tipo
        break;
    case PARADO:
        aux[1] = '0'; // Parado
        aux[3] = '1'; // Tipo
        break;
    case DORMINDO:
        aux[1] = '3'; // Dormindo
        aux[2] = 'D'; // Sentido
        aux[3] = '1'; // Tipo
        break;
    }

    masked_blit((BITMAP *)imgs[get_imagem(aux)].dat, buffer, 0, 0,
        personagem.X, personagem.Y, TILE_SIZE, TILE_SIZE);

    if (personagem.acao == DORMINDO) {
        masked_blit((BITMAP *)imgs[ZZZ].dat, buffer, 0, 0,
            personagem.X + 20, personagem.Y - 20, 98, 23);
    }
}

/**
 * Desenha baloes no final do jogo
 */
void Jogo::desenhar_baloes(vector<Objeto> &baloes) {
    for (int i = 0; i < (int)baloes.size(); i++) {
        baloes[i].blocoY -= baloes[i].conteudo;
        if ((baloes[i].blocoY + 219) <= 0) {
            baloes[i].blocoX = wlrand() % SCREEN_WIDTH;
            baloes[i].blocoY = wlrand() % SCREEN_HEIGHT;
            baloes[i].blocoY += SCREEN_HEIGHT;
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

/**
 * Imprime uma letra durante a exibicao do final do jogo
 */
void Jogo::desenhar_letra_final(BITMAP *letra, BITMAP *buffer, int pos, int i, bool terminou) {
    if (terminou || i > ((SCREEN_WIDTH - 20 * TILE_SIZE) / 2) + (pos * TILE_SIZE)) {
        masked_blit(letra, buffer, 0, 0, ((SCREEN_WIDTH - 20 * TILE_SIZE) / 2) + (pos * TILE_SIZE),
            400 - (400 % TILE_SIZE), TILE_SIZE, TILE_SIZE);
    }
}

/**
 * Imprime a tela com base no conteudo do buffer
 */
void Jogo::imprimir_tela() {
    if (is_windowed_mode()) {
        blit(buffer, screen, 0, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    } else {
        stretch_blit(buffer, screen,
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
            0, 0, realWidth, realHeight);
    }
}

/**
 * Carrega fase ou retorna FALSE
 */
bool Jogo::carregar_fase(Fase &fase, const vector< vector<Fase> > fases) {

    // Se nao existe, passar para o proximo nivel
    if (fases[fase.nivel - 1].size() < (unsigned int)fase.num) {

        // Se eh o ultimo nivel, entao acabaram as fases
        if (fase.nivel == 3) {
            return false;

        // Se nao eh o ultimo nivel
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


/**
 * Toca um som Wave do arquivo dat
 */
void Jogo::tocar(int som) {
    play_sample((SAMPLE *)sons[som].dat, 255, 127, 1000, 0);
}

/**
 * Verifica se os campos verdes foram preenchidos incorretamente
 * FALSE: nao foram todos preenchidos ou foram preenchidos corretamente
 * TRUE: Todos foram preenchidos, mas incorretamente
 */
bool Jogo::verificar_erros() {
    bool retorno = false,
         preenchido = false;

    // Para cada campo verde
    for (int i = (fase.destinos.size() - 1); i >= 0; i--) {

        // Para cada letra da fase
        preenchido = false;
        for (int j = (fase.objetos.size() - 1); j >= 0; j--) {

            // Se a letra esta no campo verde
            if ((fase.objetos[j].blocoX == fase.destinos[i].blocoX) &&
                (fase.objetos[j].blocoY == fase.destinos[i].blocoY)) {
                preenchido = true;

                // Se a letra esta errada
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

/**
 * Verifica se existe um objeto na posicao indicada
 * e retorna o indice do objeto encontrado ou -1 se nao encontrar
 */
int Jogo::tem_objeto(unsigned char blocoX, unsigned char blocoY) {
    for (int j = fase.objetos.size() - 1; j >= 0; j--) {
        if ((fase.objetos[j].blocoX == blocoX) &&
            (fase.objetos[j].blocoY == blocoY)
        ) {
            return j;
        }
    }
    return -1;
}

/**
 * Pega um objeto da fase
 */
void Jogo::pegar_objeto(unsigned char indice_objeto) {
    personagem.mala = fase.objetos[indice_objeto];
    fase.objetos.erase(fase.objetos.begin() + indice_objeto);
}

/**
 * Larga o objeto da mala na coordenada indicada
 * Retorna se acabou a fase ou nao
 */
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
                (fase.destinos[i].blocoY == fase.objetos[j].blocoY)
            ) {

                // Se o conteudo do bloco esta errado
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

        // Se nao ha objetos neste bloco
        if (!podeContinuar) {
            return false;
        }
    }
    return true;
}

/**
 * Retorna se uma posicao da fase eh passavel ou nao
 */
bool Jogo:: passavel(int blocoX, int blocoY) {
    if ((blocoX >= 0) && (blocoX < (int)fase.piso.size()) &&
        (blocoY >= 0) && (blocoY < (int)fase.piso[0].size())
    ) {
        if (!fase.passavel[blocoX][blocoY]) {
            return false;
        }
    } else {
        return false;
    }

    for (int i = fase.objetos.size() - 1; i >= 0; i--) {
        if ((fase.objetos[i].blocoX == blocoX) &&
            (fase.objetos[i].blocoY == blocoY)
        ) {
            return false;
        }
    }

    return true;
}

/**
 * Encerra as funcionalidades da Allegro
 */
void Jogo::encerrar() {
    std::cout << "[LOG] Encerrando recursos" << std::endl;
    clear_keybuf();
    if (musicaAtual != NULL) {
        std::cout << "[LOG] Encerrando musica" << std::endl;
        stop_sample(musicaAtual);
        destroy_sample(musicaAtual);
        musicaAtual = NULL;
    }

    std::cout << "[LOG] Encerrando midi" << std::endl;
    stop_midi();

    std::cout << "[LOG] Encerrando graficos" << std::endl;
    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);

    std::cout << "[LOG] Salvando fases habilitadas" << std::endl;
    salvar_habilitadas();

    std::cout << "[LOG] Liberando imagens" << std::endl;
    unload_datafile(imgs);

    std::cout << "[LOG] Liberando sons" << std::endl;
    unload_datafile(sons);

    std::cout << "[LOG] Liberando buffer" << std::endl;
    destroy_bitmap(buffer);

    std::cout << "[LOG] Encerrando allegro" << std::endl;
    allegro_exit();
}
