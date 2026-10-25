void imprime(PGconn *conn, const char *sql);
void imprimeTabela(PGconn *conn, const char *tabela);
void cadastrarCliente(PGconn *conn);
void cadastrarProduto(PGconn *conn);
void cadastrarVenda(PGconn *conn);