#include <assert.h>
#include <stdio.h>
#include "pilha.h"

/* Teste do kit (M02): empilha dois, desempilha dois. */
static void teste_basico(void) {
    t_pilha *p = pilha_criar(); assert(p != NULL);
    int a=1,b=2; assert(pilha_empilhar(p,&a)); assert(pilha_empilhar(p,&b));
    assert(pilha_desempilhar(p)==&b); assert(pilha_desempilhar(p)==&a);
    assert(pilha_desempilhar(p)==NULL);
    pilha_destruir(p,NULL);
}

/* M06, atividade 1: empilhar A, B, C; consultar topo; desempilhar; empilhar D.
 * Estado esperado (topo à esquerda): [C B A] -> topo C -> [B A] -> [D B A].
 * Ordem de saída restante: D, B, A. */
static void teste_cenario_enunciado(void) {
    char A='A', B='B', C='C', D='D';
    t_pilha *p = pilha_criar(); assert(p != NULL);

    assert(pilha_empilhar(p,&A)==1); assert(pilha_topo(p)==&A);
    assert(pilha_empilhar(p,&B)==1); assert(pilha_topo(p)==&B);
    assert(pilha_empilhar(p,&C)==1); assert(pilha_topo(p)==&C);

    assert(pilha_topo(p)==&C);          /* consulta não remove */
    assert(pilha_desempilhar(p)==&C);
    assert(pilha_topo(p)==&B);          /* topo volta ao anterior */

    assert(pilha_empilhar(p,&D)==1);
    assert(pilha_topo(p)==&D);

    assert(pilha_desempilhar(p)==&D);
    assert(pilha_desempilhar(p)==&B);
    assert(pilha_desempilhar(p)==&A);
    assert(pilha_desempilhar(p)==NULL);
    assert(pilha_topo(p)==NULL);
    pilha_destruir(p,NULL);
}

/* M06, atividade 4: intercalação longa de empilhar/desempilhar/topo,
 * conferida contra um vetor que serve de modelo da pilha. */
static void teste_intercalacao_com_modelo(void) {
    enum { N = 8 };
    int v[N] = {10,20,30,40,50,60,70,80};
    /* 'E' = empilhar o próximo de v, 'D' = desempilhar.
     * Passa por: pilha esvaziada no meio, desempilhar em pilha vazia
     * (9º passo) e reutilização depois de esvaziar. */
    const char *ops = "EEDEEDDDDEEDEED";
    int *modelo[N];
    int altura = 0, prox = 0;

    t_pilha *p = pilha_criar(); assert(p != NULL);
    for (const char *op = ops; *op != '\0'; ++op) {
        if (*op == 'E') {
            assert(prox < N);
            assert(pilha_empilhar(p,&v[prox])==1);
            modelo[altura++] = &v[prox++];
        } else {
            void *esperado = (altura > 0) ? modelo[--altura] : NULL;
            assert(pilha_desempilhar(p)==esperado);
        }
        /* depois de cada operação o topo bate com o modelo */
        assert(pilha_topo(p)==((altura > 0) ? modelo[altura-1] : NULL));
    }
    assert(prox == N && altura == 2);   /* usou tudo; sobram dois */
    while (altura > 0) {
        assert(pilha_desempilhar(p)==modelo[--altura]);
    }
    assert(pilha_desempilhar(p)==NULL);
    pilha_destruir(p,NULL);
}

int main(void) {
    teste_basico();
    teste_cenario_enunciado();
    teste_intercalacao_com_modelo();
    puts("test_pilha: OK"); return 0;
}
