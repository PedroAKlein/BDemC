#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "PG.h"
#include "util.h"

PGconn *conectar(){
    PGconn *conn = PQconnectdb(
        "host=localhost port=5432 dbname=loja user=postgres password=pedroca"
    );
    
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Erro: %s\n", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    return conn;
}

// retorna 1 se deu erro
// retorna 0 se deu certo
int resultStatus(PGconn *conn, PGresult *res){
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        fprintf(stderr, "Erro na consulta: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 1;
    }

    return 0;
}

void cadastrarCliente(PGconn *conn){
    const char nome[NOME], cpf[CPF], tel[TEL];

    campoObrigatorio(nome, "Nome", NOME);

    printf("\nExemplo: 111.111.111-11\n");
    printf("CPF: ");
    fgets(cpf, CPF, stdin);
    remove_quebraLinha(cpf);

    printf("\nExemplo: 55123456789\n");
    printf("Telefone: ");
    fgets(tel, TEL, stdin);
    remove_quebraLinha(tel);

    const char *valores[3] = {nome, checkString(cpf), checkString(tel)};
    
    PGresult *res = PQexecParams(conn, "INSERT INTO cliente (nome, cpf, telefone) VALUES ($1, $2, $3)", 3, NULL, valores, NULL, NULL, 0);

    if(PQresultStatus(res) == PGRES_COMMAND_OK)
        printf("\nCliente cadastrado!\n");
    
    else
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));

    PQclear(res);
}

void cadastrarProduto(PGconn *conn){
    const char desc[DESC], preco[PRECO], estoq[ESTOQ];

    campoObrigatorio(desc, "Descricao", DESC);

    printf("\nExemplo: 12.50");
    campoObrigatorio(preco, "Preco", PRECO);

    campoObrigatorio(estoq, "Estoque", ESTOQ);

    const char *valores[3] = {desc, preco, estoq};
    
    PGresult *res = PQexecParams(conn, "INSERT INTO produto (descricao, preco, estoque) VALUES ($1, $2, $3)", 3, NULL, valores, NULL, NULL, 0);

    if(PQresultStatus(res) == PGRES_COMMAND_OK)
        printf("\nProduto cadastrado!\n");
    
    else
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));

    PQclear(res);
}

void cadastrarVenda(PGconn *conn){
    const char idClient[ID];

    while(1){
        campoObrigatorio(idClient, "ID do Cliente", ID);

        if(!soDigitos(idClient)){
            printf("\nO ID deve possuir apenas digitos!\n");
            continue;
        }
        
        if(!existeId(conn, "cliente", idClient)){
            printf("\nO ID nao existe!\n");
            continue;
        }
        
        break;
    }
    

    const char *valores[1] = {idClient};
    
    PGresult *res = PQexecParams(conn, "INSERT INTO produto (descricao, preco, estoque) VALUES ($1, $2, $3)", 3, NULL, valores, NULL, NULL, 0);

    if(PQresultStatus(res) == PGRES_COMMAND_OK)
        printf("\nProduto cadastrado!\n");
    
    else
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));

    PQclear(res);
}