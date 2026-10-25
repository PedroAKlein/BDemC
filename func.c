#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "PG.h"
#include "util.h"
#include "func.h"

void imprime(PGconn *conn, const char *sql){
    PGresult *res = PQexec(conn, sql);

    if(resultStatus(conn, res) == 1)
        return;

    int linhas  = PQntuples(res);
    int colunas = PQnfields(res);

    for(int i=0; i<colunas; i++)
        printf("%-35s", PQfname(res, i));

    printf("\n");

    for(int i=0; i<linhas; i++){
        for(int j=0; j<colunas; j++)
            printf("%-35s", PQgetvalue(res, i, j));

        printf("\n");
    }
}

void imprimeTabela(PGconn *conn, const char *tabela){
    char sql[128];
    snprintf(sql, sizeof(sql), "SELECT * FROM %s", tabela);
    imprime(conn, sql);
}

void cadastrarCliente(PGconn *conn){
    char nome[NOME], cpf[CPF], tel[TEL];
    int opt;

    campoObrigatorio(nome, "Nome (0 para sair)", NOME);
    if(nome[0] == '0')
        return;

    do {
        printf("\nSelecione qual dado deseja compartilhar:\n");
        printf(" 1 - CPF\n 2 - Telefone\n 3 - CPF e Telefone\n");
        printf("Opcao: ");
        scanf("%d", &opt);
        getchar();
    } while(opt < 1 || opt > 3);

    if(opt == 1){
        printf("\nExemplo: 111.111.111-11\n");
        campoObrigatorio(cpf, "CPF (0 para sair)", CPF);
        
        if(strcmp(cpf, "0") == 0)
            return;

        tel[0] = '\n';

    } else if(opt == 2){
        printf("\nExemplo: 55123456789\n");
        campoObrigatorio(tel, "Telefone (0 para sair)", TEL);
        
        if(strcmp(tel, "0") == 0)
            return;

        cpf[0] = '\n';

    } else {
        printf("\nExemplo: 111.111.111-11\n");
        campoObrigatorio(cpf, "CPF (0 para sair)", CPF);

        if(strcmp(cpf, "0") == 0)
            return;

        printf("\nExemplo: 55123456789\n");
        campoObrigatorio(tel, "Telefone (0 para sair)", TEL);

        if(strcmp(tel, "0") == 0)
            return;
    }


    const char *valores[3] = {nome, checkString(cpf), checkString(tel)};
    
    PGresult *res = mandarDB(conn, "cliente", "nome, cpf, telefone", 3, valores, NULL);

    if(PQresultStatus(res) == PGRES_COMMAND_OK)
        printf("\nCliente cadastrado!\n");
    
    else
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));

    PQclear(res);
}

void cadastrarProduto(PGconn *conn){
    char desc[DESC], preco[PRECO], estoq[ESTOQ];

    campoObrigatorio(desc, "Descricao (0 para sair)", DESC);
    if(strcmp(desc, "0") == 0)
        return;

    printf("\nExemplo: 12.50");
    campoObrigatorio(preco, "Preco (0 para sair)", PRECO);
    if(strcmp(preco, "0") == 0)
        return;

    campoObrigatorio(estoq, "Estoque (0 para sair)", ESTOQ);
    if(strcmp(estoq, "0") == 0)
        return;

    const char *valores[3] = {desc, preco, estoq};
    
    PGresult *res = mandarDB(conn, "produto", "descricao, preco, estoque", 3, valores, NULL);

    if(PQresultStatus(res) == PGRES_COMMAND_OK)
        printf("\nProduto cadastrado!\n");
    
    else
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));

    PQclear(res);
}

void cadastrarVenda(PGconn *conn){
    char idClient[ID], idVenda[ID], idProd[ID], quant[QUANT], precoUni[PRECO];

    campoID(conn, idClient, "ID do Cliente (0 para sair)", ID, "cliente");
    if(idClient[0] == '0')
        return;

    if (!executa(conn, "BEGIN", 0, NULL))
        return;
    
    const char *valorVenda[1] = {idClient};
    
    PGresult *res = mandarDB(conn, "venda", "id_cliente", 1, valorVenda, "id");
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));
        PQclear(res);
        executa(conn, "ROLLBACK", 0, NULL);
        return;
    }

    pegaValor(res, 0, 0, idVenda, ID);
    PQclear(res);

    int itens = 0;

    while(1){
        campoID(conn, idProd, "ID do Produto (0 para finaliza)", ID, "produto");
        if(strcmp(idProd, "0") == 0)
            break;

        campoObrigatorio(quant, "Quantidade Vendida", QUANT);

        if(!soDigitos(quant) || atoi(quant) <= 0){
            printf("\nQuantidade invalida!\n");
            continue;
        }

        const char *vProd[1] = {idProd};

        PGresult *r = PQexecParams(conn, "SELECT preco, estoque FROM produto WHERE id = $1", 1, NULL, vProd, NULL, NULL, 0);

        if (PQresultStatus(r) != PGRES_TUPLES_OK){
            fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(r));
            PQclear(r);
            executa(conn, "ROLLBACK", 0, NULL);
            return;
        }

        char estoq[ESTOQ];
        pegaValor(r, 0, 0, precoUni, PRECO);
        pegaValor(r, 0, 1, estoq, ESTOQ);
        PQclear(r);

        if (atoi(quant) > atoi(estoq)){
            printf("\nEstoque insuficiente (disponivel: %s).\n", estoq);
            continue;
        }

        const char *vBaixa[2] = {quant, idProd};

        r = PQexecParams(conn, "UPDATE produto SET estoque = estoque - $1::int " "WHERE id = $2::int AND estoque >= $1::int", 2, NULL, vBaixa, NULL, NULL, 0);

        if (PQresultStatus(r) != PGRES_COMMAND_OK){
            fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(r));
            PQclear(r);
            executa(conn, "ROLLBACK", 0, NULL);
            return;
        }

        if (atoi(PQcmdTuples(r)) == 0){
            printf("\nEstoque insuficiente.\n");
            PQclear(r);
            continue;
        }
        PQclear(r);

        const char *vItem[4] = {idVenda, idProd, quant, precoUni};
        if (!executa(conn,
                "INSERT INTO item_venda (id_venda, id_produto, quantidade, preco_unitario) "
                "VALUES ($1, $2, $3, $4) "
                "ON CONFLICT (id_venda, id_produto) DO UPDATE "
                "SET quantidade = item_venda.quantidade + EXCLUDED.quantidade",
                4, vItem)) {
            executa(conn, "ROLLBACK", 0, NULL);
            return;
        }

        itens++;
        printf("\nItem adicionado!\n");
    }

    if(itens == 0){
        executa(conn, "ROLLBACK", 0, NULL);
        printf("\nVenda cancelada (nenhum item).\n");
        return;
    }

    if(executa(conn, "COMMIT", 0, NULL))
        printf("\nVenda %s registrada!\n", idVenda);
}

