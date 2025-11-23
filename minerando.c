#include <stdio.h>
#include <openssl/crypto.h>
#include <openssl/sha.h>
#include "mtwister.h"

#define DATA_SIZE 1008
#define BLOCK_SIZE (sizeof(struct blocoNaoMinerado))


typedef struct blocoNaoMinerado
{
	unsigned int numero;
	unsigned int nonce; 
	unsigned char data[DATA_SIZE];
	unsigned char hashAnterior[SHA256_DIGEST_LENGTH];
}blocoNaoMin;

typedef struct blocoMinerado
{
	blocoNaoMin bloco;
	unsigned char hash[SHA256_DIGEST_LENGTH];
}blocoMin;





int main()
{
	blocoNaoMin bnm;
	blocoNaoMin bufferRAM[4];
	unsigned int nonce = 0;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned char carteira[256];


	bnm.numero = 1;

	for(int i = 0 ; i<SHA256_DIGEST_LENGTH ; i++) // bloco genêsis zerado
		bnm.hashAnterior[i] = 0x00;


	printf("\nInicializando mineração...\n");

	// aqui deve ser feito a lógica da transação

	if(bnm.numero)

	for(int i = 0 ; i<DATA_SIZE ; i++)
		bufferRAM[]

	do
	{
		bnm.nonce = nonce;
		SHA256((const unsigned char *)&bnm, BLOCK_SIZE, hash);
		nonce++;
	}
	while(hash[0]!=0x00);

	printf("\nBloco minerado com sucesso.\n");
	printf("Nonce: %u\n", bnm.nonce);
	printf("Hash[0] = %02x\n", hash[0]);

	return 0;
}