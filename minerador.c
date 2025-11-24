#include<stdio.h>
#include<string.h>
#include<openssl/crypto.h>
#include<openssl/sha.h>
#include "mtwister.h"

/* 61 transações x 3 bytes(origem, destino, valor) + 1 byte do minerador*/
#define DATA_SIZE 184
#define BLOCK_SIZE (sizeof(struct blocoNaoMin))
#define NUM_BLOCK 30000

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

/*protótipos*/
void gerar_transacao(blocoNaoMin *bnm, unsigned int *carteira, MTRand *r);
int verifica_carteira(unsigned int *carteira);
void imprimir_bloco_detalhado(blocoNaoMin *bnm, unsigned char *hash);
void imprimir_carteira_final(unsigned int *carteira);

int main()
{
	blocoNaoMin bnm;
	unsigned char hash[SHA256_DIGEST_LENGTH];
	unsigned char hashAnterior[SHA256_DIGEST_LENGTH];

	/*carteira de 256 usuários diferentes, identificados pelos endereços de 0 a 255*/
	unsigned int carteira[256] = {0};
	MTRand r = seedRand(1234567);

	/*BLOCO GÊNESIS*/
	bnm.numero = 1;
	bnm.nonce = 0;

	/*limpa o vetor com zeros*/
	for(int i = 0 ; i < DATA_SIZE ; i++)
    	bnm.data[i] = 0;

	strcpy((char *)bnm.data, "The Times 03/Jan/2009 Chancellor on brink of second bailout for banks");
	
	/*escolhe o minerador aleatório e colocar na última posição*/
	unsigned char minerador = (unsigned char)(genRandLong(&r) % 256);
	bnm.data[183] = minerador;

	/*zerando hash anterior do genesis*/
	for(int i = 0 ; i<SHA256_DIGEST_LENGTH ; i++)
		bnm.hashAnterior[i] = 0;

	printf("Minerando Bloco Gênesis...\n");
	do{
		SHA256((unsigned char *)&bnm, BLOCK_SIZE, hash);
		if(hash[0]!=0)
            bnm.nonce++;
	}while(hash[0]!=0);

	carteira[minerador] += 50;

	//imprimir_bloco_detalhado(&bnm, hash);

	memcpy(hashAnterior, hash, SHA256_DIGEST_LENGTH);


	/*LOOP PRINCIPAL*/
	unsigned char orig, dest, val;

	for(int i = 2 ; i <= NUM_BLOCK ; i++){
		memset(&bnm, 0, sizeof(blocoNaoMin));
		bnm.numero = i;
		bnm.nonce = 0;

		/*copia o hash do bloco anterior*/
		memcpy(bnm.hashAnterior, hashAnterior, SHA256_DIGEST_LENGTH);
		gerar_transacao(&bnm, carteira, &r);

		do{
			SHA256((unsigned char *)&bnm, BLOCK_SIZE, hash);
			if(hash[0]!=0)
            	bnm.nonce++;
		}while(hash[0]!=0);

		unsigned char minerador = bnm.data[183];
		carteira[minerador] += 50;

		/*lê o vetor data até encontrar zeros ou atingir o limite, cada transação são 3 bytes*/
		for(int k =0 ; k<183 ; k+=3){
			orig = bnm.data[k];
			dest = bnm.data[k+1];
			val = bnm.data[k+2];
			/*condição de parada*/
			if(orig == 0 && dest==0 && val==0)
				break;
			/*atualização da carteira real*/
			carteira[orig] -= val;
			carteira[dest] += val;
		}
		//imprimir_bloco_detalhado(&bnm, hash);

		/*atualiza hash anterior para o pŕoximo loop*/
		memcpy(hashAnterior, hash, SHA256_DIGEST_LENGTH);
	}

	//imprimir_carteira_final(carteira);
	return 0;
}

