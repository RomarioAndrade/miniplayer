# Diário de bordo
## Missão M01 — 11/11
- Integrantes e papéis: João Guilherme Martins (Tester), Romário Andrade (Desenvolvedor)
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
s
## Missão M04 — 18/11 a 20/11
- Integrantes e papéis: João Guilherme Martins (Tester), Romário Andrade (Desenvolvedor)
- Objetivo: inserir no fim, buscar por chave e remover (do início ou por chave) sem quebrar o encadeamento da lista.
- O que implementamos: em `src/lse.c`, `lse_inserir_fim`, `lse_remover_inicio` e `lse_buscar` (Romário) e `lse_remover` (João, a pedido do Romário, que revisou e concordou com o código). Em `missoes/M04/test_m04.c`, completamos os três trechos que o professor deixou em branco: inserção no fim e busca, remoção do fim e do início, e remoção do início em lista com elementos. Também fizemos duas correções no código existente (ver problemas abaixo).
- Decisão técnica e justificativa:
  - Busca e remoção usam o callback `t_comparador`, para a lista continuar sem conhecer `t_musica`.
  - Remover libera só o nó; a carga volta para quem chamou, que decide se libera.
  - Com chave repetida, busca e remoção agem só na primeira ocorrência.
  - `lse_remover` religa a cadeia e guarda a carga antes do `free`, para nunca ler o nó depois de liberado.
  - Sem ponteiro `fim`, inserir no fim custa O(n); o `fim` fica para a M08.
  - Carga `NULL` continua aceita (decisão da M03), mas isso deixa ambíguo o retorno `NULL` de busca e remoção ("não achou" ou "achou carga NULL"). Os testes não usam carga `NULL` nesses casos e a limitação está registrada em `docs/contratos.md`, junto com a ideia de uma API futura com parâmetro de saída.
  - O enunciado chama o tipo do comparador de `t_comparador_lse`, mas o `lse.h` do projeto usa `t_comparador`. Mantivemos `t_comparador` e não alteramos o cabeçalho.
- Problema encontrado e solução:
  - **`lse_inserir_fim` devolvia `-1` para lista `NULL`**, mas o contrato e o teste do professor (`lse_inserir_fim(NULL,&x)==0`) esperam `0`. O teste da M04 abortava justamente nessa linha. O Romário concordou e passamos a devolver `0`.
  - **`lse_inserir_fim` usava o nó antes de conferir o `malloc`** (escrevia em `novo->prox` e só depois testava `NULL`). Agora confere o `malloc` antes de usar o nó e devolve `0` sem alterar a lista.
  - **`lse_tamanho(NULL)` devolvia `-1`**, mas o contrato da M02 diz `0`. Combinamos com o Romário que o certo é `0` e alinhamos o código com o contrato.
  - **Aviso do compilador `'/*' within comment`** (`-Wcomment`): um comentário `/* TODO M04` ficou sem fechar no fim de `lse_remover_inicio`. Fechamos o comentário (`/* M04 */`).
  - **Erro `redefinition of 'lse_remover'`** ao colar a função: o stub antigo (`return NULL;` com `TODO M04`) ainda estava no arquivo. Apagamos o stub e deixamos só a implementação.
  - **Conflito no `git pull`** nos documentos: o Romário tinha usado `git commit --amend`, e o histórico local e o remoto divergiram. Cancelamos o merge (`git merge --abort`), criamos um branch de segurança (`backup-local-19-11`), conferimos com `git diff` que só o `Makefile` e quatro documentos diferiam e alinhamos com o remoto (`git reset --hard origin/main`), sem perder código.
  - **`Makefile` principal**: a regra do `test_m04` usava variáveis `$(M04_CC)` e `$(M04_CFLAGS)` que nunca eram definidas, então não compilava. Era um teste do Romário que ficou sem reverter. Agora o `Makefile` inclui automaticamente `missoes/M*/Makefile.incremental`; `make test_m02`, `make test_m03`, `make test_m04` e `make test_missoes` funcionam na raiz.
  - Ao colar código no VS Code os asteriscos de ponteiros (`t_elem *novo`) e de comentários às vezes sumiam; conferimos cada trecho colado antes de compilar.
- Evidências de testes:
  - `make -f missoes/M04/Makefile.incremental test_m04` → `M04: todos os testes passaram.` (código de saída 0), sem avisos de compilação depois de preencher os testes (antes aparecia `unused variable` nas funções vazias).
  - Com `-fsanitize=address,undefined` → `M04: todos os testes passaram.`, sem nenhum erro de memória.
  - Regressão: `make test_missoes` roda M02, M03 e M04 e todas passam (`M02: teste de contrato passou.`, `M03: todos os testes passaram.`, `M04: todos os testes passaram.`, código 0). A M03 também passou com ASan/UBSan.
  - Teste falhando antes da correção: `Assertion 'lse_inserir_fim(NULL,&x)==0' failed` (`test_m04.c:80`). Depois da correção, passou. Detalhes no relatório de testes.
