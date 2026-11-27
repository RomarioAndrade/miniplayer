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

## M05 — Inserção ordenada

Nesta missão a LSE continua sem ponteiro `fim`. `lse_inserir_ordenado` percorre a cadeia com dois ponteiros auxiliares até achar a posição do novo nó. Nos desenhos, `[x]` é um nó e `NULL` é o fim da cadeia.

### Os três ponteiros
- `novo`: o nó recém-criado com `malloc`, que guarda a carga nova.
- `atual`: começa em `inicio` e avança enquanto `comparar(atual->carga, carga_nova) <= 0`.
- `anterior`: o nó logo antes de `atual`. Começa em `NULL`, porque antes do primeiro nó não há ninguém.

Quando o laço para, `novo` é encaixado entre `anterior` e `atual`.

### Pseudocódigo
```
lse_inserir_ordenado(lista, carga, comparar):
    se lista == NULL ou comparar == NULL:
        devolver 0
    novo = alocar nó
    se alocação falhou:
        devolver 0                      (a lista não foi tocada)
    novo.carga = carga
    anterior = NULL
    atual = lista.inicio
    enquanto atual != NULL e comparar(atual.carga, carga) <= 0:
        anterior = atual
        atual = atual.prox
    novo.prox = atual
    se anterior == NULL:
        lista.inicio = novo             (início ou lista vazia)
    senão:
        anterior.prox = novo            (meio ou fim)
    lista.tamanho = lista.tamanho + 1
    devolver 1
```

### Os quatro casos
Em todos os desenhos, inserimos a chave 20.

**1) Lista vazia**
```
antes:   inicio -> NULL                anterior = NULL, atual = NULL
depois:  inicio -> [20] -> NULL        tamanho = 1
```
O laço nem executa. Ligações: `novo->prox = atual (NULL)` e `inicio = novo`, porque `anterior == NULL`.

**2) Início** (lista `30 -> 40`)
```
antes:   inicio -> [30] -> [40] -> NULL
                    ^
                  atual              anterior = NULL
         comparar(30, 20) > 0  => o laço para sem avançar

depois:  inicio -> [20] -> [30] -> [40] -> NULL
```
Ligações: `novo->prox = atual` e `inicio = novo`, porque `anterior == NULL`.

**3) Meio** (lista `10 -> 30 -> 40`)
```
passo 1: inicio -> [10] -> [30] -> [40] -> NULL
                    ^
                  atual              anterior = NULL
         comparar(10, 20) < 0  => avança

passo 2: inicio -> [10] -> [30] -> [40] -> NULL
                    ^        ^
                anterior   atual
         comparar(30, 20) > 0  => o laço para

depois:  inicio -> [10] -> [20] -> [30] -> [40] -> NULL
```
Ligações: `novo->prox = atual` e `anterior->prox = novo`.

**4) Fim** (lista `10 -> 15`)
```
antes:   inicio -> [10] -> [15] -> NULL
                              ^       ^
                          anterior   atual = NULL

depois:  inicio -> [10] -> [15] -> [20] -> NULL
```
Ligações: `novo->prox = atual (NULL)` e `anterior->prox = novo`.

### Resumo dos ponteiros alterados
| Caso | Ponteiros alterados | Novos valores |
|---|---|---|
| Lista vazia | `novo->prox`, `inicio` | `NULL`, `novo` |
| Início | `novo->prox`, `inicio` | antigo primeiro, `novo` |
| Meio | `novo->prox`, `anterior->prox` | `atual`, `novo` |
| Fim | `novo->prox`, `anterior->prox` | `NULL`, `novo` |

Em todos os casos `tamanho` aumenta em 1.

### Ordem das ligações
1. Primeiro `novo->prox = atual`.
2. Depois `inicio = novo` ou `anterior->prox = novo`.

Essa ordem importa: se `anterior->prox` fosse alterado antes, o resto da lista (`atual` em diante) ficaria inacessível.

