#include "lse.h"
#include <stddef.h>
#include <stdlib.h>

/* Representacao interna: NAO mover para lse.h. */
typedef struct elem
{
    void *carga;
    struct elem *prox;
} t_elem;

struct lse
{
    t_elem *inicio;
    int tamanho;
    /* M08 (otimizacao opcional): t_elem *fim; */
};

/* M02: criacao, consultas e destruicao implementadas.
   Demais funcoes continuam como stubs ate as proximas missoes. */

/* Devolve lista vazia alocada ou NULL em falha de memoria. */
t_lse *lse_criar(void)
{
    t_lse *lista = malloc(sizeof(t_lse));
    if (lista == NULL)
    {
        return NULL;
    }
    lista->tamanho = 0;
    lista->inicio = NULL;
    return lista;
} /* M02 */

/* Devolve o numero de elementos; -1 se a lista for NULL. */
int lse_tamanho(const t_lse *l)
{
    if (l == NULL)
    {
        return -1;
    }
    return l->tamanho;
} /* M02 */

/* Devolve a carga do primeiro elemento sem remove-lo; NULL se vazia ou NULL. */
void *lse_primeiro(const t_lse *l)
{
    if (l == NULL || l->inicio == NULL)
    {
        return NULL;
    }
    return l->inicio->carga;
} /* M02 */

int lse_inserir_inicio(t_lse *l, void *c)
{
    if (l == NULL || c == NULL)
    {
        return 0;
    }

    t_elem novo = malloc(sizeof(t_elem)); / cria um nó novo */
    if (novo == NULL)
    {
        return 0; /* sem memória: lista fica igual */
    }
    novo->carga = c;        /* guarda a carga dentro do nó */
    novo->prox = l->inicio; /* novo aponta para o antigo primeiro */
    l->inicio = novo;       /* novo vira o primeiro */
    l->tamanho++;
    return 1;

}/* M03 */
int lse_inserir_fim(t_lse *l, void *c)
{
    (void)l;
    (void)c;
    return 0;
} /* TODO M04 */
void *lse_remover_inicio(t_lse *l)
{
    (void)l;
    return NULL;
} /* TODO M04 */
void *lse_buscar(const t_lse *l, const void *ch, t_comparador cmp)
{
    (void)l;
    (void)ch;
    (void)cmp;
    return NULL;
} /* TODO M04 */
void *lse_remover(t_lse *l, const void *ch, t_comparador cmp)
{
    (void)l;
    (void)ch;
    (void)cmp;
    return NULL;
} /* TODO M04 */
int lse_inserir_ordenado(t_lse *l, void *c, t_comparador cmp)
{
    (void)l;
    (void)c;
    (void)cmp;
    return 0;
} /* TODO M05 */

/* Destroi cada carga com 'destruir' e libera a lista (NULL: nao toca nas cargas). */
void lse_destruir(t_lse *l, void (*destruir)(void *))
{
    if (l == NULL)
    {
        return;
    }
    t_elem *atual = l->inicio;
    while (atual != NULL)
    {
        t_elem *prox = atual->prox;
        if (destruir != NULL)
        {
            destruir(atual->carga);
        }
        free(atual);
        atual = prox;
    }
    free(l);
} /* M02 */
