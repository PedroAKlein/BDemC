#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "util.h"
#include "PG.h"

void remove_quebraLinha(char *str){
    size_t len = strlen(str);
    if(len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

// se a string for vazia, seu valor sera NULL
char *checkString(char *str){
    return (str[0] == '\0' || str[0] == '\n') ? NULL : str;
}

void imprime(PGconn *conn, const char *sql, const char *tabela){
    PGresult *res = PQexec(conn, sql);

    if(resultStatus(conn, res) == 1)
        return;

    int linhas  = PQntuples(res);
    int colunas = PQnfields(res);

    printf("\n");
    printf("\n");
    printf("Tabela: %s", tabela);
    printf("\n");

    for(int i=0; i<colunas; i++)
        printf("%-20s", PQfname(res, i));

    printf("\n");

    for(int i=0; i<linhas; i++){
        for(int j=0; j<colunas; j++)
            printf("%-20s", PQgetvalue(res, i, j));

        printf("\n");
    }

}

void imprimeTabela(PGconn *conn, const char *tabela){
    char sql[128];
    snprintf(sql, sizeof(sql), "SELECT * FROM %s", tabela);
    imprime(conn, sql, tabela);
}

void campoObrigatorio(char *campo, char *text, int tam){
    while(1){
        printf("\n*%s (0 para sair): ", text);
        fgets(campo, tam, stdin);

        if(campo[0] != '\n')
            break;

        printf("%s eh obrigatorio!\n", text);
    }
    remove_quebraLinha(campo);
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

int soDigitos(const char *str){
    if(str[0] == '\0') 
        return 0;

    for (; *str; str++)
        if (*str < '0' || *str > '9') 
            return 0;

    return 1;
}
