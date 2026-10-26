#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "func.h"
#include "PG.h"
#include "util.h"

int main(){
    int opt = -1, sair = 0;

    PGconn *conn = conectar();

    while(opt != 0){
        menu();
        scanf("%d", &opt);
        getchar();

        switch(opt){
            case CADASTRO: cadastro(conn); break;
            case CONSULTA: consulta(conn); break;
            case ALTERACAO: alteracao(conn); break;

            case ORGANIZACAO:
                
                break;

            case REGISTRO:
                
                break;

            case RELATORIO:
                
                break;

            case SAIR: sair = 1; break;
            
            default: printf("Opcao Invalida!\n"); break;
        }

        if(sair) break;
    }

    PQfinish(conn); 
    return 0;
}