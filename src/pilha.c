#include "pilha.h"
#include "lse.h"
#include <stddef.h>
#include <stdlib.h>
struct pilha
{
    t_lse *lista;
};
/* M06: implementar apenas delegando para lse_*. */
t_pilha *pilha_criar(void) { 
    t_lse *lista = lse_criar();
    if (lista == NULL)
    {
        return NULL;
    }    
    t_pilha *pilha = malloc(sizeof(t_pilha));
    if(pilha == NULL){
        lse_destruir(lista, NULL);
        return NULL;
    }
    
    pilha->lista=lista;
    return pilha; 
    
}
int pilha_empilhar(t_pilha *p, void *c)
{
    (void)p;
    (void)c;
    return 0;
}
void *pilha_desempilhar(t_pilha *p)
{
    (void)p;
    return NULL;
}
void *pilha_topo(const t_pilha *p)
{
    if (p == NULL)
    {
        return NULL;
    }

    if (p->lista == NULL)
    {
        return NULL;
    }
    
    t_lse *lista = p->lista; 
    
    return lse_primeiro(lista);
}
void pilha_destruir(t_pilha *p, void (*d)(void *))
{
    if (p != NULL && p->lista != NULL)
    {
        lse_destruir(p->lista,d);
    }
    free(p);
}
