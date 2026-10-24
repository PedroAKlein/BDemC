#define CADASTRO 1
#define CONSULTA 2
#define ALTERACAO 3
#define ORGANIZACAO 4
#define REGISTRO 5
#define RELATORIO 6
#define SAIR 0

#define NOME 102
#define CPF 16
#define TEL 22
#define DESC 102
#define PRECO 12
#define ESTOQ 15
#define QUANT 15
#define ID 12

void imprime(PGconn *conn, const char *sql, const char *tabela);
void imprimeTabela(PGconn *conn, const char *tabela);
void remove_quebraLinha(char *str);
char *checkString(char *str);
void campoObrigatorio(char *campo, char *text, int tam);
void campoID(PGconn *conn, char *campo, char *text, int tam, const char *tabela);
int existeId(PGconn *conn, const char *tabela, const char *id);
int soDigitos(const char *str);