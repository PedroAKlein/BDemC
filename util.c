#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "func.h"
#include "util.h"
#include "PG.h"

void menu(){
    printf("\n----== MENU ==----\n");
    printf(" 1 - Cadastro\n");
    printf(" 2 - Consulta\n");
    printf(" 3 - Alteracao\n");
    printf(" 4 - Relatorio\n");
    printf(" 0 - Sair\n");
    printf("\n");
    printf("Opcao: ");
}

void menuCoringa(char *titulo){
    printf("\n----== %s ==----\n", titulo);
    printf(" Selecione uma tabela:\n");
    printf("  1 - Clientes\n");
    printf("  2 - Produtos\n");
    printf("  3 - Vendas\n");
    printf(" 0 - Sair\n");
    printf("\n");
    printf("Opcao: ");
}

void consulta(PGconn *conn){
    int opt = -1;

    while(opt != 0){
        menuCoringa("CONSULTA");
    
        opt = lerOpcao();
    
        switch(opt){
            case tab_cliente: imprimeTabela(conn, "cliente"); break;
            case tab_produto: imprimeTabela(conn, "produto"); break;
            case tab_venda: imprimeVendas(conn); break;
            case SAIR: return;
    
            default: printf("Opcao Invalida!\n"); break;
        }
    }
}

void cadastro(PGconn *conn){
    int opt = -1;

    while(opt != 0){
        menuCoringa("CADASTRO");
    
        opt = lerOpcao();
    
        switch(opt){
            case tab_cliente: cadastrarCliente(conn); break;
            case tab_produto: cadastrarProduto(conn); break;
            case tab_venda: cadastrarVenda(conn); break;
            case SAIR: return;
    
            default: printf("Opcao Invalida!\n"); break;
        }
    }
}

void alteracao(PGconn *conn){
    int opt = -1;

    while(opt != 0){
        menuCoringa("ALTERACAO");
    
        opt = lerOpcao();
    
        switch(opt){
            case tab_cliente: alterarCliente(conn); break;
            case tab_produto: alterarProduto(conn); break;
            case tab_venda: alterarVenda(conn); break;
            case SAIR: return;
    
            default: printf("Opcao Invalida!\n"); break;
        }
    }
}

void relatorio(PGconn *conn){
    int opt = -1;

    while(opt != 0){
        printf("\n----== RELATORIO ==----\n");
        printf(" Selecione uma opcao:\n");
        printf("  1 - Relatorio de Clientes\n");
        printf("  2 - Relatorio de Produtos\n");
        printf("  3 - Relatorio de Vendas\n");
        printf(" 0 - Sair\n");
        printf("\n");
        printf("Opcao: ");

        opt = lerOpcao();

        switch(opt){
            case 1: rel_cliente(conn); break;
            case 2: rel_produto(conn); break;
            case 3: rel_venda(conn); break;
            case SAIR: return;
            default: printf("\nOpcao invalida!\n"); break;
        }
    }
}