- Próxima ação: M05, assim que o professor liberar o pacote da missão. Antes disso, deixar o `CHECKLIST.md` da M04 marcado e fazer o commit final identificável da missão.

## Missão M05 — 23/11 a 24/11
- Integrantes e papéis: João Guilherme Martins (Tester), Romário Andrade (Desenvolvedor)
- Objetivo: inserir um elemento na posição certa de uma lista já ordenada, usando um comparador genérico, sem expor `t_elem` e sem ordenar a lista inteira a cada inserção.
- O que implementamos: `lse_inserir_ordenado` em `src/lse.c` (Romário) e os testes em `missoes/M05/test_m05.c` (João): lista vazia, início, meio e fim com inteiros (`teste_vazia_inicio_meio_fim`), empates estáveis com registros de música (`teste_duplicados_estaveis`) e argumentos inválidos, reinserção e integração com `lse_remover` (`teste_invalido_e_reinsercao`). O kit da M05 foi instalado copiando só a pasta `missoes/M05`, sem substituir `src/lse.c`, `include/lse.h` nem o `Makefile`, e o commit da instalação foi feito antes de implementar.
- Decisão técnica e justificativa:
  - O comparador é chamado como `comparar(carga_existente, carga_nova)`, com duas cargas (na M04 a comparação era entre carga e chave).
  - Empates: o laço avança enquanto `comparar(atual->carga, carga_nova) <= 0`, então o novo elemento entra **depois** dos iguais, preservando a ordem de chegada (estabilidade).
  - A função devolve `0` para lista `NULL`, comparador `NULL` ou falha de `malloc`, e nesses casos a lista não é alterada (o `malloc` é conferido antes de qualquer ligação).
  - A ligação segue a ordem `novo->prox = atual` e depois `inicio = novo` (se `anterior == NULL`) ou `anterior->prox = novo`, para nunca perder o resto da lista.
  - Complexidade: tempo O(n) no pior caso e espaço extra O(1). A lista continua sem ponteiro `fim`.
  - A lista não copia nem libera a carga; nenhuma cópia de `t_musica` é feita.
  - O enunciado cita `t_lse_comparador` nos objetivos, mas a assinatura do contrato usa `t_comparador`, que é o nome que já existe no `lse.h`. Mantivemos `t_comparador` e não alteramos o cabeçalho.
- Problema encontrado e solução:
  - A primeira versão dos testes tinha trocado os `assert(1==0)` do kit por `assert(l)` depois de `lse_destruir`, e o terceiro teste estava comentado. Nenhum chamava `lse_inserir_ordenado`, então passavam sem verificar nada. Substituímos pelos testes completos, que conferem tamanho e ordem ponteiro a ponteiro.
  - O arquivo `lse.c` terminava a função com `/* TODO M05 */`; trocamos por `/* M05 */`, com o aval do Romário.
  - O `valgrind` não está instalado no WSL; a verificação de vazamentos foi feita com o LeakSanitizer, que já vem com o AddressSanitizer.
- Evidências de testes:
  - Kit original, antes de preencher os testes: `Assertion '1==0' failed` em `test_m05.c:20` (código 134).
  - `make test_m05` → `M05: todos os testes passaram.` (código 0), sem avisos de compilação.
  - Regressão: `make test_missoes` roda M02, M03, M04 e M05 e todas passam (código 0).
  - Com `-fsanitize=address,undefined` → `M05: todos os testes passaram.`, sem nenhum erro de memória. Detalhes no relatório de testes.
- Reflexão (questões do enunciado):
  - **Por que inserir no início é O(1) e a inserção ordenada pode ser O(n)?** Inserir no início só mexe em `inicio`, sem procurar nada. Na inserção ordenada é preciso achar a posição, e como a lista encadeada não tem acesso direto a um nó do meio, o laço pode percorrer todos os nós (por exemplo, quando o novo é maior que todos).
  - **Qual o risco de usar critérios diferentes em duas inserções na mesma lista?** A lista deixa de estar ordenada por qualquer critério. Como a função assume que a lista já está ordenada pelo mesmo critério, as inserções seguintes escolhem posições erradas e a ordem não se recupera.
  - **Como o comparador permite ordenar por título, duração ou identificador sem alterar `lse.c`?** O `lse.c` só chama o ponteiro de função que recebe e nunca olha dentro da carga. Para mudar o critério basta passar outra função de comparação (`strcmp` do título, duração ou `id`), e a lista continua sem conhecer `t_musica`.
  - **O que ocorre com a ordem relativa de elementos com chaves iguais?** Ela é mantida: o `<= 0` faz o novo elemento entrar depois dos iguais, então a ordem de chegada é preservada (inserção estável).
