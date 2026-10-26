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
        printf("%-23s", PQfname(res, i));

    printf("\n");

    for(int i=0; i<linhas; i++){
        for(int j=0; j<colunas; j++)
            printf("%-23s", PQgetvalue(res, i, j));

        printf("\n");
    }

    PQclear(res);
}

void imprimeTabela(PGconn *conn, const char *tabela){
    char sql[128];
    snprintf(sql, sizeof(sql), "SELECT * FROM %s", tabela);
    imprime(conn, sql);
}

void imprimeVendas(PGconn *conn){
    printf("\n");
    imprime(conn,
        "SELECT v.id AS venda, v.data, c.nome AS cliente, "
        "p.descricao AS produto, i.quantidade, i.preco_unitario, "
        "i.quantidade * i.preco_unitario AS subtotal "
        "FROM venda v "
        "JOIN cliente c ON c.id = v.id_cliente "
        "JOIN item_venda i ON i.id_venda = v.id "
        "JOIN produto p ON p.id = i.id_produto "
        "ORDER BY v.id, p.descricao");
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

void alterarCliente(PGconn *conn){
    char idClient[ID], nome[NOME], cpf[CPF], tel[TEL], sql[256];
    PGresult *res;

    while(1){
        campoID(conn, idClient, "ID do Cliente (0 para sair)", ID, "cliente");

        if(strcmp(idClient, "0") == 0)
            return;

        printf("\n");
        snprintf(sql, sizeof(sql), "SELECT nome, cpf, telefone FROM cliente WHERE id = %s", idClient);
        imprime(conn, sql);

        campoNormal(nome, "NOVO Nome (ENTER para pular)", NOME);

        printf("\nExemplo: 111.111.111-11\n");
        campoNormal(cpf, "NOVO CPF (ENTER para pular)", CPF);
        while(!checkCPF(cpf)){
            printf("\nSiga o exemplo, nao se esqueca dos \'.\' e do \'-\'!\n");
            printf("Exemplo: 111.111.111-11\n");
            campoNormal(tel, "NOVO Telefone (ENTER para pular)", TEL);
        }


        printf("\nExemplo: 55123456789\n");
        campoNormal(tel, "NOVO Telefone (ENTER para pular)", TEL);
        while(!soDigitos(tel)){
            printf("\nApenas numeros!\n");
            printf("Exemplo: 55123456789\n");
            campoNormal(tel, "NOVO Telefone (ENTER para pular)", TEL);
        }

        const char *valores[4] = {checkString(nome), checkString(cpf), checkString(tel), idClient};

        if(nome[0] == '\0' && cpf[0] == '\0' && tel[0] == '\0'){
            printf("\nNada a alterar.\n");
            continue;
        }

        res = PQexecParams(conn, "UPDATE cliente SET nome = COALESCE($1, nome), " "cpf = COALESCE($2, cpf), " "telefone = COALESCE($3, telefone) " "WHERE id = $4", 4, NULL, valores, NULL, NULL, 0);

        if(PQresultStatus(res) == PGRES_COMMAND_OK){
            if(atoi(PQcmdTuples(res)) == 1)
                printf("\nAlterado!\n");

            else
                printf("\nNenhum registro alterado.\n");

        } else {
            const char *codigo = PQresultErrorField(res, PG_DIAG_SQLSTATE);

            if(codigo && strcmp(codigo, "23505") == 0)
                printf("\nJa existe um cliente com esse CPF.\n");

            else
                fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));
        }
        PQclear(res);
    }
}

void alterarProduto(PGconn *conn){
    char idProduto[ID], desc[DESC], preco[PRECO], estoq[ESTOQ], sql[256];
    PGresult *res;

    while(1){
        campoID(conn, idProduto, "ID do Produto (0 para sair)", ID, "produto");

        if(strcmp(idProduto, "0") == 0)
            return;

        printf("\n");
        snprintf(sql, sizeof(sql), "SELECT descricao, preco, estoque FROM produto WHERE id = %s", idProduto);
        imprime(conn, sql);

        campoNormal(desc, "NOVA Descricao (ENTER para pular)", DESC);

        printf("\nExemplo: 12.99");
        campoNormal(preco, "NOVO Preco (ENTER para pular)", PRECO);
        while(!checkPRECO(preco)){
            printf("\nSiga o exemplo, nao se esqueca do \'.\'!\n");
            printf("Exemplo: 12.99\n");
            campoNormal(preco, "NOVO Preco (ENTER para pular)", PRECO);
        }

        campoNormal(estoq, "NOVO Estoque (ENTER para pular)", ESTOQ);
        while(!soDigitos(estoq)){
            printf("\nApenas numeros!\n");
            campoNormal(estoq, "NOVO Estoque (ENTER para pular)", ESTOQ);
        }

        const char *valores[4] = {checkString(desc), checkString(preco), checkString(estoq), idProduto};

        if(desc[0] == '\0' && preco[0] == '\0' && estoq[0] == '\0'){
            printf("\nNada a alterar.\n");
            continue;
        }

        res = PQexecParams(conn, "UPDATE produto SET descricao = COALESCE($1, descricao), " "preco = COALESCE($2, preco), " "estoque = COALESCE($3, estoque) " "WHERE id = $4", 4, NULL, valores, NULL, NULL, 0);

        if(PQresultStatus(res) == PGRES_COMMAND_OK){
            if(atoi(PQcmdTuples(res)) == 1)
                printf("\nAlterado!\n");

            else
                printf("\nNenhum registro alterado.\n");
        } 
        PQclear(res);
    }
}

