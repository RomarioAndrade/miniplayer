#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "musica.h"
int main(void) {
    t_musica *m = musica_criar(1, "wasurerarenai", "sakanaction", 4);

    assert(m != NULL); /* Falha ate M01 ser implementada. */
    assert(m->id == 1 && strcmp(m->titulo, "wasurerarenai") == 0);
    assert(m->duracao == 4);

    musica_imprimir(m);

    musica_destruir(m);
    //puts("test_musica: OK");
    return 0;
}