### Empates (estabilidade)
O laço usa `<= 0`, então ele continua andando enquanto a carga existente for menor **ou igual** à nova. O novo elemento fica depois de todos os que têm a mesma chave:
```
lista:   10 -> 20(a) -> 20(b) -> 30        inserir 20(novo)
comparar(20a, 20) == 0  => avança
comparar(20b, 20) == 0  => avança
comparar(30,  20)  > 0  => para
depois:  10 -> 20(a) -> 20(b) -> 20(novo) -> 30
```
Se o laço usasse `< 0`, o novo entraria **antes** dos iguais e a ordem de chegada se inverteria. O teste `teste_duplicados_estaveis` verifica essa decisão.

### Complexidade
- **Tempo:** O(n) no pior caso, quando o novo é maior ou igual a todos e o laço percorre a lista inteira (inserção no fim). No melhor caso (novo menor que o primeiro) é O(1). Inserir no início sem ordem, `lse_inserir_inicio`, é sempre O(1), porque não precisa procurar a posição.
- **Espaço:** O(1) extra: só um nó novo e dois ponteiros auxiliares (`anterior` e `atual`). A carga não é copiada.

### Memória e invariantes
- **Memória:** a função aloca apenas o nó (`t_elem`) e confere o `malloc` antes de mexer na lista. A carga não é copiada nem liberada pela lista; continua pertencendo a quem a inseriu.
- **Encapsulamento:** `t_elem` continua privado de `src/lse.c`; o cabeçalho `include/lse.h` não foi alterado e `lse.c` não depende de `t_musica`.
- **Comparador genérico:** o critério de ordem vem de fora, pelo `t_comparador`. Trocar de `id` para título ou duração exige só outra função de comparação, sem mexer em `lse.c`.
- **Invariantes preservadas:** `tamanho` igual ao número de nós alcançáveis; `tamanho == 0` só quando `inicio == NULL`; o último nó aponta para `NULL`; sem ciclos; nenhum nó inacessível.
- **Pré-condição:** a lista precisa estar ordenada pelo mesmo critério. A função não verifica nem corrige isso.

## M06 — Pilha por composição

### Relação de composição
A pilha **tem uma** lista: `struct pilha { t_lse *lista; };`, privada em `src/pilha.c`. O cabeçalho `include/pilha.h` expõe só `typedef struct pilha t_pilha`, então quem usa a pilha não enxerga a lista. A pilha não tem nós próprios: o encadeamento continua todo dentro da LSE, e `pilha.c` usa apenas a interface pública de `lse.h`, sem acessar `t_elem` nem os campos de `t_lse`.

```
t_pilha (malloc em pilha_criar)
   └── t_lse (lse_criar)
         └── nós t_elem (alocados pela LSE)
               └── carga (pertence a quem a inseriu)
```

### Composição × duplicação
Uma primeira versão de `pilha.c` redeclarava `t_elem` e montava um encadeamento próprio (`struct pilha { t_elem *topo; }`). Isso duplica código que já está testado na LSE e quebra o encapsulamento. Voltamos ao campo `t_lse *lista`, previsto no kit, para que a pilha só repasse o trabalho à lista.

### Criação e falha parcial
`pilha_criar` faz duas alocações, uma de cada vez:
1. `lse_criar()`. Se falhar, nada foi alocado ainda e a função devolve `NULL`.
2. `malloc` do `t_pilha`. Se falhar, a lista do passo 1 já existe e seria perdida, então ela é liberada com `lse_destruir(lista, NULL)` antes de devolver `NULL`.

Regra usada: em uma falha, libera-se o que **deu certo antes**, nunca o ponteiro que acabou de falhar (que é `NULL`). A lista é liberada com `lse_destruir`, e não com `free`, porque quem a criou foi a LSE e o tipo é opaco. O callback é `NULL` porque a lista ainda está vazia.

