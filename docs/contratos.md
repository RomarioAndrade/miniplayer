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
