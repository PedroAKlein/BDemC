#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "PG.h"

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

// retorna 1 se falhou, 0 se deu certo
int resultStatus(PGconn *conn, PGresult *res){
    if(PQresultStatus(res) != PGRES_TUPLES_OK){
        fprintf(stderr, "Erro na consulta: %s\n", PQerrorMessage(conn));
        PQclear(res);
        return 1;
    }

    return 0;
}

// simplificacao da funcao do postgresql
PGresult *mandarDB(PGconn *conn, const char *tabela, const char *colunas, int nValues, const char **valores, const char *retorno){
    char sql[256], marcadores[128] = "", tmp[12];

    for(int i = 1; i <= nValues; i++){
        if (i > 1) strcat(marcadores, ", ");
        snprintf(tmp, sizeof tmp, "$%d", i);
        strcat(marcadores, tmp);
    }

    int n = snprintf(sql, sizeof sql, "INSERT INTO %s (%s) VALUES (%s)", tabela, colunas, marcadores);

    if (retorno)
        snprintf(sql + n, sizeof sql - n, " RETURNING %s", retorno);

    return PQexecParams(conn, sql, nValues, NULL, valores, NULL, NULL, 0);
}

// executa um comando que nao devolve linhas (BEGIN, COMMIT, UPDATE, INSERT...)
// retorna 1 se deu certo, 0 se falhou
int executa(PGconn *conn, const char *sql, int n, const char *const *valores){
    PGresult *res = PQexecParams(conn, sql, n, NULL, valores, NULL, NULL, 0);
    
    int ok = (PQresultStatus(res) == PGRES_COMMAND_OK);
    
    if (!ok)
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));
    
    PQclear(res);
    return ok;
}

// atribui um valor do DB a uma string
void pegaValor(PGresult *res, int linha, int coluna, char *var, int tam){
    if (linha >= PQntuples(res) || coluna >= PQnfields(res)) {
        var[0] = '\0';
        return;
    }
    snprintf(var, tam, "%s", PQgetvalue(res, linha, coluna));
}

int existeId(PGconn *conn, const char *tabela, const char *id){
    char sql[128];
    snprintf(sql, sizeof sql, "SELECT 1 FROM %s WHERE id = $1", tabela);

    const char *valores[1] = {id};
    PGresult *res = PQexecParams(conn, sql, 1, NULL, valores, NULL, NULL, 0);

    int existe = 0;
    if (PQresultStatus(res) == PGRES_TUPLES_OK)
        existe = PQntuples(res) > 0;
    else
        fprintf(stderr, "Erro: %s\n", PQresultErrorMessage(res));

    PQclear(res);
    return existe;
}