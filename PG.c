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
    char nome[NOME], cpf[CPF], tel[TEL];

    campoObrigatorio(nome, "Nome", NOME);

    printf("\nExemplo: 111.111.111-11\n");
    campoNormal(cpf, "CPF", CPF);

    printf("\nExemplo: 55123456789\n");
    campoNormal(tel, "Telefone", TEL);

    const char *valores[3] = {nome, checkString(cpf), checkString(tel)};
    
    PGresult *res = PQexecParams(conn, "INSERT INTO cliente (nome, cpf, telefone) VALUES ($1, $2, $3)", 3, NULL, valores, NULL, NULL, 0);

    if(PQresultStatus(res) == PGRES_COMMAND_OK)
        printf("\nCliente cadastrado!\n");
    
    else
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));

    PQclear(res);
}

void cadastrarProduto(PGconn *conn){
    char desc[DESC], preco[PRECO], estoq[ESTOQ];

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
    char idClient[ID];

    campoID(conn, idClient, "ID do Cliente", ID, cliente);
    
    const char *valores[1] = {idClient};
    
    PGresult *res = PQexecParams(conn, "INSERT INTO venda (id_cliente) VALUES ($1)", 1, NULL, valores, NULL, NULL, 0);

    if(PQresultStatus(res) == PGRES_TUPLES_OK){
        PQclear(res);
        
        char idVenda[ID], idProd[ID], quant[QUANT], precoUni[PRECO];



    } else
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));

    PQclear(res);
}