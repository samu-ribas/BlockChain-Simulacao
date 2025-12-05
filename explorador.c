#include<stdio.h>
#include<string.h>
#include<openssl/sha.h>
#include<stdlib.h>

#define DATA_SIZE 184
#define BLOCK_SIZE (sizeof(struct blocoNaoMin))

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

void imprimir_bloco_completo(blocoMin *b);

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

void imprimir_bloco_completo(blocoMin *b){
    printf("\n-------------------------------------------\n");
    printf("BLOCO %u\n", b->bloco.numero);
    printf("Nonce: %u\n", b->bloco.nonce);
    
    printf("Hash: ");
    for(int i=0; i<32; i++)
        printf("%02x", b->hash[i]);
    printf("\n");

    printf("Hash anterior: ");
    for(int i=0; i<32; i++)
        printf("%02x", b->bloco.hashAnterior[i]);
    printf("\n");

    printf("Minerador: %u (Recebeu +50 BTC)\n", b->bloco.data[183]);

    if (b->bloco.numero == 1)
        printf("Dados: %s\n", b->bloco.data);
    else{
        printf("Transacoes:\n");

        int tem_tx = 0;
        unsigned char o, d, v; //origem, destino e valor
        for(int k = 0; k < 183; k += 3) {
            o = b->bloco.data[k];
            d = b->bloco.data[k+1];
            v = b->bloco.data[k+2];
            //se encontrar 3 zeros para de ler 
            if(o == 0 && d == 0 && v == 0)
                break;
            
            printf("Origem:%3u -> Destino:%3u | Valor: %3u BTC]\n", o, d, v);
            tem_tx = 1;
        }
        if(!tem_tx)
            printf("Bloco vazio, sem transacoes\n");
    }
    printf("\n-------------------------------------------\n");
}