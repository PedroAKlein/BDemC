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
        opt = lerOpcao();

        switch(opt){
            case CADASTRO: cadastro(conn); break;
            case CONSULTA: consulta(conn); break;
            case ALTERACAO: alteracao(conn); break;
            case RELATORIO: relatorio(conn); break;
            case SAIR: sair = 1; break;
            
            default: printf("Opcao Invalida!\n"); break;
        }

        if(sair) break;
    }

    PQfinish(conn); 
    return 0;
}