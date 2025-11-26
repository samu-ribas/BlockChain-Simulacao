#include<stdio.h>
#include<string.h>
#include<openssl/sha.h>
#include<stdlib.h>

#define DATA_SIZE 184

typedef struct blocoNaoMin
{
    unsigned int numero;
    unsigned int nonce;
    unsigned char data[DATA_SIZE];
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
}blocoNaoMin;

typedef struct blocoMin
{
	blocoNaoMin bloco;
	unsigned char hash[SHA256_DIGEST_LENGTH];
}blocoMin;


int main()
{
    FILE *arqBin = fopen("blockchain.bin", "rb");
    if(!arqBin){
        printf("Erro ao abrir o arquivo\n");
        return 1;
    }
    char opcao;
    do{
        printf("--------------MENU BLOCKCHAIN--------------\n");
        printf("a) Endereco com mais bitcoins\nb) Endereco que minerou mais blocos\nc) Hash do bloco com mais transações\nd) Hash do bloco com menos transações\ne) Quantidade media de bitcoins por bloco\nf) Imprimir bloco por numero\ng) Imprimir n primeiros blocos por endereco\nh) Imprimir n primeiros blocos\ni) Imprimir blocos por Nonce\nx) Sair\nEscolha uma opção: ");
        scanf("%c", &opcao);
        switch (opcao)
        {
            case 'a':
            case 'A':
                
                break;
            case 'b':
            case 'B':
                
                break;
            case 'c':
            case 'C':
                
                break;
            case 'd':
            case 'D':
                
                break;
            case 'e':
            case 'E':
            
                break;
            case 'f':
            case 'F':
            
                break;
            case 'g':
            case 'G':
            
                break;
            case 'i':
            case 'I':
    
                break;
            case 'x':
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida!\n");
        }
    }while(opcao!='x');
    fclose(arqBin);
    return 0;
}