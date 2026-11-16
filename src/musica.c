#include "musica.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

/* M01: substituir stubs por alocacao, copia segura de strings e free. */

t_musica *musica_criar(int id, const char *titulo, const char *artista, int duracao)
{
    t_musica *musica = malloc(sizeof(t_musica));
    musica->id = id;
    strcpy(musica->titulo, titulo);
    strcpy(musica->artista, artista);
    musica->duracao = duracao;

    return musica;
}

/* Imprime titulo, artista e duracao da musica.
   @param carga ponteiro para t_musica (recebido como void* para servir de
          callback generico da LSE); se NULL, nao imprime nada. */
void musica_imprimir(void *carga){
    if (carga != NULL){
        t_musica *musica = carga;
        printf("Musica: %s - %s , duração: %d\n", musica->titulo,musica->artista,musica->duracao);
    }    
}

void musica_destruir(void *carga)
{
    assert(carga!=NULL);    
    free((t_musica*)carga);
    
}