- Próxima ação: M06 (pilha como especialização da lista), em 25/11. Antes disso, marcar o `CHECKLIST.md` da M05, atualizar o `README.md` e fazer o commit final da missão.

## Missão M06 — 25/11
- Integrantes: João Guilherme Martins, Romário Andrade
- Objetivo: implementar uma pilha genérica LIFO reutilizando a LSE das M03–M05 por composição, sem duplicar o encadeamento e sem acessar `t_elem`.
- O que implementamos: em `src/pilha.c`, a estrutura privada `struct pilha { t_lse *lista; }`, `pilha_criar` (com tratamento de falha parcial), `pilha_topo` e `pilha_destruir` (Romário), e `pilha_empilhar` e `pilha_desempilhar` (João). Teste próprio em `tests/test_pilha.c` (cenário do enunciado e intercalação conferida contra um vetor-modelo), incluído no `make test`.
- Decisão técnica e justificativa:
  - **Composição:** a pilha guarda um `t_lse *` e usa só as funções de `lse.h`. Assim reaproveita o encadeamento já testado e a representação da lista continua opaca.
  - **Criação em passos:** primeiro `lse_criar`, depois o `malloc` da pilha. Se o segundo falhar, a lista já criada é liberada com `lse_destruir(lista, NULL)`, e não com `free`, porque a lista é um tipo opaco criado pela LSE.
  - **Destruição:** `lse_destruir(p->lista, d)` e depois `free(p)`, nessa ordem, porque o ponteiro da lista está dentro da pilha. O callback é repassado sem alteração: com `NULL` as cargas ficam intactas (referências emprestadas, como as músicas da biblioteca no histórico); com uma função, as cargas são liberadas.
  - **Empilhar/desempilhar no início:** delegam para `lse_inserir_inicio` e `lse_remover_inicio`, ambas O(1).
  - **Topo:** o último empilhado fica no início da lista, então `pilha_topo` devolve `lse_primeiro(p->lista)`, só consultando, sem remover.
  - Complexidade: criar, empilhar, desempilhar e topo O(1); destruir O(n), sem contar o callback.
- Problema encontrado e solução:
  - A primeira versão de `pilha.c` redeclarava `t_elem` e montava um encadeamento próprio (`struct pilha { t_elem *topo; }`), o que o enunciado proíbe. Voltamos à estrutura do kit, com `t_lse *lista`.
  - Faltava `#include <stdlib.h>`, e a compilação falhava com `implicit declaration of function 'malloc'` e `'free'`.
  - Na falha parcial da criação, o código fazia `free(pilha)` dentro de `if (pilha == NULL)`, ou seja, `free(NULL)`, que não faz nada, e a lista criada antes vazava. Trocamos por `lse_destruir(lista, NULL)`. Regra adotada: em uma falha, liberar o que deu certo antes, e não o que acabou de falhar.
  - `pilha_topo` devolvia `p->lista` (o ponteiro da lista) em vez da carga. Compilava sem aviso, porque todo ponteiro vira `void *`, mas o teste falhava com a pilha vazia (`pilha_topo(p)==NULL`). Trocamos por `lse_primeiro(p->lista)`.
- Evidências de testes:
  - `src/pilha.c` compila com `-std=c11 -Wall -Wextra -Wpedantic` sem avisos.
  - Criar e destruir uma pilha vazia (com callback `NULL` e com callback), além de `pilha_destruir(NULL, ...)`, rodou com `-fsanitize=address,undefined` sem erros de memória nem vazamentos, e o callback não foi chamado para a pilha vazia.
  - `pilha_topo` devolve `NULL` para pilha vazia (duas consultas seguidas) e para `NULL`, também sem erros com ASan/UBSan.
  - `make test_missoes`: M02 a M06 passam (`M06: todos os testes passaram.`). `make test`: `test_pilha: OK`. Com ASan/UBSan, sem erros nem vazamentos. Detalhes no relatório de testes (T27–T29).
- Reflexão (questões do enunciado):
  - **Por que inserir e remover no início dá O(1)?** Só se altera `inicio`, sem percorrer nada. Remover no fim de uma LSE é O(n), porque é preciso achar o penúltimo nó.
  - **Por que consultar o topo não pode remover a carga?** Consulta e remoção são operações diferentes: quem só quer olhar o topo perderia o elemento e a pilha mudaria sem ser pedido.
  - **Por que destruir o histórico não deve liberar as músicas?** As músicas pertencem à biblioteca; o histórico só guarda referências. Liberá-las deixaria a biblioteca com ponteiros inválidos (uso após `free` ou liberação dupla).
  - **Composição × duplicação?** Na composição a pilha usa a LSE já testada pela interface pública; duplicar seria reescrever o encadeamento dentro da pilha, com mais código para manter e testar.
- Próxima ação: M07 (histórico e navegação com duas pilhas), em 27/11. A M06 está concluída: checklist marcado, README atualizado e commit final feito.