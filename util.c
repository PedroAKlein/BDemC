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
    printf(" 4 - Organizacao\n");
    printf(" 5 - Registro\n");
    printf(" 6 - Relatorio\n");
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
    
        scanf("%d", &opt);
        getchar();
    
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
    
        scanf("%d", &opt);
        getchar();
    
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
    
        scanf("%d", &opt);
        getchar();
    
        switch(opt){
            case tab_cliente: alterarCliente(conn); break;
            case tab_produto: alterarProduto(conn); break;
            case tab_venda: alterarVenda(conn); break;
            case SAIR: return;
    
            default: printf("Opcao Invalida!\n"); break;
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

// retorna 1 se tiver os digitos especiais, 0 caso contrario
int checkCPF(char *cpf){
    if(cpf[0] == '\0' || cpf[0] == '\n')
        return 1;
    
    if(cpf[3] != '.')
        return 0;

    if(cpf[7] != '.')
        return 0;

    if(cpf[11] != '-')
        return 0;

    return 1;
}

// retorna 1 se tiver o digito especial, 0 caso contrario
int checkPRECO(char *preco){
    if(preco[0] == '\0') 
        return 1;

    int ponto = 0;
      
    for(char *p = preco; *p; p++){
        if(*p == '.'){ 
            if(ponto) 
                return 0; 
            
            ponto = 1; 

        } else if(*p < '0' || *p > '9') 
            return 0;
    }
    return 1;
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