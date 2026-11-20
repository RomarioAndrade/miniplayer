# Arquitetura
## Componentes
| Módulo | Responsabilidade | Dependências |
|---|---|---|
| musica | Dados da música | biblioteca padrão |
| lse | Nós genéricos e encadeamento | biblioteca padrão |
| pilha | Política LIFO | lse |
| fila | Política FIFO | lse |
| miniplayer | Simulação e integração | demais TADs |

## Diagrama de dependências
MiniPlayer → (LSE, Pilha, Fila) → LSE → t_elem interno.

## Decisões técnicas
- **Por que `void *`?** Assim a mesma LSE guarda qualquer tipo (músicas, inteiros nos testes) sem conhecer `t_musica`. Comparar e destruir cargas fica a cargo de callbacks.
- **Quem é dono das cargas?** Quem as criou. A lista é dona só dos nós: remover devolve a carga sem liberá-la, e `lse_destruir` só libera as cargas se receber uma função destruidora.
- **Como tratar referências compartilhadas?** A biblioteca é dona das músicas; fila e histórico só guardam ponteiros para elas e são destruídos com `NULL`. Antes de liberar uma música, tirar as referências a ela da fila e do histórico.
- **Quando adicionar ponteiro `fim`?** Quando inserir no fim precisar ser O(1), por exemplo ao enfileirar na fila. Previsto para a M08; até lá, `lse_inserir_fim` percorre a lista (O(n)).

## M02 — Decisões de memória e encapsulamento
- **Representação opaca:** `t_elem` e os campos de `struct lse` ficam somente em `src/lse.c`; `include/lse.h` expõe apenas `typedef struct lse t_lse`. Os módulos clientes (pilha, fila, miniplayer) não acessam nós diretamente.
- **Por que `void *carga`:** a LSE é genérica e não conhece o tipo armazenado (`t_musica`, inteiros nos testes etc.). Ela só guarda e devolve o ponteiro.
- **Dono das cargas:** a lista não sabe o tipo das cargas; quem chama `lse_destruir` passa a função que as destrói (ou `NULL` para não tocar nelas).
- **Falha de alocação:** `lse_criar` devolve `NULL` em vez de acessar memória inválida; o chamador decide como reagir.
- **Entradas `NULL`:** `lse_tamanho`, `lse_primeiro` e `lse_destruir` aceitam `NULL` sem falhar (programação defensiva).
- **Ponteiro `fim`:** continua não usado; previsto como otimização opcional na M08.


## M03 — Encadeamento e inserção no início
Desenho após inserir `&a`, depois `&b`, depois `&c` (o último inserido fica na frente):

```
lista -> inicio -> [carga=&c | prox] -> [carga=&b | prox] -> [carga=&a | prox=NULL]
         tamanho = 3
```

- **Como a inserção funciona:** `lse_inserir_inicio` aloca um `t_elem`, guarda a carga, faz o `prox` do novo nó apontar para o antigo `inicio` e só depois atualiza `inicio` para o novo nó. Essa ordem importa: se `inicio` fosse trocado antes, a lista antiga seria perdida.
- **Percurso:** `lse_percorrer` anda de `inicio` até `NULL` e chama o callback com a carga de cada nó, sem expor `t_elem` ao chamador.
- **Quem libera o nó:** a própria LSE, dentro de `lse_destruir`.
- **Quem libera a carga:** quem usa a lista. A LSE só chama a função `destruir_carga` recebida; com `NULL`, as cargas não são tocadas (por exemplo, variáveis automáticas na pilha).
- **Carga `NULL`:** aceita. O nó existe e conta no `tamanho`.
- **Falha de alocação:** `lse_inserir_inicio` devolve `0` e a lista fica como estava.
- **Encapsulamento:** `t_elem` continua somente em `src/lse.c`; nenhum módulo cliente acessa os nós diretamente.

## M04 — Inserção no fim, busca e remoção

Nesta missão a LSE continua sem ponteiro `fim`. Por isso inserir no fim, buscar e remover por chave percorrem a cadeia (O(n)); remover do início é O(1). Nos desenhos, `[x]` é um nó e `NULL` é o fim da cadeia.

### Inserção no fim
`lse_inserir_fim` aloca o nó, confere o `malloc`, guarda a carga e coloca `prox = NULL`. Depois liga o nó ao final:

