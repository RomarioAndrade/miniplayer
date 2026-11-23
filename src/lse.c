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

/* Devolve o numero de elementos; 0 se a lista for NULL. */
int lse_tamanho(const t_lse *l)
{
    if (l == NULL)
    {
        return 0;
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
    if (l == NULL)
    {
        return 0;
    }

    t_elem *novo = malloc(sizeof(t_elem)); /* cria um nó novo */
    if (novo == NULL)
    {
        return 0; /* sem memória: lista fica igual */
    }
    novo->carga = c;        /* guarda a carga dentro do nó */
    novo->prox = l->inicio; /* novo aponta para o antigo primeiro */
    l->inicio = novo;       /* novo vira o primeiro */
    l->tamanho++;
    return 1;

} /* M03 */

/* Visita as cargas do inicio ao fim. Nao faz nada se lista ou callback for NULL. */
void lse_percorrer(const t_lse *l, t_lse_visitar cb_visitar)
{
    if (l == NULL || cb_visitar == NULL)
    {
        return;
    }
    for (t_elem *atual = l->inicio; atual != NULL; atual = atual->prox)
    {
        cb_visitar(atual->carga);
    }
} /* M03 */

/* Insere a carga no fim (percorre a cadeia, O(n)). Devolve 1 em sucesso;
   0 se lista NULL ou malloc falhar (lista nao e alterada). Carga NULL e permitida. */
int lse_inserir_fim(t_lse *l, void *c)
{
    if (l == NULL)
    {
        return 0;
    }

    t_elem *novo = malloc(sizeof(t_elem));
    if (novo == NULL)
    {
        return 0;
    }

    novo->carga = c;
    novo->prox = NULL;

    if (l->inicio == NULL)
    {
        l->inicio = novo;
    }
    else
    {
        t_elem *atual = l->inicio;
        while (atual->prox != NULL)
        {
            atual = atual->prox;
        }
        atual->prox = novo;
    }

    l->tamanho++;
    return 1;
} /* M04 */
void *lse_remover_inicio(t_lse *l)
{
    if (l == NULL || l->inicio == NULL) // decisão de projeto
    {
        return NULL;
    }

    t_elem *del_node = l->inicio;
    void *carga = del_node->carga;
    l->inicio = del_node->prox;
    l->tamanho--;

    // free(del_node->carga);
    free(del_node);

    return carga;

} /* M04 */

/* Busca a carga cuja chave satisfaz cmp(carga, chave) == 0; NULL se nao encontrar.
    @param t_lse*
    @param void*ch
    @param t_comparador*
    @return void *carga
*/
void *lse_buscar(const t_lse *l, const void *ch, t_comparador cmp)
{
    if (l == NULL || cmp == NULL)
    {
        return NULL;
    }
    // seguindo a mesma logica do for em lse_percorrer
    for (t_elem *elemento = l->inicio; elemento != NULL; elemento = elemento->prox)
    {
        if (cmp(elemento->carga, ch) == 0)
        {
            return elemento->carga;
        }
    }
    return NULL;
} /* M04 */

/* Remove o PRIMEIRO no cuja carga satisfaz cmp(carga, chave) == 0.
   Libera somente o no; a carga e devolvida a quem chamou (nao e liberada aqui).
   Devolve NULL se nao encontrar, ou se lista/cmp for NULL. */
void *lse_remover(t_lse *l, const void *ch, t_comparador cmp)
{
    if (l == NULL || cmp == NULL)
    {
        return NULL;
    }

    t_elem *anterior = NULL;
    t_elem *atual = l->inicio;
    while (atual != NULL)
    {
        if (cmp(atual->carga, ch) == 0)
        {
            if (anterior == NULL)
            {
                l->inicio = atual->prox; /* removeu o primeiro no */
            }
            else
            {
                anterior->prox = atual->prox; /* "pula" o no removido */
            }
            void *carga = atual->carga;
            free(atual); /* libera so o no, nunca a carga */
            l->tamanho--;
            return carga;
        }
        anterior = atual;
        atual = atual->prox;
    }
    return NULL; /* chave nao encontrada */
} /* M04 */

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
