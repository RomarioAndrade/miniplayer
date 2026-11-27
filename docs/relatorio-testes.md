# Relatório de testes

## M01 — Música
| ID | Missão | Função/contrato | Entrada/cenário | Resultado esperado | Resultado obtido | Status |
|---|---|---|---|---|---|---|
| T01 | M01 | musica_criar | dados válidos: `1, "wasurerarenai", "sakanaction", 4` | ponteiro não nulo e campos corretos | ponteiro válido; `id`, `titulo` e `duracao` conferidos pelos `assert` | aprovado |
| T01b | M01 | musica_imprimir | música criada no T01 | mostrar título, artista e duração | `Musica: wasurerarenai - sakanaction , duração: 4` | aprovado |
| T01c | M01 | musica_destruir | música criada no T01 | memória liberada, sem vazamento | AddressSanitizer não acusou nada | aprovado |

### Execução
```
$ make test
gcc -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude tests/test_musica.c src/musica.c -o build/test_musica
== build/test_musica ==
Musica: wasurerarenai - sakanaction , duração: 4
```
Compilou sem nenhum aviso.

### Memória (M01)
Rodamos o mesmo teste com o AddressSanitizer:
```
$ gcc -std=c11 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -Iinclude tests/test_musica.c src/musica.c -o build/test_musica_asan
$ ./build/test_musica_asan
Musica: wasurerarenai - sakanaction , duração: 4
```
Nenhum vazamento nem acesso inválido.

### O que ainda não foi testado
- Falha de `malloc` na `musica_criar`: hoje o código não confere se o `malloc` devolveu `NULL`.
- Título ou artista com mais de 79 caracteres: a `musica_criar` usa `strcpy`, que estoura o vetor de 80 posições nesse caso.
- `musica_imprimir(NULL)`: o código trata (não imprime nada), mas o teste não cobre.
- `musica_destruir(NULL)`: para o programa no `assert`.

## M02 — Lista vazia
| ID | Missão | Função/contrato | Entrada/cenário | Resultado esperado | Resultado obtido | Status |
|---|---|---|---|---|---|---|
| T02 | M02 | lse_criar | criar uma lista nova | ponteiro não nulo | com o stub do kit voltava `NULL` e o teste parava; com a nossa implementação devolve a lista | aprovado |
| T03 | M02 | lse_tamanho | lista recém-criada | 0 | 0 | aprovado |
| T04 | M02 | lse_primeiro | lista vazia | `NULL` | `NULL` | aprovado |
| T05 | M02 | lse_destruir | lista vazia, passando `NULL` como função de destruir carga | lista liberada, sem vazamento | antes vazavam 16 bytes; depois de implementar, nenhum vazamento | aprovado |

