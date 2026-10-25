//* TABELAS
#define cliente 1
#define produto 2
#define venda 3
#define item_venda 4

PGconn *conectar();
int resultStatus(PGconn *conn, PGresult *res);
PGresult *mandarDB(PGconn *conn, const char *tabela, const char *colunas, int nValues, const char **valores, const char *retorno);
int executa(PGconn *conn, const char *sql, int n, const char *const *valores);
void pegaValor(PGresult *res, int linha, int coluna, char *var, int tam);
int existeId(PGconn *conn, const char *tabela, const char *id);