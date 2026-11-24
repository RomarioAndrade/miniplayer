/* Testes incrementais M05 — não acessar campos privados de t_lse/t_elem. */
#include <assert.h>
#include <stdio.h>
#include "lse.h"

static int comparar_int(const void *a, const void *b) {
    int x = *(const int *)a, y = *(const int *)b;
    return (x > y) - (x < y);
}

typedef struct { int id; const char *titulo; } musica_t;
static int comparar_musica(const void *a, const void *b) {
    int x = ((const musica_t *)a)->id, y = ((const musica_t *)b)->id;
    return (x > y) - (x < y);
}

/* Auxiliares: conferem tamanho e ordem exata dos ponteiros de carga. */
static const void *esperados[8];
static int visitas;
static void verificar(void *carga) {
    assert(visitas < 8);
    assert(carga == esperados[visitas]);
    visitas++;
}
static void conferir(t_lse *l, int n) {
    assert(lse_tamanho(l) == n);
    visitas = 0;
    lse_percorrer(l, verificar);
    assert(visitas == n);
}

static void teste_vazia_inicio_meio_fim(void) {
    int a = 20, b = 10, c = 30, d = 15, e = 5, f = 40;
    t_lse *l = lse_criar(); assert(l);

    /* lista vazia */
    assert(lse_inserir_ordenado(l, &a, comparar_int) == 1);
    esperados[0] = &a; conferir(l, 1);

    /* início: menor que todos */
    assert(lse_inserir_ordenado(l, &b, comparar_int) == 1);
    esperados[0] = &b; esperados[1] = &a; conferir(l, 2);

    /* fim: maior que todos */
    assert(lse_inserir_ordenado(l, &c, comparar_int) == 1);
    esperados[0] = &b; esperados[1] = &a; esperados[2] = &c; conferir(l, 3);

    /* meio: entre 10 e 20 */
    assert(lse_inserir_ordenado(l, &d, comparar_int) == 1);
    esperados[0] = &b; esperados[1] = &d; esperados[2] = &a; esperados[3] = &c;
    conferir(l, 4);

    /* novo início (5) e novo fim (40) com a lista já com vários nós */
    assert(lse_inserir_ordenado(l, &e, comparar_int) == 1);
    assert(lse_inserir_ordenado(l, &f, comparar_int) == 1);
    esperados[0] = &e; esperados[1] = &b; esperados[2] = &d;
    esperados[3] = &a; esperados[4] = &c; esperados[5] = &f;
    conferir(l, 6);

    lse_destruir(l, NULL);
}

static void teste_duplicados_estaveis(void) {
    musica_t a={10,"primeira"}, b={10,"segunda"}, c={5,"menor"}, d={10,"terceira"};
    musica_t e={20,"maior"};
    t_lse *l=lse_criar(); assert(l);

    /* três registros com o mesmo id (10) mantêm a ordem de chegada */
    assert(lse_inserir_ordenado(l, &a, comparar_musica) == 1);
    assert(lse_inserir_ordenado(l, &b, comparar_musica) == 1);
    esperados[0] = &a; esperados[1] = &b; conferir(l, 2);

    /* id menor entra antes de todos */
    assert(lse_inserir_ordenado(l, &c, comparar_musica) == 1);
    esperados[0] = &c; esperados[1] = &a; esperados[2] = &b; conferir(l, 3);

    /* novo empate entra depois dos iguais já existentes */
    assert(lse_inserir_ordenado(l, &d, comparar_musica) == 1);
    esperados[0] = &c; esperados[1] = &a; esperados[2] = &b; esperados[3] = &d;
    conferir(l, 4);

    /* id maior entra no fim, depois do grupo de empates */
    assert(lse_inserir_ordenado(l, &e, comparar_musica) == 1);
    esperados[0] = &c; esperados[1] = &a; esperados[2] = &b;
    esperados[3] = &d; esperados[4] = &e;
    conferir(l, 5);

    /* a lista guarda ponteiros: as cargas continuam sendo as originais */
    assert(lse_primeiro(l) == &c);

    lse_destruir(l,NULL);
}

static void teste_invalido_e_reinsercao(void) {
    int a = 10, b = 20, c = 15;
    t_lse *l = lse_criar(); assert(l);

    /* argumentos inválidos: retorna 0 e não altera nada */
    assert(lse_inserir_ordenado(NULL, &a, comparar_int) == 0);
    assert(lse_inserir_ordenado(l, &a, NULL) == 0);
    conferir(l, 0);

    assert(lse_inserir_ordenado(l, &a, comparar_int) == 1);
    assert(lse_inserir_ordenado(l, &b, comparar_int) == 1);
    esperados[0] = &a; esperados[1] = &b; conferir(l, 2);

    /* falha com comparador NULL em lista não vazia: lista intacta */
    assert(lse_inserir_ordenado(l, &c, NULL) == 0);
    esperados[0] = &a; esperados[1] = &b; conferir(l, 2);

    /* após a falha, uma inserção válida ainda funciona */
    assert(lse_inserir_ordenado(l, &c, comparar_int) == 1);
    esperados[0] = &a; esperados[1] = &c; esperados[2] = &b; conferir(l, 3);

    /* reinserir a mesma carga cria um novo nó, depois do igual existente */
    assert(lse_inserir_ordenado(l, &c, comparar_int) == 1);
    esperados[0] = &a; esperados[1] = &c; esperados[2] = &c; esperados[3] = &b;
    conferir(l, 4);

    /* integração com a M04: remover e inserir de novo mantém a ordem */
    assert(lse_remover(l, &a, comparar_int) == &a);
    assert(lse_inserir_ordenado(l, &a, comparar_int) == 1);
    esperados[0] = &a; esperados[1] = &c; esperados[2] = &c; esperados[3] = &b;
    conferir(l, 4);

    lse_destruir(l, NULL);
}

int main(void) {
    teste_vazia_inicio_meio_fim();
    teste_duplicados_estaveis();
    teste_invalido_e_reinsercao();
    puts("M05: todos os testes passaram.");
    return 0;
}