### Antes da implementação
Primeiro rodamos o teste da M02 com o stub original do kit, só pra ver ele falhar:
```
m02_kit: missoes/M02/TESTE_INCREMENTAL_M02.c:8: teste_lista_vazia: Assertion `l != NULL && "lse_criar deve retornar uma lista valida"' failed.
Abortado (exit 134)
```
Com a versão que a gente já tinha começado, o teste passava, mas não estava certo de verdade: o gcc dava um aviso na `lse_tamanho` (faltava `return`) porque a `lse_destruir` ainda não liberava nada.

### Depois da implementação
```
$ make -f missoes/M02/Makefile.incremental test_m02
cc -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude missoes/M02/TESTE_INCREMENTAL_M02.c src/lse.c -o build/m02_incremental
./build/m02_incremental
M02: teste de contrato passou.
```

### Memória (M02)
- Pra procurar vazamento usamos o AddressSanitizer, porque o valgrind não está instalado na máquina:
  `cc -std=c11 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -Iinclude missoes/M02/TESTE_INCREMENTAL_M02.c src/lse.c`
  Com o código final ele não reclamou de nada.
- Também rodamos o `test_lse.c` antigo (naquele momento ainda com o trecho de inserção comentado) e deu `test_lse: OK`. O `make test` da raiz continuou funcionando.


## M03 — Construção e percurso da lista
| ID | Missão | Função/contrato | Entrada/cenário | Resultado esperado | Resultado obtido | Status |
|---|---|---|---|---|---|---|
| T06 | M03 | lse_tamanho, lse_primeiro, lse_percorrer | lista vazia | tamanho 0, primeiro `NULL`, percurso sem visitas | igual ao esperado | aprovado |
| T07 | M03 | lse_inserir_inicio, lse_percorrer | inserir 1 elemento | tamanho 1, primeiro correto, 1 visita | igual ao esperado | aprovado |
| T08 | M03 | lse_inserir_inicio, lse_percorrer | inserir 3 elementos (a, b, c) | percurso na ordem c, b, a (LIFO na cabeça); nós alcançáveis = tamanho a cada inserção | igual ao esperado | aprovado |
| T09 | M03 | lse_inserir_inicio | cargas de tipos diferentes (`int`, `struct`, string) | lista guarda e devolve os ponteiros sem conhecer o tipo | igual ao esperado | aprovado |
| T10 | M03 | lse_inserir_inicio | carga `NULL` | devolve 1, tamanho 1, callback chamado 1 vez | igual ao esperado | aprovado |
| T11 | M03 | lse_destruir | 3 cargas alocadas no heap, com callback destrutor | callback chamado 3 vezes, sem vazamento | igual ao esperado | aprovado |
| T12 | M03 | lse_inserir_inicio / lse_percorrer / lse_destruir | `lista == NULL` (e `cb_visitar == NULL`) | inserir devolve 0; percorrer e destruir não fazem nada nem quebram | igual ao esperado | aprovado |

### Execução
```
$ make -f missoes/M03/Makefile.incremental test_m03
gcc -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude missoes/M03/test_m03.c src/lse.c -o build/test_m03_incremental
./build/test_m03_incremental
M03: todos os testes passaram.
```

### Memória (M03)
- Compilador: gcc 15.2.0 (Ubuntu, via WSL). Ferramenta: AddressSanitizer e UBSan.
```
$ gcc -std=c11 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -Iinclude missoes/M03/test_m03.c src/lse.c -o build/m03_asan
$ ./build/m03_asan
M03: todos os testes passaram.
```
Nenhum vazamento nem acesso inválido foi reportado.

### Casos-limite cobertos
- Estrutura vazia: T06.
- Estrutura com um elemento: T07.
- Carga `NULL`: T10.
- Entradas `NULL`: T12.
- Falha de alocação: não simulada nesta missão; no código, `lse_inserir_inicio` devolve 0 sem alterar a lista.

### Problemas encontrados
- O enunciado usa `t_visitar` no protótipo e declara `t_lse_visitar`; usamos `t_lse_visitar`.
- A primeira versão rejeitava carga `NULL`, contra o enunciado; corrigido.
- Asteriscos perdidos ao copiar o código causaram erro de sintaxe; corrigido.

## M04 — Inserção no fim, busca e remoção
| ID | Missão | Função/contrato | Entrada/cenário | Resultado esperado | Resultado obtido | Status |
|---|---|---|---|---|---|---|
| T13 | M04 | lse_inserir_fim, lse_buscar | `teste_inserir_fim_e_buscar`: inserir `a`, `b`, `c` em lista vazia; buscar `a` (início), `b` (meio), `c` (fim) e `99` (ausente) | ordem a, b, c; busca devolve a própria carga; `NULL` para ausente; a busca não altera a lista | igual ao esperado | aprovado |
| T14 | M04 | lse_remover | `teste_remover_cabeca_meio_cauda`: lista `a,b,c,d`; remover `b` (meio), `d` (fim) e `a` (início) | devolve a carga removida; cadeia religada; lista final só com `c`, tamanho 1 | antes devolvia `NULL` (stub); depois da implementação, igual ao esperado | aprovado |
| T15 | M04 | lse_remover_inicio | `teste_remover_inicio`: lista vazia, depois lista `a,b,c` | `NULL` na lista vazia; cargas devolvidas na ordem `a`, `b`; tamanho diminui a cada remoção | igual ao esperado | aprovado |
| T16 | M04 | lse_remover | chave repetida: `a` e `b` valem 5; lista `c,a,b` | remove só a primeira ocorrência (`a`); `b` continua na lista | igual ao esperado | aprovado |
| T17 | M04 | lse_remover | chave ausente (99) | devolve `NULL`; lista e tamanho não mudam | igual ao esperado | aprovado |
| T18 | M04 | lse_remover_inicio, lse_inserir_fim | esvaziar a lista com `lse_remover_inicio` e inserir de novo | lista vazia (tamanho 0) devolve `NULL`; depois de reinserir, tamanho 1 e carga correta | igual ao esperado | aprovado |
| T19 | M04 | lse_buscar, lse_remover | `teste_generico_musicas`: registros `musica_t` com `id`, comparador por `id` | acha e remove a música 22 e mantém a 11 | igual ao esperado | aprovado |
| T20 | M04 | todas | `teste_argumentos_invalidos`: lista `NULL`, lista vazia, comparador `NULL` | `lse_inserir_fim(NULL,…) == 0`; `NULL` nas demais; nada quebra; a lista continua íntegra | antes falhava: `lse_inserir_fim(NULL,&x)` devolvia `-1`; depois da correção, igual ao esperado | aprovado |
| T21 | M04 | lse_remover | remover por chave o único elemento da lista | devolve a carga; lista vazia (tamanho 0) | igual ao esperado | aprovado |

### Antes da implementação (19/11)
Com `lse_remover` ainda como stub, o teste da M04 parava na primeira remoção (a regressão da M02 e da M03 passava):
```
$ make test_missoes
./build/m02_incremental
M02: teste de contrato passou.
./build/test_m03_incremental
M03: todos os testes passaram.
./build/test_m04_incremental
test_m04_incremental: missoes/M04/test_m04.c:45: teste_remover_cabeca_meio_cauda: Assertion `lse_remover(l,&b,comparar_int)==&b' failed.
make: *** [missoes/M04/Makefile.incremental:10: test_m04] Abortado
```