void remove_quebraLinha(char *str){
    size_t len = strlen(str);
    if(len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

// se a string for vazia, seu valor sera NULL
char *checkString(char *str){
    return (str[0] == '\0' || str[0] == '\n') ? NULL : str;
}

void campoObrigatorio(char *var, char *text, int tam){
    while(1){
        printf("\n%s: ", text);
        fgets(var, tam, stdin);

        if(var[0] != '\n')
            break;

        printf("%s eh obrigatorio!\n", text);
    }
    remove_quebraLinha(var);
}

void campoID(PGconn *conn, char *var, char *text, int tam, const char *tabela){
    while(1){
        campoObrigatorio(var, text, tam);

        if(strcmp(var, "0") == 0)
            return;

        if(strcmp(var, "help") == 0){
            printf("\n");
            
            if(strcmp(tabela, "cliente") == 0)
                imprime(conn, "SELECT id, nome, cpf, telefone FROM cliente ORDER BY nome");

            else if(strcmp(tabela, "produto") == 0)
                imprime(conn, "SELECT id, descricao, estoque FROM produto ORDER BY descricao");

            else
                imprime(conn, "SELECT id, data FROM venda ORDER BY data");

            continue;
        }

        if(!soDigitos(var)){
            printf("\nO ID deve possuir apenas digitos!\n");
            continue;
        }
        
        if(!existeId(conn, tabela, var)){
            printf("\nO ID nao existe!\n");
            printf("Digite \"help\" para ver a tabela de %ss\n", tabela);
            continue;
        }

        break;
    }
}

void campoNormal(char *var, char *text, int tam){
    printf("\n%s: ", text);
    fgets(var, tam, stdin);
    remove_quebraLinha(var);
}

// retorna 1 se for apenas digitos, 0 caso contrario
int soDigitos(const char *str){
    if(str[0] == '\0' || str[0] == '\n') 
        return 1;

    for (; *str; str++)
        if (*str < '0' || *str > '9') 
            return 0;

    return 1;
}

// retorna 1 se o CPF estiver vazio (campo pulado) ou no formato 111.111.111-11
int checkCPF(char *cpf){
    if(cpf[0] == '\0' || cpf[0] == '\n')
        return 1;

    if(strlen(cpf) != 14)
        return 0;

    for(int i = 0; i < 14; i++){
        if(i == 3 || i == 7){
            if(cpf[i] != '.')
                return 0;

        } else if(i == 11){
            if(cpf[i] != '-')
                return 0;

        } else if(cpf[i] < '0' || cpf[i] > '9')
            return 0;
    }

    return 1;
}

// retorna 1 se for vazio ou um preco valido: so digitos e no maximo um ponto,
// ate 8 digitos antes do ponto, ate 2 depois e valor maior que zero
int checkPRECO(char *preco){
    if(preco[0] == '\0')
        return 1;

    int inteiros = 0, decimais = 0, ponto = 0, positivo = 0;

    for(char *p = preco; *p; p++){
        if(*p == '.'){
            if(ponto)
                return 0;

            ponto = 1;

        } else if(*p >= '0' && *p <= '9'){
            if(ponto)
                decimais++;
            else
                inteiros++;

            if(*p != '0')
                positivo = 1;

        } else
            return 0;
    }

    if(inteiros == 0 && decimais == 0)
        return 0;

    if(inteiros > 8 || decimais > 2)
        return 0;

    return positivo;
}

int checkDATA(char *d){
    if(d[0] == '\0') 
        return 1;

    if(strlen(d) != 10) 
        return 0;

    for(int i = 0; i < 10; i++){
        if(i == 4 || i == 7){ 
            if(d[i] != '-') 
                return 0; 

        } else if(d[i] < '0' || d[i] > '9') 
            return 0;
    }
    return 1;
}

// retorna 1 se for vazio ou so digitos (ate 9, para caber em INT)
int checkINT(char *str){
    if(str[0] == '\0')
        return 1;

    return soDigitos(str) && strlen(str) <= 9;
}

// retorna 1 se for vazio ou so digitos (ate 20, tamanho da coluna)
int checkTEL(char *tel){
    if(tel[0] == '\0')
        return 1;

    return soDigitos(tel) && strlen(tel) <= 20;
}

// as funcoes "ler" repetem a pergunta ate o valor ser valido
// retornam 0 se o usuario digitar 0 (cancelar)
int lerCPF(char *cpf){
    while(1){
        campoObrigatorio(cpf, "CPF (0 para sair)", CPF);

        if(strcmp(cpf, "0") == 0)
            return 0;

        if(checkCPF(cpf))
            return 1;

        printf("\nSiga o exemplo, nao se esqueca dos \'.\' e do \'-\'!\n");
        printf("Exemplo: 111.111.111-11\n");
    }
}

int lerTelefone(char *tel){
    while(1){
        campoObrigatorio(tel, "Telefone (0 para sair)", TEL);

        if(strcmp(tel, "0") == 0)
            return 0;

        if(checkTEL(tel))
            return 1;

        printf("\nApenas numeros (ate 20 digitos)!\n");
        printf("Exemplo: 55123456789\n");
    }
}

int lerPreco(char *preco){
    while(1){
        campoObrigatorio(preco, "Preco (0 para sair)", PRECO);

        if(strcmp(preco, "0") == 0)
            return 0;

        if(checkPRECO(preco))
            return 1;

        printf("\nPreco invalido! Use ate 8 digitos, ponto e ate 2 casas decimais, maior que zero.\n");
        printf("Exemplo: 12.50\n");
    }
}

void lerEstoque(char *estoq){
    while(1){
        campoObrigatorio(estoq, "Estoque (0 se nao houver)", ESTOQ);

        if(checkINT(estoq))
            return;

        printf("\nApenas numeros (ate 9 digitos)!\n");
    }
}

// le uma linha inteira e converte para numero (ate 4 digitos)
// retorna -1 se a linha for vazia, tiver letras/sinais ou for grande demais
// retorna 0 (sair) se a entrada acabar (EOF), para os menus nao ficarem em loop
int lerOpcao(){
    char buf[16];

    if(fgets(buf, sizeof(buf), stdin) == NULL)
        return 0;

    if(strchr(buf, '\n') == NULL){
        int c;
        while((c = getchar()) != '\n' && c != EOF);
        return -1;
    }

    remove_quebraLinha(buf);

    if(buf[0] == '\0' || !soDigitos(buf) || strlen(buf) > 4)
        return -1;

    return atoi(buf);
}

// le uma data ate estar no formato aaaa-mm-dd
// retorna 1 se leu uma data valida, 0 se o usuario digitou 0
int lerData(char *var, char *texto){
    while(1){
        campoObrigatorio(var, texto, DATA);

        if(strcmp(var, "0") == 0)
            return 0;

        if(checkDATA(var))
            return 1;

        printf("\nSiga o exemplo, nao se esqueca dos \'-\'!\n");
        printf("Exemplo: aaaa-mm-dd\n");
    }
}

// le a data inicial e a final
// retorna 1 se o periodo e valido, 0 se o usuario cancelou
int lerPeriodo(char *d1, char *d2){
    printf("\nExemplo: aaaa-mm-dd (0 para sair)\n");

    if(!lerData(d1, "Data inicial"))
        return 0;

    if(!lerData(d2, "Data final"))
        return 0;

    // o formato aaaa-mm-dd ordena igual em texto e em calendario
    if(strcmp(d1, d2) > 0){
        printf("\nA data inicial nao pode ser maior que a final!\n");
        return 0;
    }

    return 1;
}