void gerar_transacao(blocoNaoMin *bnm, unsigned int *carteira, MTRand *r){
	/*
	Olhar quem tem dinheiro na carteira;
	Escolher aleatoriamente quantas transações fazer (0 a 61);
	Escolher valores e destinos;
	Debitar da carteira imediatamente para evitar gastar o mesmo dinheiro duas vezes no mesmo bloco;
	Preencher o vetor data.
	*/
	int i;
	unsigned int carteira_temp[256];

	/*zerando*/
	for(i=0 ; i<DATA_SIZE ; i++)
    	bnm->data[i] = 0;

    /*definir a quantidade de transações (0 a 61)*/
	int qtd_transacoes = genRandLong(r) % 62;

	/*verifica se tem saldo*/
	if(!verifica_carteira(carteira))
		qtd_transacoes = 0;

	/*cópia da carteira para controlar os gastos dentro deste bloco*/
	for(i = 0 ; i<256 ; i++)
		carteira_temp[i] = carteira[i];

	/*indice para escrever no vetor atual*/
 	int pos_atual = 0;

 	for(i=0 ; i<qtd_transacoes ; i++){
 		if(!verifica_carteira(carteira_temp)) 
            break;

 		unsigned char origem, destino, valor;

 		/*escolhe origem, deve ter saldo > 0*/
 		do{
 			origem = (unsigned char)genRandLong(r) % 256; 
 		}while(carteira_temp[origem] == 0);

 		/*escolhe o destinint verifica_carteira(unsigned int *carteira)o*/
 		destino = (unsigned char)(genRandLong(r) % 256);

 		/*escolhe o valor entre 0 e o saldo atual da carteira temporaria*/
 		unsigned int max_valor = carteira_temp[origem];
 		valor = (unsigned char)(genRandLong(r)%(max_valor + 1));
 		carteira_temp[origem] -= valor;

 		/*grava no vetor data*/
 		bnm->data[pos_atual++] = origem;
 		bnm->data[pos_atual++] = destino;
 		bnm->data[pos_atual++] = valor;
 	}
 	unsigned char minerador = (unsigned char)(genRandLong(r) % 256);
 	bnm->data[183] = minerador;
}

int verifica_carteira(unsigned int *carteira){
	/*
	percorre o vetor para ver se alguém tem saldo maior que zero.
	Se ninguém tiver, não é possível criar transações
	*/
	for(int i=0 ; i<256 ; i++)
		if(carteira[i] > 0) return 1;
	return 0;
}
/*
void imprimir_bloco_detalhado(blocoNaoMin *bnm, unsigned char *hash){
	printf("===============================================\n");
	printf("BLOCO %d (Nonce: %u)\n", bnm->numero, bnm->nonce);
	printf("Hash atual: ");
	for(int i=0 ; i<SHA256_DIGEST_LENGTH ; i++)
		printf("%02x", hash[i]);
	printf("\n");

	printf("Hash anterior: ");
	for(int i=0 ; i<SHA256_DIGEST_LENGTH ; i++)
		printf("%02x", bnm->hashAnterior[i]);
	printf("\n");
	printf("Minerador : %d\n", bnm->data[183]);

	/*se for gênesis imprime a string se não imprime transações*//*
	if(bnm->numero == 1)
		printf("Dados	: %s\n", bnm->data);
	else{
		printf("Transações:\n");
		int tem_tx = 0;
		for(int k=0 ; k<183; k+=3){
			unsigned char o = bnm->data[k];
            unsigned char d = bnm->data[k+1];
            unsigned char v = bnm->data[k+2];
            if(o == 0 && d == 0 && v == 0) break;

            printf("   [Orig:%3d -> Dest:%3d | $:%3d]\n", o, d, v);
            tem_tx = 1;
		}
		if(!tem_tx) printf("	(sem transações)\n");
	}
	printf("\n");
}
*/
/*
void imprimir_carteira_final(unsigned int *carteira){
	printf("\n=== SALDOS FINAIS (Enderecos > 0) ===\n");
    unsigned long total_btc = 0;
    for(int i = 0; i < 256; i++) {
        if(carteira[i] > 0) {
            printf("Endereço %3d: %5u BTC\n", i, carteira[i]);
            total_btc += carteira[i];
        }
    }
    printf("Total em circulacao: %lu BTC (Deveria ser %d)\n", total_btc, NUM_BLOCK * 50);
}
*/