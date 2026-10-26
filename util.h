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
#define DATA 12

void menu();
void menuCoringa(char *titulo);
void consulta(PGconn *conn);
void cadastro(PGconn *conn);
void alteracao(PGconn *conn);
void remove_quebraLinha(char *str);
char *checkString(char *str);
void campoObrigatorio(char *campo, char *text, int tam);
void campoID(PGconn *conn, char *campo, char *text, int tam, const char *tabela);
void campoNormal(char *var, char *text, int tam);
int soDigitos(const char *str);
int checkCPF(char *cpf);
int checkPRECO(char *preco);
int checkDATA(char *d);