void mostrarVenda(PGconn *conn, const char *idVenda){
    char sql[512];

    snprintf(sql, sizeof sql,
        "SELECT v.id AS venda, v.data, c.nome AS cliente "
        "FROM venda v JOIN cliente c ON c.id = v.id_cliente "
        "WHERE v.id = %s", idVenda);
    imprime(conn, sql);

    printf("\n");
    snprintf(sql, sizeof sql,
        "SELECT p.id, p.descricao AS produto, i.quantidade, i.preco_unitario, "
        "i.quantidade * i.preco_unitario AS subtotal "
        "FROM item_venda i JOIN produto p ON p.id = i.id_produto "
        "WHERE i.id_venda = %s ORDER BY p.descricao", idVenda);
    imprime(conn, sql);
}

void alterarVenda(PGconn *conn){
    char idVenda[ID], opt[8];

    while(1){
        campoID(conn, idVenda, "ID da Venda (0 para sair)", ID, "venda");
        if(strcmp(idVenda, "0") == 0)
            return;

        printf("\n");
        mostrarVenda(conn, idVenda);

        printf("\n 1 - Alterar cliente / data\n 2 - Alterar quantidade de um item\n 0 - Voltar\n");
        campoObrigatorio(opt, "Opcao", 8);

        if(strcmp(opt, "1") == 0){
            char idClient[ID], data[DATA];

            campoNormal(idClient, "NOVO ID do Cliente (ENTER para pular)", ID);
            if(strcmp(idClient, "help") == 0){
                imprimeTabela(conn, "cliente");
                campoNormal(idClient, "NOVO ID do Cliente (ENTER para pular)", ID);
            }
            while(idClient[0] != '\0' && (!soDigitos(idClient) || !existeId(conn, "cliente", idClient))){
                printf("\nCliente invalido ou inexistente!\n");
                printf("Digite \"help\" para ver a tabela de clientes!\n");
                campoNormal(idClient, "NOVO ID do Cliente (ENTER para pular)", ID);
                if(strcmp(idClient, "help") == 0){
                    imprimeTabela(conn, "cliente");
                    campoNormal(idClient, "NOVO ID do Cliente (ENTER para pular)", ID);
                }
            }

            printf("\nExemplo: aaaa-mm-dd\n");
            campoNormal(data, "NOVA Data (ENTER para pular)", DATA);
            while(!checkDATA(data)){
                printf("\nSiga o exemplo, nao se esqueca dos \'-\'!\n");
                printf("Exemplo: aaaa-mm-dd\n");
                campoNormal(data, "NOVA Data (ENTER para pular)", DATA);
            }

            if(idClient[0] == '\0' && data[0] == '\0'){
                printf("\nNada a alterar.\n");
                return;
            }

            const char *valores[3] = {checkString(idClient), checkString(data), idVenda};

            if(executa(conn, "UPDATE venda SET id_cliente = COALESCE($1::int, id_cliente), " "data = COALESCE($2::date, data) WHERE id = $3::int", 3, valores))
                printf("\nAlterado!\n");

        } else if(strcmp(opt, "2") == 0){
            char idProd[ID], quant[QUANT], atual[QUANT], sDiff[QUANT];

            campoID(conn, idProd, "ID do Produto a alterar (0 para voltar)", ID, "produto");
            if(strcmp(idProd, "0") == 0)
                return;

            const char *vBusca[2] = {idVenda, idProd};
            PGresult *res = PQexecParams(conn, "SELECT quantidade FROM item_venda WHERE id_venda = $1 AND id_produto = $2", 2, NULL, vBusca, NULL, NULL, 0);

            if(PQresultStatus(res) != PGRES_TUPLES_OK){
                fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));
                PQclear(res);
                return;
            }
            if(PQntuples(res) == 0){
                printf("\nEsse produto nao esta nessa venda.\n");
                PQclear(res);
                return;
            }
            pegaValor(res, 0, 0, atual, QUANT);
            PQclear(res);

            while(1){
                campoObrigatorio(quant, "NOVA Quantidade (0 para cancelar)", QUANT);
                if(strcmp(quant, "0") == 0)
                    return;
                if(soDigitos(quant) && atoi(quant) > 0)
                    break;
                printf("\nQuantidade invalida!\n");
            }

            int diff = atoi(quant) - atoi(atual);
            if(diff == 0){
                printf("\nNada a alterar.\n");
                return;
            }
            snprintf(sDiff, sizeof sDiff, "%d", diff);

            if(!executa(conn, "BEGIN", 0, NULL))
                return;

            const char *vEst[2] = {sDiff, idProd};
            res = PQexecParams(conn, "UPDATE produto SET estoque = estoque - $1::int " "WHERE id = $2::int AND estoque >= $1::int", 2, NULL, vEst, NULL, NULL, 0);

            if(PQresultStatus(res) != PGRES_COMMAND_OK){
                fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));
                PQclear(res);
                executa(conn, "ROLLBACK", 0, NULL);
                return;
            }
            if(atoi(PQcmdTuples(res)) == 0){
                printf("\nEstoque insuficiente.\n");
                PQclear(res);
                executa(conn, "ROLLBACK", 0, NULL);
                return;
            }
            PQclear(res);

            const char *vItem[3] = {quant, idVenda, idProd};
            if(!executa(conn, "UPDATE item_venda SET quantidade = $1::int " "WHERE id_venda = $2::int AND id_produto = $3::int", 3, vItem)){
                executa(conn, "ROLLBACK", 0, NULL);
                return;
            }

            if(executa(conn, "COMMIT", 0, NULL))
                printf("\nQuantidade alterada!\n");
        }
    }
}