### Destruição e propriedade das cargas
`pilha_destruir(p, d)` libera primeiro a lista, com `lse_destruir(p->lista, d)`, e depois o objeto pilha, com `free(p)`. A ordem importa: depois de `free(p)` não é mais possível ler `p->lista`.

A pilha não decide sozinha o destino das cargas. Ela repassa o callback `d` sem alterá-lo:
- `d == NULL`: só nós, lista e pilha são liberados; as cargas ficam intactas. É o caso do futuro histórico, em que as músicas pertencem à biblioteca e a pilha guarda apenas referências emprestadas.
- `d != NULL`: a LSE chama `d(carga)` para cada carga ainda armazenada. Só é seguro quando a pilha é a dona exclusiva das cargas.

Com `p == NULL` a função não faz nada (`free(NULL)` é permitido em C).

### Empilhar e desempilhar
O topo é o **início** da lista: `pilha_empilhar` delega para `lse_inserir_inicio` e `pilha_desempilhar` para `lse_remover_inicio`. As duas mexem só em `inicio`, então são O(1). Usar o fim da LSE seria pior: sem ponteiro para o penúltimo nó, remover no fim exige percorrer a lista (O(n)). Desempilhar devolve a carga sem liberá-la.

### Topo
O último empilhado fica no início da lista, então o topo é a primeira carga: `pilha_topo` devolve `lse_primeiro(p->lista)`. É só consulta (a carga não é removida, por isso o `const`); devolve `NULL` com pilha vazia ou `NULL`.

### Complexidade
- `pilha_criar`: O(1) em tempo e espaço.
- `pilha_empilhar` e `pilha_desempilhar`: O(1), porque só alteram o início da lista.
- `pilha_topo`: O(1), porque `lse_primeiro` só lê o início.
- `pilha_destruir`: O(n) em tempo, com n cargas armazenadas, porque `lse_destruir` percorre todos os nós (sem contar o custo do callback); espaço extra O(1).

### Estado da pilha passo a passo (atividade 1)
O topo da pilha é o **início** da lista. Nos desenhos, a lista é lida da esquerda para a direita, e o primeiro nó é o topo. `[x]` é um nó e `NULL` é o fim da cadeia.

```
começo:            inicio -> NULL                        pilha vazia

empilhar A:        inicio -> [A] -> NULL                 topo = A
empilhar B:        inicio -> [B] -> [A] -> NULL          topo = B
empilhar C:        inicio -> [C] -> [B] -> [A] -> NULL   topo = C

consultar topo:    devolve C                             nada muda
                   inicio -> [C] -> [B] -> [A] -> NULL

desempilhar:       devolve C (o nó de C é liberado, a carga não)
                   inicio -> [B] -> [A] -> NULL          topo = B

empilhar D:        inicio -> [D] -> [B] -> [A] -> NULL   topo = D
```

O mesmo estado como pilha (o topo fica em cima):
```
depois de A, B, C:       depois de desempilhar:     depois de empilhar D:
      | C |  <- topo            | B |  <- topo            | D |  <- topo
      | B |                     | A |                     | B |
      | A |                                               | A |
```

**Ordem de saída restante:** D, B, A (o último a entrar é o primeiro a sair). Depois do A, `pilha_desempilhar` devolve `NULL`, porque a pilha está vazia.

### Ponteiros alterados em cada operação
| Operação | Ponteiros alterados | Custo |
|---|---|---|
| `pilha_empilhar` | o novo nó passa a apontar para o antigo primeiro, e `inicio` passa a apontar para o novo nó | O(1) |
| `pilha_desempilhar` | `inicio` passa a apontar para o segundo nó (e o primeiro nó é liberado) | O(1) |
| `pilha_topo` | nenhum (só lê `inicio`) | O(1) |

Esse desenho é o mesmo cenário do teste `teste_cenario_enunciado` (T28), que confere cada um desses passos.