# Contratos dos TADs
Para **cada função**, preencher:
- Nome e assinatura:
- Pré-condições:
- Pós-condições:
- Valor de retorno em sucesso/falha:
- Quem aloca/libera a memória:
- Invariantes preservadas:
- Testes associados:

## Invariantes da LSE
- `tamanho >= 0`.
- `tamanho == 0` se e somente se `inicio == NULL`.
- Há exatamente `tamanho` nós alcançáveis a partir de `inicio`.
- O último nó aponta para `NULL` e não há ciclos.

## M02 — Contratos da LSE (lista vazia)

### `t_lse *lse_criar(void)`
- Pré-condições: nenhuma.
- Pós-condições: em sucesso, lista com `inicio == NULL` e `tamanho == 0`.
- Retorno: ponteiro para a lista em sucesso; `NULL` se `malloc` falhar (nada fica alocado).
- Memória: a LSE aloca a estrutura `t_lse`; quem chama deve liberá-la com `lse_destruir`.
- Invariantes: estabelece todas as invariantes da LSE (lista vazia).
- Testes: `missoes/M02/TESTE_INCREMENTAL_M02.c` (`teste_lista_vazia`), `tests/test_lse.c`.

### `int lse_tamanho(const t_lse *lista)`
- Pré-condições: `lista` criada por `lse_criar` (ou `NULL`).
- Pós-condições: a lista não é alterada.
- Retorno: número de elementos; `0` para lista recém-criada; `0` se `lista == NULL` (decisão defensiva).
- Memória: não aloca nem libera.
- Invariantes: preservadas (função somente de consulta).
- Testes: `teste_lista_vazia`.

### `void *lse_primeiro(const t_lse *lista)`
- Pré-condições: `lista` criada por `lse_criar` (ou `NULL`).
- Pós-condições: a lista não é alterada; a carga não é removida.
- Retorno: carga do primeiro nó; `NULL` se a lista estiver vazia ou for `NULL`.
- Memória: não aloca nem libera; a carga continua pertencendo a quem a inseriu.
- Invariantes: preservadas (função somente de consulta).
- Testes: `teste_lista_vazia`.

### `void lse_destruir(t_lse *lista, void (*destruir_carga)(void *))`
- Pré-condições: `lista` criada por `lse_criar`; `destruir_carga` sabe destruir as cargas (ex.: `musica_destruir`).
- Pós-condições: cargas, nós e lista liberados.
- Retorno: nenhum.
- Memória: com `destruir_carga == NULL`, as cargas não são liberadas.
- Invariantes: não se aplica (a lista deixa de existir).
- Testes: `teste_lista_vazia` (lista vazia, `destruir_carga == NULL`), verificado sem vazamentos com `-fsanitize=address`.


## M03 — Contratos da LSE (inserção no início e percurso)

### `int lse_inserir_inicio(t_lse *lista, void *carga)`
- Pré-condições: `lista` criada por `lse_criar` (ou `NULL`); `carga` pode ser qualquer ponteiro, inclusive `NULL`.
- Pós-condições: em sucesso, o novo nó passa a ser o primeiro, seu `prox` aponta para o antigo primeiro e `tamanho` aumenta em 1. Em falha, a lista não é alterada.
- Retorno: `1` em sucesso; `0` se `lista == NULL` ou se o `malloc` do nó falhar.
- Memória: a LSE aloca o nó (`t_elem`). A carga não é copiada nem alocada pela lista, e continua pertencendo a quem a inseriu. O nó é liberado por `lse_destruir`; a carga só é liberada se for passada uma função `destruir_carga`.
- Decisão sobre carga `NULL`: é aceita. Um nó com carga `NULL` é um nó válido e conta no `tamanho`. Consequência: `lse_primeiro` devolve `NULL` tanto para lista vazia quanto para primeiro elemento com carga `NULL`; para distinguir, usar `lse_tamanho`.
- Invariantes preservadas: após a inserção, `inicio != NULL` e `tamanho > 0`; o último nó continua com `prox == NULL`; o número de nós alcançáveis a partir de `inicio` é igual a `tamanho`; não há ciclos.
- Testes: `missoes/M03/test_m03.c` (`teste_encadeamento_e_percurso`, `teste_carga_generica`, `teste_carga_nula`, `lse_inserir_inicio(NULL, NULL) == 0` no `main`).

### `void lse_percorrer(const t_lse *lista, t_lse_visitar cb_visitar)`
- Pré-condições: `lista` criada por `lse_criar` (ou `NULL`); `cb_visitar` é uma função `void f(void *carga)` (ou `NULL`).
- Pós-condições: a lista não é alterada; `cb_visitar` é chamado uma vez para cada nó, do início ao fim, recebendo a carga do nó.
- Retorno: nenhum. Lista vazia: o callback nunca é chamado. `lista == NULL` ou `cb_visitar == NULL`: não faz nada.
- Memória: não aloca nem libera. O callback não deve inserir nem remover nós durante o percurso.
- Invariantes: preservadas (função somente de leitura sobre a cadeia).
- Testes: `teste_lista_vazia`, `teste_encadeamento_e_percurso`, `teste_carga_generica`, `teste_carga_nula`.