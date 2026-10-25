#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "menu.h"
#include "cadastro.h"
#include "PG.h"
#include "util.h"

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

void cadastro(PGconn *conn){
    int opt = -1;

    while(opt != 0){
        menuCoringa("CADASTRO");
    
        scanf("%d", &opt);
        getchar();
    
        switch(opt){
            case cliente: cadastrarCliente(conn); break;
            case produto: cadastrarProduto(conn); break;
            case venda: cadastrarVenda(conn); break;
            case SAIR: return;
    
            default: printf("Opcao Invalida!\n"); break;
        }
    }
}

void consulta(PGconn *conn){
    int opt = -1;

    while(opt != 0){
        menuCoringa("CONSULTA");
    
        scanf("%d", &opt);
        getchar();
    
        switch(opt){
            case cliente: imprimeTabela(conn, "cliente"); break;
            case produto: imprimeTabela(conn, "produto"); break;
            case venda: imprimeTabela(conn, "venda"); break;
            case SAIR: return;
    
            default: printf("Opcao Invalida!\n"); break;
        }
    }
}

