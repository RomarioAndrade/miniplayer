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

## M04 — Contratos da LSE (inserção no fim, busca e remoção)

Nesta missão a LSE continua sem ponteiro `fim`. Por isso `lse_inserir_fim`, `lse_buscar` e `lse_remover` percorrem a cadeia e custam O(n); `lse_remover_inicio` custa O(1).

### `int lse_inserir_fim(t_lse *lista, void *carga)`
- Pré-condições: `lista` criada por `lse_criar` (ou `NULL`); `carga` pode ser qualquer ponteiro, inclusive `NULL` (decisão da M03).
- Pós-condições: em sucesso, um novo nó com a carga passa a ser o último, o antigo último aponta para ele, `tamanho` aumenta em 1 e a ordem dos demais elementos é preservada. Em lista vazia, o novo nó passa a ser também o primeiro. Em falha, a lista não é alterada.
- Retorno: `1` em sucesso; `0` se `lista == NULL` ou se o `malloc` do nó falhar.
- Memória: a LSE aloca o nó (`t_elem`) e confere o `malloc` antes de usá-lo. A carga não é copiada e continua pertencendo a quem a inseriu.
- Invariantes preservadas: o último nó tem `prox == NULL`; há exatamente `tamanho` nós alcançáveis a partir de `inicio`; `tamanho == 0` só se `inicio == NULL`; sem ciclos.
- Testes: `missoes/M04/test_m04.c` (`teste_inserir_fim_e_buscar`, `teste_remover_cabeca_meio_cauda`, `teste_remover_inicio`, `teste_generico_musicas`, `teste_argumentos_invalidos`, que confere `lse_inserir_fim(NULL, &x) == 0`).

### `void *lse_buscar(const t_lse *lista, const void *chave, t_comparador comparar)`
- Pré-condições: `lista` criada por `lse_criar` (ou `NULL`); `comparar(carga, chave)` devolve `0` quando a carga corresponde à chave e não altera a lista.
- Pós-condições: a lista não é alterada (nem a ordem, nem o `tamanho`).
- Retorno: a **primeira** carga, na ordem da lista, para a qual `comparar(carga, chave) == 0`; `NULL` se nenhuma corresponder, ou se `lista` ou `comparar` for `NULL`.
- Memória: não aloca nem libera. A carga devolvida continua pertencendo a quem a inseriu e continua na lista.
- Invariantes: preservadas (função somente de consulta).
- Testes: `teste_inserir_fim_e_buscar` (elemento presente no início, no meio e no fim; ausente), `teste_generico_musicas` (busca por `id` em registros de música), `teste_argumentos_invalidos` (lista `NULL` e lista vazia).

### `void *lse_remover(t_lse *lista, const void *chave, t_comparador comparar)`
- Pré-condições: `lista` criada por `lse_criar` (ou `NULL`); `comparar` é uma função válida (não `NULL`) com o mesmo significado de `lse_buscar`.
- Pós-condições: em sucesso, remove só a **primeira** ocorrência correspondente à chave e religa a cadeia (se era a cabeça, `inicio` passa a apontar para o próximo; senão, o nó anterior passa a apontar para o próximo), e `tamanho` diminui em 1. Se não houver correspondência, a lista não é alterada.
- Retorno: a carga removida; `NULL` se não encontrar, ou se `lista` ou `comparar` for `NULL`.
- Memória: a LSE libera **somente o nó** (`t_elem`). A carga não é liberada: ela é devolvida a quem chamou, que passa a ser o responsável por ela e deve liberá-la se tiver sido alocada no heap. Depois de removida, a carga não é mais alcançada por `lse_destruir`.
- Invariantes preservadas: com a lista vazia depois da remoção, `inicio == NULL` e `tamanho == 0`; o último nó continua com `prox == NULL`; há exatamente `tamanho` nós alcançáveis; sem ciclos.
- Testes: `teste_remover_cabeca_meio_cauda` (remoção do meio, do fim e do início), `teste_remover_inicio` (chave duplicada remove só a primeira ocorrência; chave ausente não altera a lista; esvaziar e reinserir), `teste_generico_musicas`, `teste_argumentos_invalidos` (lista `NULL`, lista vazia, `comparar == NULL`).

### `void *lse_remover_inicio(t_lse *lista)`
- Pré-condições: `lista` criada por `lse_criar` (ou `NULL`).
- Pós-condições: em sucesso, `inicio` passa a apontar para o segundo nó (ou `NULL`, se só havia um) e `tamanho` diminui em 1. Em lista vazia ou `NULL`, nada é alterado.
- Retorno: a carga do primeiro nó; `NULL` se a lista estiver vazia ou for `NULL`.
- Memória: a LSE libera somente o nó removido. A carga é devolvida a quem chamou, que passa a ser o responsável por ela.
- Invariantes preservadas: as mesmas de `lse_remover`.
- Testes: `teste_remover_inicio` (lista vazia, lista com elementos, esvaziar por completo), `teste_argumentos_invalidos` (`lse_remover_inicio(NULL) == NULL`).

### `int lse_tamanho(const t_lse *lista)`
- A decisão da M02 continua valendo: `0` quando `lista == NULL`. O código foi alinhado com este contrato na M04 (antes devolvia `-1`).

### Observações
- **Ambiguidade do `NULL`:** como a carga `NULL` é aceita (M03), um retorno `NULL` em `lse_buscar`, `lse_remover` e `lse_remover_inicio` pode significar "não achou / lista vazia" ou "achou uma carga `NULL`". Por isso os testes da M04 não usam carga `NULL`; quem precisar distinguir os casos pode conferir `lse_tamanho` antes. Uma API futura poderia devolver `int` (achou / não achou) e entregar a carga por um parâmetro de saída (`void **`).
- **Nome do tipo do comparador:** o enunciado da M04 chama o tipo de `t_comparador_lse`; o `lse.h` do projeto usa `t_comparador`. Mantivemos `t_comparador` e não alteramos o cabeçalho.
- **Responsabilidade pela carga:** a LSE nunca libera a carga de um nó removido por `lse_remover` ou `lse_remover_inicio`; ela só libera cargas em `lse_destruir`, quando recebe uma função `destruir_carga`.