### Depois de implementar `lse_remover` (19/11)
Com `lse_remover` pronta, o teste avançou e parou em outro defeito, no `lse_inserir_fim` (devolvia `-1` para lista `NULL`, e o contrato e o teste do professor esperam `0`):
```
$ make -f missoes/M04/Makefile.incremental test_m04
gcc -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude missoes/M04/test_m04.c src/lse.c -o build/test_m04_incremental
./build/test_m04_incremental
test_m04_incremental: missoes/M04/test_m04.c:80: teste_argumentos_invalidos: Assertion `lse_inserir_fim(NULL,&x)==0' failed.
make: *** [missoes/M04/Makefile.incremental:10: test_m04] Abortado
```
Nessa execução, o compilador ainda avisava `unused variable` (`a`, `b`, `c`, `ausente`) nas duas funções de teste que não tinham sido preenchidas. Esses avisos desapareceram quando os testes foram completados.

### Depois das correções e dos testes completos (20/11)
Correções no código: `lse_inserir_fim` passou a devolver `0` para lista `NULL` e a conferir o `malloc` antes de usar o nó; `lse_tamanho(NULL)` passou a devolver `0` (como diz o contrato); o comentário `/* TODO M04` de `lse_remover_inicio` foi fechado (tirava o aviso `-Wcomment`).
```
$ make -f missoes/M04/Makefile.incremental test_m04; echo "codigo: $?"
gcc -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude missoes/M04/test_m04.c src/lse.c -o build/test_m04_incremental
./build/test_m04_incremental
M04: todos os testes passaram.
codigo: 0
```
Compilou sem nenhum aviso.

### Regressão (M02 + M03 + M04)
```
$ make test_missoes; echo "codigo: $?"
./build/m02_incremental
M02: teste de contrato passou.
./build/test_m03_incremental
M03: todos os testes passaram.
./build/test_m04_incremental
M04: todos os testes passaram.
codigo: 0
```
M02 e M03 continuam aprovadas depois da M04, sem regressão.

### Memória (M04)
- Compilador: gcc 15.2.0 (Ubuntu, via WSL). Ferramenta: AddressSanitizer e UBSan.
```
$ gcc -std=c11 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -Iinclude missoes/M04/test_m04.c src/lse.c -o build/test_m04_asan
$ ./build/test_m04_asan; echo "codigo: $?"
M04: todos os testes passaram.
codigo: 0
```
Nenhum vazamento nem acesso inválido foi reportado.
- Regressão da M03 com os mesmos detectores:
```
$ gcc -std=c11 -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined -Iinclude missoes/M03/test_m03.c src/lse.c -o build/test_m03_asan
$ ./build/test_m03_asan; echo "codigo: $?"
M03: todos os testes passaram.
codigo: 0
```
- Os testes da M04 usam variáveis automáticas como carga e passam `NULL` em `lse_destruir`, então nada é liberado indevidamente. Os nós removidos são liberados por `lse_remover` e `lse_remover_inicio`, e o ASan não acusou vazamento.

### Casos-limite cobertos
- Estrutura vazia: T15, T18 e T20 (busca, remoção e remoção do início em lista vazia).
- Estrutura com um elemento: T13 (primeira inserção), T14 (lista final com um nó) e T21 (remoção por chave do único elemento).
- Remoção no início, no meio e no fim: T14 e T15.
- Chave repetida: T16. Chave ausente: T17.
- Esvaziar e reutilizar a lista: T18.
- Tipo de carga diferente de `int` (registro de música): T19.
- Entradas `NULL` (lista e comparador): T20.

### O que ainda não foi testado
- Falha de `malloc` em `lse_inserir_fim`: não simulada. No código, a função confere o `malloc` e devolve `0` sem alterar a lista.
- Carga `NULL` em busca e remoção: os testes da M04 não usam carga `NULL`, porque o retorno `NULL` fica ambíguo ("não achou" ou "achou carga `NULL`"). A limitação está registrada em `docs/contratos.md`.
- `lse_tamanho(NULL) == 0`: o código já está alinhado com o contrato, mas os testes da M04 não têm uma asserção específica para esse caso.


## M05 — Inserção ordenada
| ID | Missão | Função/contrato | Entrada/cenário | Resultado esperado | Resultado obtido | Status |
|---|---|---|---|---|---|---|
| T22 | M05 | lse_inserir_ordenado | `teste_vazia_inicio_meio_fim`: inserir 20 (lista vazia), 10 (início), 30 (fim), 15 (meio), depois 5 (novo início) e 40 (novo fim) | lista sempre ordenada: 5, 10, 15, 20, 30, 40; `tamanho` correto a cada inserção; ordem conferida ponteiro a ponteiro | o kit trazia `assert(1==0)`; depois do teste completo, igual ao esperado | aprovado |
| T23 | M05 | lse_inserir_ordenado | `teste_duplicados_estaveis`: registros `musica_t` com `id` 10 (primeira), 10 (segunda), 5, 10 (terceira) e 20 | o novo elemento entra depois dos que têm a mesma chave: ordem final 5, primeira, segunda, terceira, 20; `lse_primeiro` devolve o `id` 5 | igual ao esperado | aprovado |
| T24 | M05 | lse_inserir_ordenado | `teste_invalido_e_reinsercao`: lista `NULL`, comparador `NULL` (em lista vazia e não vazia) | devolve `0`; a lista e o `tamanho` não mudam | igual ao esperado | aprovado |
| T25 | M05 | lse_inserir_ordenado | reinserir a mesma carga | cria um novo nó, depois do igual já existente; `tamanho` aumenta 1 | igual ao esperado | aprovado |
| T26 | M05 | lse_inserir_ordenado, lse_remover | remover um elemento com `lse_remover` e inseri-lo de novo | a lista volta à posição ordenada correta | igual ao esperado | aprovado |

### Antes da implementação (teste do kit)
O arquivo `test_m05.c` entregue pelo professor traz três `assert(1==0)` propositais. Rodando-o contra o `lse.c`, ele para no primeiro:
```
$ ./build/test_m05_incremental
test_m05_incremental: missoes/M05/test_m05.c:20: teste_vazia_inicio_meio_fim: Assertion `1==0' failed.
codigo: 134
```
O compilador também avisava `unused variable` (`a`, `b`, `c`, `d`) em `teste_duplicados_estaveis` e `unused function` para `comparar_int` e `comparar_musica`. Esses avisos desapareceram quando os testes foram completados.

### Depois dos testes completos (24/11)
```
$ make test_m05; echo "codigo: $?"
gcc -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude missoes/M05/test_m05.c src/lse.c -o build/test_m05_incremental
./build/test_m05_incremental
M05: todos os testes passaram.
codigo: 0
```
Compilou sem nenhum aviso.

### Regressão (M02 + M03 + M04 + M05)
```
$ make test_missoes; echo "codigo: $?"
./build/m02_incremental
M02: teste de contrato passou.
./build/test_m03_incremental
M03: todos os testes passaram.
./build/test_m04_incremental
M04: todos os testes passaram.
./build/test_m05_incremental
M05: todos os testes passaram.
codigo: 0
```
M02, M03 e M04 continuam aprovadas depois da M05, sem regressão.

### Memória (M05)
- Compilador: gcc 15.2.0 (Ubuntu, via WSL). Ferramenta: AddressSanitizer e UBSan (o ASan inclui o detector de vazamentos, LeakSanitizer).
```
$ gcc -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude -fsanitize=address,undefined missoes/M05/test_m05.c src/lse.c -o build/test_m05_asan
$ ./build/test_m05_asan; echo "codigo: $?"
M05: todos os testes passaram.
codigo: 0
```
Nenhum vazamento nem acesso inválido foi reportado. O valgrind não está instalado neste ambiente, e a verificação de vazamentos foi feita com o LeakSanitizer.
- Os testes usam variáveis automáticas como carga e passam `NULL` em `lse_destruir`, então nenhuma carga é liberada indevidamente. A lista só aloca e libera os nós.

### Casos-limite cobertos
- Lista vazia: T22 (primeira inserção) e T24 (argumentos inválidos).
- Inserção no início, no meio e no fim: T22.
- Valores iguais (empates estáveis): T23.
- Mesma carga inserida duas vezes: T25.
- Tipo de carga diferente de `int` (registro de música): T23.
- Entradas `NULL` (lista e comparador) sem alterar a lista: T24.
- Integração com a M04 (remover e reinserir): T26.




## M06 — Pilha por composição
| ID | Missão | Função/contrato | Entrada/cenário | Resultado esperado | Resultado obtido | Status |
|---|---|---|---|---|---|---|
| T27 | M06 | todas | `test_m06.c` (kit): LIFO, topo não destrutivo, pilhas independentes, destruição com e sem callback, pilha `NULL` | conforme os contratos do enunciado | igual ao esperado | aprovado |
| T28 | M06 | empilhar, desempilhar, topo | `teste_cenario_enunciado` (próprio): empilhar A, B, C; topo; desempilhar; empilhar D | topo C, sai C, topo D; ordem restante D, B, A; depois `NULL` | igual ao esperado | aprovado |
| T29 | M06 | empilhar, desempilhar, topo | `teste_intercalacao_com_modelo` (próprio): 15 operações intercaladas, conferidas contra um vetor-modelo, com desempilhar em pilha vazia no meio | topo e carga desempilhada iguais ao modelo a cada passo; `NULL` na pilha vazia | igual ao esperado | aprovado |

### Execução e regressão (27/11)
```
$ make test
== build/test_pilha ==
test_pilha: OK
$ make test_missoes
M02: teste de contrato passou.
M03: todos os testes passaram.
M04: todos os testes passaram.
M05: todos os testes passaram.
M06: todos os testes passaram.
```
Compilou sem avisos. Com AddressSanitizer e UBSan (`test_m06.c` e `tests/test_pilha.c`), nenhum vazamento nem acesso inválido.

Para conferir que o teste próprio detecta erro, trocamos `lse_inserir_inicio` por `lse_inserir_fim` numa cópia da pilha: o teste falhou, como esperado.
