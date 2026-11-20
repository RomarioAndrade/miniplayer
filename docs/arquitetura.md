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
Remoção do meio (`b`) em `a -> b -> c`: só o `prox` do anterior muda.

```
antes:  inicio -> [a] -> [b] -> [c] -> NULL
depois: inicio -> [a] ---------> [c] -> NULL      (nó de b liberado, carga &b devolvida)
```

- **Inserção no fim:** percorre até o último nó (O(n)); sem ponteiro `fim` até a M08. Lista vazia: o novo nó vira `inicio`.
- **Remoção do início:** `inicio` passa a ser `inicio->prox`.
- **Remoção do meio/fim:** o anterior passa a apontar para o `prox` do removido.
- **Busca e remoção por chave:** usam o callback `t_comparador`; a LSE não conhece `t_musica` (músicas são comparadas por `id` fora de `lse.c`).
- **Memória:** remover libera só o nó; a carga volta para quem chamou, que decide se libera.
