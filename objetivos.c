//// 1. (0,5) Exibe um prompt e aguarda entrada do usuário.
//// 2. (0,5) O comando exit deve sair do processador de comandos.
//// 3. (0,5) Quando a entrada do usuário é o nome de um programa, o mesmo deve ser
//// executado.
//// 4. (0,5) Quando a entrada do usuário não é um programa ou comando válido, deve ser
//// mostrada uma mensagem de erro adequada.
//// 5. (1,0) Os argumentos digitados na linha de comando devem ser passados ao programa
//// que será executado.

//// 6. (1,0) O prompt deve ter o formato: [MySh] nome-de-usuario@hospedeiro:diretorio-
//// atual$.

//// 7. (0,5) No prompt, o caminho do diretório home do usuário deve ser substituído por ~,
//// caso este faça parte do caminho atual. Exemplo: de /home/juquinha/aula para
//// ~/aula.
//// 8. (0,5) Implemente o comando cd para mudar diretórios (quando recebe argumento) ou
//// voltar ao diretório home do usuário (sem argumentos ou com o argumento ~).
//// 9. (0,5) Mostre uma mensagem de erro adequada se cd falhar.
//// 10. (1,0) Ctrl+Z e Ctrl+C não devem causar a parada ou interrupção do processador de
//// comandos (apenas ignore a linha com o comando).
//// 11. (0,5) Ctrl+D deve sair do processador de comandos, tal qual se o comando exit tivesse
//// sido utilizado.
//// 12. (1,0) O processador de comandos deve permitir o uso de pipes. O símbolo | indica a
//// separação entre cada programa, conectando a saída padrão do programa à esquerda
//// com a entrada padrão do programa à direita através de um pipe.
//// 13. (2,0) O processador de comandos deve permitir o uso de múltiplos pipes na mesma
//// linha de comando.