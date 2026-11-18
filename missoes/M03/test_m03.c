/* Testes aditivos da M03. Executar a partir da raiz com o Makefile incremental. */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "lse.h"

/* ---------- estado e callbacks auxiliares ---------- */
static int contador_visitas = 0;
static void *esperado[3];       /* cargas na ordem em que devem ser visitadas */
static int destruidas = 0;

/* Apenas conta quantas vezes foi chamado. */
static void contar_visita(void *carga) {
    (void)carga;
    contador_visitas++;
}

/* Confere que a carga visitada e a esperada naquela posicao. */
static void verificar_ordem(void *carga) {
    assert(contador_visitas < 3);
    assert(carga == esperado[contador_visitas]);
    contador_visitas++;
}

/* Libera uma carga int alocada no heap e conta a chamada. */
static void destruir_int(void *carga) {
    free(carga);
    destruidas++;
}

/* Conta os nos pelo percurso (unica forma de olhar a cadeia sem t_elem). */
static int contar_nos(const t_lse *l) {
    contador_visitas = 0;
    lse_percorrer(l, contar_visita);
    return contador_visitas;
}

/* ---------- testes ---------- */
static void teste_lista_vazia(void) {
    t_lse *l = lse_criar();
    assert(l != NULL);
    assert(lse_tamanho(l) == 0);
    assert(lse_primeiro(l) == NULL);
    assert(contar_nos(l) == 0);             /* percurso sem visitas */
    lse_percorrer(NULL, contar_visita);     /* nao faz nada, nao quebra */
    lse_percorrer(l, NULL);                 /* nao faz nada, nao quebra */
    lse_destruir(l, NULL);
}

static void teste_encadeamento_e_percurso(void) {
    int a = 1, b = 2, c = 3;
    t_lse *l = lse_criar();
    assert(l != NULL);

    /* um elemento */
    assert(lse_inserir_inicio(l, &a) == 1);
    assert(lse_tamanho(l) == 1);
    assert(lse_primeiro(l) == &a);
    assert(contar_nos(l) == lse_tamanho(l));
    contador_visitas = 0;
    esperado[0] = &a;
    lse_percorrer(l, verificar_ordem);
    assert(contador_visitas == 1);

    /* tres elementos: ordem inversa a insercao (LIFO na cabeca) */
    assert(lse_inserir_inicio(l, &b) == 1);
    assert(lse_tamanho(l) == 2);
    assert(lse_primeiro(l) == &b);
    assert(contar_nos(l) == lse_tamanho(l));
    assert(lse_inserir_inicio(l, &c) == 1);
    assert(lse_tamanho(l) == 3);
    assert(lse_primeiro(l) == &c);
    assert(contar_nos(l) == lse_tamanho(l));

    contador_visitas = 0;
    esperado[0] = &c;
    esperado[1] = &b;
    esperado[2] = &a;
    lse_percorrer(l, verificar_ordem);
    assert(contador_visitas == 3);

    lse_destruir(l, NULL);                  /* cargas sao variaveis automaticas */
}

static void teste_carga_generica(void) {
    /* outro tipo de carga: struct e string, sem a lista saber o tipo */
    struct ponto { int x; int y; } p = { 7, 9 };
    const char *texto = "miniplayer";
    int n = 42;
    t_lse *l = lse_criar();
    assert(l != NULL);

    assert(lse_inserir_inicio(l, &n) == 1);
    assert(lse_inserir_inicio(l, (void *)texto) == 1);
    assert(lse_inserir_inicio(l, &p) == 1);
    assert(lse_tamanho(l) == 3);

    struct ponto *pp = lse_primeiro(l);
    assert(pp == &p && pp->x == 7 && pp->y == 9);

    contador_visitas = 0;
    esperado[0] = &p;
    esperado[1] = (void *)texto;
    esperado[2] = &n;
    lse_percorrer(l, verificar_ordem);
    assert(contador_visitas == 3);

    lse_destruir(l, NULL);
}

static void teste_carga_nula(void) {
    /* decisao da M03: carga NULL e aceita; o no existe e e contado */
    t_lse *l = lse_criar();
    assert(l != NULL);
    assert(lse_inserir_inicio(l, NULL) == 1);
    assert(lse_tamanho(l) == 1);
    assert(lse_primeiro(l) == NULL);        /* ambiguo: use lse_tamanho para distinguir */
    assert(contar_nos(l) == 1);             /* o callback e chamado uma vez, com NULL */
    lse_destruir(l, NULL);
}

static void teste_destruicao_com_callback(void) {
    t_lse *l = lse_criar();
    assert(l != NULL);
    for (int i = 0; i < 3; i++) {
        int *p = malloc(sizeof *p);
        assert(p != NULL);
        *p = i;
        assert(lse_inserir_inicio(l, p) == 1);
    }
    assert(lse_tamanho(l) == 3);
    destruidas = 0;
    lse_destruir(l, destruir_int);          /* libera 3 cargas + 3 nos + lista */
    assert(destruidas == 3);

    lse_destruir(NULL, destruir_int);       /* NULL: nao faz nada */
    assert(destruidas == 3);
}

int main(void) {
    teste_lista_vazia();
    teste_encadeamento_e_percurso();
    teste_carga_generica();
    teste_carga_nula();
    teste_destruicao_com_callback();
    assert(lse_inserir_inicio(NULL, NULL) == 0);
    puts("M03: todos os testes passaram.");
    return 0;
}