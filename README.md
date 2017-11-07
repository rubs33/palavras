# Construindo Palavras

Construindo Palavras é um jogo com fases voltadas para alfabetização e outras voltadas para lógica.

## Requisitos

* Compilador de C++ (gcc-g++)
* Biblioteca Allegro 4 (allegro-devel)

## Modo de compilação

Linux

```
g++ principal.cpp `allegro-config --libs` -O3 -o palavras
```

Windows:

```
g++ principal.cpp `allegro-config --libs` -mwindows -DWINDOWS=1 -O3 -o palavras.exe
```
