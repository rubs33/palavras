# Construindo Palavras

Construindo Palavras é um jogo com fases voltadas para alfabetização e outras voltadas para lógica.

## Requisitos

* Compilador de C++ (gcc-g++)
* Biblioteca Allegro 4 (allegro-devel)

## Modo de compilação

Linux

```sh
./build.sh
```

OU

```sh
g++ *.cpp `allegro-config --libs` -O3 -o palavras
```

Windows:

```sh
g++ *.cpp `allegro-config --libs` -mwindows -DWINDOWS=1 -O3 -o palavras.exe
```

## Formato dos arquivos de fases

Arquivo texto contendo:

* largura da fase (número de blocos)
* altura da fase (número de blocos)
* lista com os blocos de parede ou chão (precisa ser do tamanho especificado por largura x altura)
* coordenada X inicial do personagem
* coordenada Y inicial do personagem
* quantidade de objetos (letras) da fase
* lista com a letra da fase, coordenada X e coordenada Y
* quantidade de blocos verdes (que recebem as letras)
* lista com a letra do bloco verde, coordenada X e coordenada Y
* linha 1 da dica (até 52 caracteres em ISO-8859-1)
* linha 2 da dica (até 52 caracteres em ISO-8859-1)
* linha 3 da dica (até 52 caracteres em ISO-8859-1)

Nome do arquivo: "{NIVEL}{NUMERO}.txt"

NIVEL precisa ser:

* "f" para Fácil/Iniciante
* "m" para Médio/Intermediário
* "d" para Difícil/Avançado

NUMERO precisa ter 2 dígitos, iniciados em "01".