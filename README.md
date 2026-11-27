# MiniPlayer — ICC002 (2026/2)
Projeto integrador progressivo, em duplas, em C. Reprodução **simulada**.

## Integrantes
- João Guilherme Franco de Sá Martins
- Romário Roberto de Pádua Andrade

## Compilação e Execução
`make` e `./build/miniplayer`.

## Testes
`make test`. **ATENÇÃO:** os testes estão preparados para falhar até que os stubs sejam implementados. Isso é intencional. Comece com `make build/test_musica && ./build/test_musica`.

## Contratos
- `void *carga` é genérico; a lista não conhece `t_musica`.
- A lista possui os nós, **não** as cargas, salvo quando `lse_destruir` recebe callback destruidor.
- Remover devolve a carga e libera apenas o nó.
- Pilha e fila reutilizam `lse_*`, sem manipular `t_elem`.
- Biblioteca possui as músicas; histórico e fila só as referenciam.
- Antes de liberar uma música, remover referências a ela de fila e histórico.

## Como documentar
Preencher `docs/arquitetura.md`, `docs/contratos.md`, `docs/diario-de-bordo.md` e `docs/relatorio-testes.md` a cada missão. Ver `GUIA_ESTUDANTE.md`.

## Estado atual (M06)
- Missões concluídas: M01 (música), M02 (lista vazia), M03 (inserção no início e percurso), M04 (inserção no fim, busca e remoção) e M05 (inserção ordenada genérica) da lista simplesmente encadeada (`src/lse.c`); M06 (pilha LIFO por composição sobre a LSE, `src/pilha.c`).
- Ainda como stub: fila (M08).

## Como executar os testes das missões
Na raiz do projeto:
- `make test_m02` … `make test_m06`: testes de cada missão.
- `make test_missoes`: testes de todas as missões (regressão); passam M02 a M06.
- `make test`: inclui o teste próprio da pilha (`tests/test_pilha.c`).
- Também funciona sem o Makefile principal: `make -f missoes/M06/Makefile.incremental test_m06`.
- Para detectar erros de memória: compile com `-fsanitize=address,undefined`.