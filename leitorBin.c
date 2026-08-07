#include <stdio.h>
#include <openssl/sha.h>
 
#define DATA_SIZE 184

typedef struct blocoNaoMin{
    unsigned int numero;
    unsigned int nonce; 
    unsigned char data[DATA_SIZE]; 
    unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
}blocoNaoMin;

typedef struct blocoMin{
    blocoNaoMin bloco;
    unsigned char hash[SHA256_DIGEST_LENGTH];
}blocoMin;

int main()
{
    FILE *arqBin = fopen("blockchain.bin", "rb");
    if(!arqBin){
        printf("Erro, arquivo blockchain.bin não encontrado\n");
        return 1;
    }
    blocoMin bufferLeitura;
    int qtd_lida;

    /*
    lê um bloco por vez 
    ler 5 blocos como teste
    */
    for(int i = 0; i < 5; i++){
        qtd_lida = fread(&bufferLeitura, sizeof(blocoMin), 1, arqBin);
        if (qtd_lida != 1) {
            printf("Fim do arquivo ou erro de leitura.\n");
            break;
        }
        printf("Bloco %u | Nonce: %u | Minerador: %u\n", bufferLeitura.bloco.numero,bufferLeitura.bloco.nonce,bufferLeitura.bloco.data[183]);      
        printf("Hash: ");
        for(int i = 0; i < 32; i++)
            printf("%02x", bufferLeitura.hash[i]);
        printf("\n");
        printf("-----------------------------------------\n");
    }
    fclose(arqBin);
    return 0;
}