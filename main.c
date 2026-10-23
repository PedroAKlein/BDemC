#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "PG.h"
#include "util.h"

void menu();
void menuTab(char *titulo);
void cadastro(PGconn *conn);
void consulta(PGconn *conn);

int main(){
    int opt = -1, exit = 0;

    PGconn *conn = conectar();

    while(opt != 0){
        menu();
        scanf("%d", &opt);
        getchar();

        switch(opt){
            case CADASTRO: cadastro(conn); break;
            case CONSULTA: consulta(conn); break;

            case ALTERACAO:
                
                break;

            case ORGANIZACAO:
                
                break;

            case REGISTRO:
                
                break;

            case RELATORIO:
                
                break;

            case SAIR: exit = 1; break;
            
            default: printf("Opcao Invalida!\n"); break;
        }

        if(exit) break;
    }

    PQfinish(conn); 
    return 0;
}

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

void menuTab(char *titulo){
    printf("\n----== %s ==----\n", titulo);
    printf(" Selecione uma tabela:\n");
    printf("  1 - Clientes\n");
    printf("  2 - Produtos\n");
    printf("  3 - Vendas\n");
    printf("  4 - Itens em Venda\n");
    printf(" 0 - Sair\n");
    printf("\n");
    printf("Opcao: ");
}

void cadastro(PGconn *conn){
    int opt = -1;

    while(opt != 0){
        menuTab("CADASTRO");
    
        scanf("%d", &opt);
        getchar();
    
        switch(opt){
            case cliente: cadastrarCliente(conn); break;
            case produto: cadastrarProduto(conn); break;
            case venda: //cadastrarVenda(conn); break;
            case item_venda: //cadastrarItem_venda(conn); break;
            case SAIR: return;
    
            default: printf("Opcao Invalida!\n"); break;
        }
    }
}

void consulta(PGconn *conn){
    int opt = -1;

    while(opt != 0){
        menuTab("CONSULTA");
    
        scanf("%d", &opt);
        getchar();
    
        switch(opt){
            case cliente: imprimeTabela(conn, "cliente"); break;
            case produto: imprimeTabela(conn, "produto"); break;
            case venda: imprimeTabela(conn, "venda"); break;
            case item_venda: imprimeTabela(conn, "item_venda"); break;
            case SAIR: return;
    
            default: printf("Opcao Invalida!\n"); break;
        }
    }
}

