#include<stdio.h>
#include<string.h>
#include<openssl/crypto.h>
#include<openssl/sha.h>
#include "mtwister.h"

/* tamanho exato exigido 61 transações x 3 bytes (origem, destino, valor) + 1 byte do minerador*/
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

int main()
{
	blocoNaoMin bnm;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	/*carteira de 256 usuários diferentes, identificados pelos endereços de 0 a 255*/
	unsigned int carteira[256] = {0};
	MTRand r = seedRand(1234567);


	bnm.numero = 1;
	bnm.nonce = 0;

	/*limpa o vetor com zeros*/
	memset(bnm.data, 0, 184);
	/*copia a string e o resto fica zerado*/
	strcpy((char *)bnm.data, "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks");
	/*escolher o minerador aleatório e colocar na última posição*/
	unsigned char minerador = (unsigned char)(genRandLong(&r) % 256);
	bnm.data[183] = minerador;

	/*zerando bloco genesis, SHA256_DIGEST_LENGTH vale 32*/
	for(int i = 0 ; i<SHA256_DIGEST_LENGTH ; i++)
		bnm.hashAnterior[i] = 0;

	printf("Minerando Bloco Gênesis para o minerador %d...\n", minerador);

	do
	{
		SHA256((unsigned char *)&bnm, BLOCK_SIZE, hash);
		if(hash[0] != 0)
            bnm.nonce++;
	}
	while(hash[0]!=0);

	printf("\nBloco minerado com sucesso.\n");
	printf("Nonce: %d\n", bnm.nonce);
	printf("Hash[0] = %02x\n", hash[0]);

	/*recompensa de 50 bitcoins na carteira*/
	carteira[minerador] += 50;
	printf("Saldo do minerador %d agora é: %d BTC\n", minerador, carteira[minerador]);
	return 0;
}