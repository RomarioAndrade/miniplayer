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

## Decisões técnicas (preencher)
- Por que `void *`?
- Quem é dono das cargas?
- Como tratar referências compartilhadas?
- Quando adicionar ponteiro `fim`?

## M02 — Decisões de memória e encapsulamento
- **Representação opaca:** `t_elem` e os campos de `struct lse` ficam somente em `src/lse.c`; `include/lse.h` expõe apenas `typedef struct lse t_lse`. Os módulos clientes (pilha, fila, miniplayer) não acessam nós diretamente.
- **Por que `void *carga`:** a LSE é genérica e não conhece o tipo armazenado (`t_musica`, inteiros nos testes etc.). Ela só guarda e devolve o ponteiro.
- **Dono das cargas:** quem insere é dono da carga. A LSE é dona apenas dos nós `t_elem` e do cabeçalho `t_lse`. Por isso `lse_destruir` recebe uma função opcional `destruir_carga`: `NULL` significa "não liberar as cargas" (referências compartilhadas ou dados de terceiros).
- **Falha de alocação:** `lse_criar` devolve `NULL` em vez de acessar memória inválida; o chamador decide como reagir.
- **Entradas `NULL`:** `lse_tamanho`, `lse_primeiro` e `lse_destruir` aceitam `NULL` sem falhar (programação defensiva).
- **Ponteiro `fim`:** continua não usado; previsto como otimização opcional na M08.
