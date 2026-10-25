#include <stdio.h>
#include "menu.h"

void menu(){
    printf("\n----== MENU ==----\n");
    printf(" 1 - Cadastro\n");
    printf(" 2 - Consulta\n");
    printf(" 3 - Alteracao\n");
    printf(" 4 - Organizacao\n");
    printf(" 5 - Registro\n");
    printf(" 6 - Relatorio\n");
    printf(" 0 - Sair\n");
    printf("\n");
    printf("Opcao: ");
}

void menuCoringa(char *titulo){
    printf("\n----== %s ==----\n", titulo);
    printf(" Selecione uma tabela:\n");
    printf("  1 - Clientes\n");
    printf("  2 - Produtos\n");
    printf("  3 - Vendas\n");
    printf(" 0 - Sair\n");
    printf("\n");
    printf("Opcao: ");
}