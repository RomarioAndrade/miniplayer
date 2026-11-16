# Relatório de testes
| ID | Missão | Função/contrato | Entrada/cenário | Resultado esperado | Resultado obtido | Status |
|---|---|---|---|---|---|---|
| T01 | M01 | musica_criar | dados válidos | ponteiro não nulo e campos corretos | preencher | pendente |

## Casos-limite
- Estrutura vazia:
- Estrutura com um elemento:
- Chave inexistente:
- Falha de alocação (discutir estratégia de simulação):

## Memória
Registrar comandos de compilação, ferramentas utilizadas e problemas corrigidos.

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
