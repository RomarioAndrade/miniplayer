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
- Estrutura com um elemento: T13 (primeira inserção) e T14 (última remoção, lista final com um nó).
- Remoção no início, no meio e no fim: T14 e T15.
- Chave repetida: T16. Chave ausente: T17.
- Esvaziar e reutilizar a lista: T18.
- Tipo de carga diferente de `int` (registro de música): T19.
- Entradas `NULL` (lista e comparador): T20.

### O que ainda não foi testado
- Falha de `malloc` em `lse_inserir_fim`: não simulada. No código, a função confere o `malloc` e devolve `0` sem alterar a lista.
- Carga `NULL` em busca e remoção: os testes da M04 não usam carga `NULL`, porque o retorno `NULL` fica ambíguo ("não achou" ou "achou carga `NULL`"). A limitação está registrada em `docs/contratos.md`.
- `lse_tamanho(NULL) == 0`: o código já está alinhado com o contrato, mas os testes da M04 não têm uma asserção específica para esse caso.

### Problemas encontrados
- `lse_inserir_fim` devolvia `-1` para lista `NULL` (contrato e teste esperam `0`) e usava o nó antes de conferir o `malloc`; corrigido.
- `lse_tamanho(NULL)` devolvia `-1`, contra o contrato da M02 (`0`); corrigido.
- Comentário `/* TODO M04` sem fechar em `lse_remover_inicio`, que gerava o aviso `-Wcomment`; corrigido.
- Erro `redefinition of 'lse_remover'` por causa do stub antigo que ficou no arquivo; o stub foi removido.
- A regra do `test_m04` no `Makefile` principal usava variáveis indefinidas; o `Makefile` passou a incluir automaticamente `missoes/M*/Makefile.incremental`.