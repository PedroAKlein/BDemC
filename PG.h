//* TABELAS
#define cliente 1
#define produto 2
#define venda 3
#define item_venda 4

PGconn *conectar();
int resultStatus(PGconn *conn, PGresult *res);
void cadastrarCliente(PGconn *conn);
void cadastrarProduto(PGconn *conn);
void cadastrarVenda(PGconn *conn);