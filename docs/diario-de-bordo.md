# Diário de bordo
## Missão M01 — 11/11
- Integrantes e papéis: João Guilherme Martins, Romário Andrade
- Objetivo: Implemtar uma função(`musica_imprimir`) que imprima na saida as informações de uma musica
- O que implementamos:`musica_imprimir`
- Decisão técnica e justificativa: deixamos a `musica_imprimir` recebendo `void *carga`, igual à `musica_destruir`. A ideia é que, mais pra frente, dê pra passar ela direto pra uma função da lista que percorre todas as músicas, sem ter que criar uma função extra só pra converter o tipo.
- Problema encontrado e solução:nenhum
- Evidências de testes:`gcc -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude tests/test_musica.c src/musica.c -o build/test_musica`\
== build/test_musica ==\
Musica: wasurerarenai - sakanaction , duração: 4
- Próxima ação:implementar a próxima `missão M02`.

## Missão M02 — 13/11
- Integrantes e papéis: João Guilherme Martins (Tester), Romário Andrade (Desenvolvedor)
- Objetivo: fazer a lista funcionar no caso mais simples, que é a lista vazia: criar, ver o tamanho, pegar o primeiro elemento e destruir. Tudo isso no nosso próprio `lse.c`, sem trocar pelo arquivo do kit.
- O que implementamos: as funções `lse_criar`, `lse_tamanho`, `lse_primeiro` e `lse_destruir`. Antes de mexer em qualquer coisa fizemos um commit de segurança ("checkpoint antes da M02") e extraímos o pacote da missão com `unzip -n`, pra não correr o risco de sobrescrever nada nosso. Os 4 arquivos entraram em `missoes/M02/` sem problema.
- Decisão técnica e justificativa: a lista não sabe o tipo das cargas, então o `lse_destruir` recebe a função que apaga cada uma (ex.: `musica_destruir`). Com `NULL`, as cargas não são tocadas.
- Problema encontrado e solução: a gente já tinha começado a `lse_criar` e a `lse_tamanho` antes da missão, e o teste até passava, mas olhando com mais cuidado tinha três coisas erradas:
  - a `lse_criar` não conferia se o `malloc` tinha dado certo. Colocamos um `if` pra devolver `NULL` nesse caso;
  - a `lse_tamanho` não tinha `return` quando a lista era `NULL`. O gcc avisava (`control reaches end of non-void function`), mas isso tinha passado despercebido. Agora devolve 0;
  - a `lse_destruir` ainda estava vazia, então a lista nunca era liberada. 
- Evidências de testes: com o stub original do kit (em que a `lse_criar` devolve `NULL`), o teste da M02 parava logo na primeira verificação (`l != NULL`). Depois da implementação rodamos `make -f missoes/M02/Makefile.incremental test_m02` e saiu `M02: teste de contrato passou.`, sem nenhum aviso de compilação.  Os detalhes estão no relatório de testes.
- Próxima ação: partir pra inserção e remoção na lista (M03/M04). Já descomentamos o trecho do `test_lse.c` que testa isso e, por enquanto, ele falha na linha 10, o que é esperado porque essas funções ainda não existem.


## Missão M03 — 16/11
- Integrantes e papéis: João Guilherme Martins (Tester), Romário Andrade (Desenvolvedor)
- Objetivo: construir a lista de verdade: inserir elementos no início, percorrer a lista com uma função callback sem expor os nós (`t_elem`) e testar os casos-limite.
- O que implementamos: `lse_inserir_inicio` e `lse_percorrer` em `src/lse.c`, a declaração de `lse_percorrer` (e do tipo `t_lse_visitar`) em `include/lse.h` e os testes em `missoes/M03/test_m03.c`. `lse_tamanho`, `lse_primeiro` e `lse_destruir` já vinham da M02 e foram só conferidas. Antes de mexer, fizemos um commit de segurança ("checkpoint antes da M03") com a pasta `missoes/M03`.
- Decisão técnica e justificativa:
  - Carga `NULL` é aceita em `lse_inserir_inicio`, como o enunciado permite. O nó existe e conta no `tamanho`. Por isso `lse_primeiro` devolve `NULL` tanto para lista vazia quanto para primeiro elemento com carga `NULL`; para distinguir, usa-se `lse_tamanho`.
  - Se o `malloc` do nó falhar, a função devolve 0 e a lista fica igual.
  - Quem libera o quê: o nó é liberado pela própria lista, em `lse_destruir`; a carga só é liberada se quem chamou passar a função `destruir_carga`.
  - O teste não enxerga `t_elem` (continua só em `lse.c`), então ele confere a lista pelo `lse_percorrer`.
- Problema encontrado e solução:
  - O enunciado declara o tipo como `t_lse_visitar`, mas usa `t_visitar` no protótipo, o que não compila. Usamos `t_lse_visitar` nos dois lugares.
  - A primeira versão de `lse_inserir_inicio` recusava carga `NULL` (`|| c == NULL`), contra o enunciado. Tiramos essa condição.
  - Ao copiar o código, perdemos os asteriscos em `t_elem *novo` e em `/*` de um comentário, o que quebra a compilação. Corrigimos e conferimos.
  - O `gcc` não estava instalado no Windows; compilamos e testamos pelo WSL.
- Evidências de testes: `make -f missoes/M03/Makefile.incremental test_m03` → `M03: todos os testes passaram.`, sem avisos de compilação. Também rodamos com AddressSanitizer e UBSan, sem nenhum erro. Detalhes no relatório de testes.
- Próxima ação: M04 (inserir no fim, remover e buscar). O `tests/test_lse.c` antigo ainda para na linha de `lse_remover_inicio`, o que é esperado até lá.