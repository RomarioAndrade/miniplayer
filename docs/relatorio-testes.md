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