```
lista vazia:     inicio -> NULL                      tamanho = 0
inserir a:       inicio -> [a] -> NULL               tamanho = 1   (inicio mudou)
inserir b:       inicio -> [a] -> [b] -> NULL        tamanho = 2   (prox do último mudou)
inserir c:       inicio -> [a] -> [b] -> [c] -> NULL tamanho = 3   (prox do último mudou)
```

- Lista vazia: o único ponteiro que muda é `inicio`.
- Lista com elementos: anda até o último nó (aquele com `prox == NULL`) e só o `prox` dele muda.
- Se o `malloc` falhar, nada é ligado e a lista fica como estava.

### Busca
`lse_buscar` anda de `inicio` até `NULL` chamando `comparar(carga, chave)` em cada nó e devolve a **primeira** carga que der `0`. Nenhum ponteiro muda. Se terminar sem achar, devolve `NULL`.

### Remoção: o que muda em cada caso

**Lista vazia:** `inicio` já é `NULL`; nada a remover, devolve `NULL` e nada muda.

**Lista com um nó** (`inicio -> [a] -> NULL`), removendo `a`:

```
antes:   inicio -> [a] -> NULL        tamanho = 1
depois:  inicio -> NULL               tamanho = 0     (nó de a liberado, carga &a devolvida)
```
Muda `inicio` (passa a ser `a->prox`, que é `NULL`) e `tamanho`.

**Lista com três nós** (`inicio -> [a] -> [b] -> [c] -> NULL`):

```
remover a (início):
  antes:   inicio -> [a] -> [b] -> [c] -> NULL
  depois:  inicio -------> [b] -> [c] -> NULL       muda: inicio

remover b (meio):
  antes:   inicio -> [a] -> [b] -> [c] -> NULL
  depois:  inicio -> [a] --------> [c] -> NULL      muda: prox de a

remover c (fim):
  antes:   inicio -> [a] -> [b] -> [c] -> NULL
  depois:  inicio -> [a] -> [b] -> NULL             muda: prox de b (passa a ser NULL)
```

Resumo dos ponteiros alterados:

| Caso | Ponteiro alterado | Novo valor |
|---|---|---|
| Remover o primeiro nó | `inicio` | `primeiro->prox` |
| Remover nó do meio | `anterior->prox` | `removido->prox` |
| Remover o último nó | `anterior->prox` | `NULL` |
| Remover o único nó | `inicio` | `NULL` (e `tamanho` volta a 0) |

Em todos os casos o `tamanho` diminui em 1 e o nó removido é liberado.

### Ordem das operações na remoção
1. Percorrer a lista guardando o nó `atual` e o `anterior`.
2. Ao achar a chave, **religar a cadeia primeiro** (`inicio` ou `anterior->prox`).
3. **Guardar a carga** do nó numa variável local.
4. Só então liberar o nó (`free`) e diminuir `tamanho`.
5. Devolver a carga guardada.

Essa ordem importa: depois do `free` não se pode mais ler `atual->carga` nem `atual->prox`, senão haveria acesso a memória já liberada.

### Remoção do início
`lse_remover_inicio` não precisa de busca: guarda o primeiro nó, move `inicio` para `inicio->prox`, guarda a carga, libera o nó e devolve a carga (O(1)). Lista vazia ou `NULL`: devolve `NULL`.

### Chave duplicada
`lse_buscar` e `lse_remover` agem só sobre a **primeira** ocorrência. Com `a` e `b` valendo 5 em `a -> b -> c`, remover a chave 5 retira `a` e mantém `b`.

### Memória e invariantes
- **Memória:** remover libera só o nó; a carga volta para quem chamou, que decide se libera. `lse_destruir` só libera cargas se receber uma função destruidora.
- **Callback `t_comparador`:** a LSE não conhece `t_musica`; as músicas são comparadas por `id` em funções fora de `lse.c`.
- **Invariantes preservadas:** `tamanho` sempre igual ao número de nós alcançáveis; `tamanho == 0` só quando `inicio == NULL`; o último nó aponta para `NULL`; sem ciclos.
- **Limitação:** sem ponteiro `fim`, inserir no fim custa O(n). O ponteiro `fim` fica para a M08.