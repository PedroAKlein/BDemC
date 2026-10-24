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
    int opt;

    campoObrigatorio(nome, "Nome", NOME);
    if(nome[0] == '0')
        return;

    do {
        printf("\nSelecione qual dado deseja compartilhar:\n");
        printf(" 1 - CPF\n 2 - Telefone\n 3 - CPF e Telefone\n");
        printf("Opcao: ");
    } while(opt < 1 || opt > 3);

    if(opt == 1){
        printf("\nExemplo: 111.111.111-11\n");
        campoObrigatorio(cpf, "CPF", CPF);
        
        if(cpf[0] == '0')
            return;

        tel[0] = '\n';

    } else if(opt == 2){
        printf("\nExemplo: 55123456789\n");
        campoObrigatorio(tel, "Telefone", TEL);
        
        if(tel[0] == '0')
            return;

        cpf[0] = '\n';

    } else {
        printf("\nExemplo: 111.111.111-11\n");
        campoObrigatorio(cpf, "CPF", CPF);

        if(cpf[0] == '0')
            return;

        printf("\nExemplo: 55123456789\n");
        campoObrigatorio(tel, "Telefone", TEL);

        if(tel[0] == '0')
            return;
    }


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
    if(desc[0] == '0')
        return;

    printf("\nExemplo: 12.50");
    campoObrigatorio(preco, "Preco", PRECO);
    if(preco[0] == '0')
        return;

    campoObrigatorio(estoq, "Estoque", ESTOQ);
    if(estoq[0] == '0')
        return;

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
    if(idClient[0] == '0')
        return;
    
    const char *valores[1] = {idClient};
    
    PGresult *res = PQexecParams(conn, "INSERT INTO venda (id_cliente) VALUES ($1)", 1, NULL, valores, NULL, NULL, 0);

    if(PQresultStatus(res) == PGRES_TUPLES_OK){
        PQclear(res);
        
        char idVenda[ID], idProd[ID], quant[QUANT], precoUni[PRECO];



    } else
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));

    PQclear(res);
}