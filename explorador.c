#include<stdio.h>
#include<string.h>
#include<openssl/sha.h>
#include<stdlib.h>

/* 61 transações x 3 bytes(origem, destino, valor) + 1 byte do minerador*/
#define DATA_SIZE 184
#define BLOCK_SIZE (sizeof(struct blocoNaoMin))
/*le 4 blocos de uma vez*/
#define FATOR_BLOCO 4

#define TAM_HASH_NONCE 4099 
#define TAM_HASH_ADRESS 256

/*estruturas*/
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

/* nó da lista encadeada carrega em RAM*/
typedef struct NoIndice{
    unsigned int chave; /*guarda o endereço ou o nonce*/
    unsigned int id_bloco;
    struct NoIndice *prox;
}NoIndice;

typedef struct Minerador{
    unsigned int saldo;
    unsigned int qtd_minerados;
}NoMinerador;

/* Registro genérico */
typedef struct{
    unsigned char chave;
    unsigned int id_bloco;
}RegistroIndice;

/*struct auxiliar para fazer a ordenação*/
typedef struct{
    blocoMin b;
    int qtd;
}BlocoOrdenavel;

/*declarando globalmente as tabelas usadas*/
NoIndice* tabela_endereco[TAM_HASH_ADRESS] = {NULL};
NoIndice* tabela_nonce[TAM_HASH_NONCE] = {NULL}; 
NoMinerador carteira_minerador[TAM_HASH_ADRESS] = {0}; /*Hash de Endereçamento Direto (onde o índice 10 é o minerador 10)*/

/*flags de controle*/
int g_carregado = 0;
int i_carregado = 0;
int ab_carregado = 0;
int total_blocos_lidos = 0;

/*funções auxiliares*/
void imprimir_bloco_completo(blocoMin *b);
int contar_transações(blocoMin *bloco);
int transacoes_crescente(const void *a, const void *b);
void insere_tabela(NoIndice **tabela, int tamanho_tabela, unsigned int chave, unsigned int id);
void carregar_indice(char tipo, FILE *arqBlockchain);
void carregar_carteira(FILE *arq);
/*funções principais em ordem (a,b,c,d,e,f,g,h,i)*/
void opcao_A(FILE *arq);
void opcao_B(FILE *arq);
void media_bitcoins(FILE *arq);
void buscar_bloco_f(FILE *arq);
void consulta_g(FILE *arq);
void imprimir_n_primeiro_ordenados(FILE *arq);
void consulta_i(FILE *arq);

/*main e menu*/
int main()
{
    char opcao;
    FILE *arqBin = fopen("blockchain.bin", "rb"); /*abrindo arq. bin. dos blocos*/

    if(!arqBin){
        perror("Não consegui abrir o arquivo\n");
        return 1;
    }

    do{
        printf("\n--------------MENU BLOCKCHAIN--------------\n");
        printf("a) Endereco com mais bitcoins\nb) Endereco que minerou mais blocos\nc) Hash do bloco com mais transações\nd) Hash do bloco com menos transações\ne) Quantidade media de bitcoins por bloco\nf) Imprimir bloco por numero\ng) Imprimir n primeiros blocos por endereco\nh) Imprimir n primeiros blocos ordenador por transações\ni) Imprimir blocos por Nonce\nx) Sair\nEscolha uma opção: ");
        scanf(" %c", &opcao);

        switch (opcao)
        {
            case 'a':
            case 'A':
                opcao_A(arqBin);
                break;

            case 'b':
            case 'B':
                opcao_B(arqBin);
                break;

            case 'c':
            case 'C':
                
                break;

            case 'd':
            case 'D':
                
                break;

            case 'e':
            case 'E':
                media_bitcoins(arqBin);
                break;

            case 'f':
            case 'F':
                buscar_bloco_f(arqBin);
                break;

            case 'g':
            case 'G':
                consulta_g(arqBin);
                break;

            case 'h':
            case 'H':
                imprimir_n_primeiro_ordenados(arqBin);
                break;

            case 'i':
            case 'I':
                consulta_i(arqBin);
                break;

            case 'x':
            case 'X':
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
    printf("\n--------------------------------------------------------------------------------------\n");
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

        int tem_transacao = 0;
        unsigned char o, d, v; //origem, destino e valor
        for(int k = 0; k < 183; k += 3) {
            o = b->bloco.data[k];
            d = b->bloco.data[k+1];
            v = b->bloco.data[k+2];
            //se encontrar 3 zeros para de ler 
            if(o == 0 && d == 0 && v == 0)
                break;
            
            printf("\t\tOrigem: %3u -> Destino: %3u | Valor: %3u BTC\n", o, d, v);
            tem_transacao = 1;
        }
        if(!tem_transacao)
            printf("Bloco vazio, sem transacoes\n");
    }
}

int contar_transações(blocoMin *b){
    /*função auxiliar para contar transações*/
    if(b->bloco.numero == 1)// bloco genêsis
        return 0;

    int contador = 0;
    unsigned char o, d, v;
    for(int k = 0 ; k<183 ; k += 3){
        o = b->bloco.data[k];
        d = b->bloco.data[k+1];
        v = b->bloco.data[k+2];
        if(o == 0 && d == 0 && v == 0)
            break;
        contador++;
    }
    return contador;
}

int transacoes_crescente(const void *a, const void *b){
/*
    função comparadora para o quick sort
    retorna <0 se a<b, 0 se a=b , >0 se a>b
*/
    BlocoOrdenavel *ba = (BlocoOrdenavel *)a;
    BlocoOrdenavel *bb = (BlocoOrdenavel *)b;
    return (ba->qtd - bb->qtd);
}

void insere_tabela(NoIndice **tabela, int tamanho_tabela, unsigned int chave, unsigned int id){
    /*inserindo na hash como tratamento de colisao por encadeamento*/
    int idx = chave % tamanho_tabela;
    NoIndice *novo = NULL;
    novo = (NoIndice *)malloc(sizeof(NoIndice));
    if(!novo)
        return;
    novo->chave = chave;
    novo->id_bloco = id;
    novo->prox = tabela[idx];
    tabela[idx] = novo;
}

void carregar_indice(char tipo, FILE *arqBlockchain){
    /*a função recebe ou tipo I OU tipo G e configura os ponteiros*/
    /* Ponteiros auxiliares para configurar o comportamento da função*/
    NoIndice **tabela;
    int tamanho;
    char *nome_arquivo;
    int *flag_carregado;

    /*baseada no tipo*/
    if(tipo == 'G')
    {
        if(g_carregado)
            return; /*já carregou*/
        tabela = tabela_endereco;
        tamanho = TAM_HASH_ADRESS;
        nome_arquivo = "indice_minerador.bin";
        flag_carregado = &g_carregado;
    } 
    else 
    { 
        if(i_carregado)
            return;
        tabela = tabela_nonce;
        tamanho = TAM_HASH_NONCE;
        nome_arquivo = "indice_nonce.bin";
        flag_carregado = &i_carregado;
    }

    FILE *arqIndice = fopen(nome_arquivo, "rb");

    /*arquivo existe, carregar do disco*/
    if(arqIndice){
        printf("Carregando indice %c do disco...\n", tipo);
        RegistroIndice reg;
        while(fread(&reg, sizeof(RegistroIndice), 1, arqIndice)){
            insere_tabela(tabela, tamanho, reg.chave, reg.id_bloco);
        }
        fclose(arqIndice);
    } 
    /*arquivo não existe, ler blockchain e criar*/
    else{
        printf("Criando indice %c a partir da blockchain...\n", tipo);
        arqIndice = fopen(nome_arquivo, "wb");
        
        blocoMin b;
        RegistroIndice reg;
        
        rewind(arqBlockchain);
        while(fread(&b, sizeof(blocoMin), 1, arqBlockchain)){
            /* decide ual campo extrair */
            unsigned int chave_atual = (tipo == 'G') ? b.bloco.data[183] : b.bloco.nonce;
            
            insere_tabela(tabela, tamanho, chave_atual, b.bloco.numero);

            /*gravar no disco*/
            reg.chave = chave_atual;
            reg.id_bloco = b.bloco.numero;
            fwrite(&reg, sizeof(RegistroIndice), 1, arqIndice);
        }
        fclose(arqIndice);
    }

    *flag_carregado = 1; /*marca como pronto*/
    printf("Indice %c pronto para uso\n", tipo);
}

void carregar_carteira(FILE *arq){
/*
essa função só faz o trabalho pesado se for necessário, ou seja
somente a primeira vez para carregar os dados do minerador para RAM
(saldo e quantidade minerada)
*/
    /*confere se ja carregou os dados se sim, retorna*/
    if(ab_carregado)
        return;
    printf("\n[Carregando dados do disco para a RAM...]\n");
    /*aloca e preenche tudo com zeros */
    blocoMin b;
    rewind(arq);
    unsigned char minerador, orig, dest, val;
    while(fread(&b, sizeof(blocoMin), 1, arq)){
        total_blocos_lidos++;
        /*processa os endereços para extrair o saldo e a quantidade minerada*/
        minerador = b.bloco.data[183];
        carteira_minerador[minerador].saldo += 50;
        carteira_minerador[minerador].qtd_minerados++;
        /*processa transações para saber quem tem mais transações e quem tem menos*/
        if(b.bloco.numero > 1){
            /*campo data tem 183 bytes + 1 do minerador*/
            for(int k = 0; k < 183; k += 3){
                 orig = b.bloco.data[k];
                 dest = b.bloco.data[k+1];
                 val  = b.bloco.data[k+2];

                if(orig == 0 && dest == 0 && val == 0)
                    break;
                /*atualiza o saldo dos endereços envolvidos*/
                carteira_minerador[orig].saldo -= val;
                carteira_minerador[dest].saldo += val;
            }
        }
    }
    /*muda flag para carregado*/
    ab_carregado = 1;
    printf("Dados carregados!\n");
}

/*função A, maior saldo BTC */
void opcao_A(FILE *arq){
    /*calcula o maior saldo, depois num laço for considera possivel empate*/
    carregar_carteira(arq);
    unsigned int maior_saldo = 0, i;

    /*busca quem tem mais bitcoins, ou seja, maior valor*/
    for(i=0 ; i<TAM_HASH_ADRESS ; i++){
        if(carteira_minerador[i].saldo > maior_saldo)
            maior_saldo = carteira_minerador[i].saldo;
    }
    printf("\n===========ENDEREÇO(S) COM MAIS BITCOINS (%u BTC)===========\n", maior_saldo);
    for(i=0 ; i<TAM_HASH_ADRESS ; i++){
        if(carteira_minerador[i].saldo == maior_saldo)
            printf("-> Endereço: %d\n", i);
    }
}

/*função B, quem mais minerou*/
void opcao_B(FILE *arq){
     /*calcula quem tem o maior numero de blocos minerados, depois num laço for considera possivel empate*/
    carregar_carteira(arq);
    unsigned int maior_mineracao = 0, i;

    /*busca quem minerou mais blocos*/
    for(i=0 ; i<TAM_HASH_ADRESS ; i++){
        if(carteira_minerador[i].qtd_minerados > maior_mineracao)
            maior_mineracao = carteira_minerador[i].qtd_minerados;
    }
    printf("\n===========ENDEREÇO(S) QUE MAIS MINERARAM BLOCOS (%u blocos)===========\n", maior_mineracao);
    for(i=0 ; i<TAM_HASH_ADRESS ; i++){
        if(carteira_minerador[i].qtd_minerados == maior_mineracao)
            printf("-> Endereço: %d\n", i);
    }  
}

/*função E, calcula média de bitcoins por bloco*/
void media_bitcoins(FILE *arq){
    carregar_carteira(arq);
    if(total_blocos_lidos == 0){
        printf("\nNenhum bloco encontrado.");
        return;
    }
    unsigned long total_btc = 0;
    for(int i = 0; i < 256; i++){
        total_btc += carteira_minerador[i].saldo;
    }
    double media = (double)total_btc/total_blocos_lidos;
    printf("\n=========== Quantidade Media de Bitcoins por Bloco ===========\n");
    printf("Total em Circulacao: %lu BTC\n", total_btc);
    printf("Total de Blocos: %u\n", total_blocos_lidos);
    printf("Media: %.2f BTC/bloco\n", media);
    printf("================================================================\n");
}

/*função F, busca bloco, dado um n*/
void buscar_bloco_f(FILE *arq){
    unsigned int num;
    /*buffer na ram*/
    blocoMin buffer_pagina[FATOR_BLOCO];

    printf("Digite o numero do bloco: ");
    scanf("%u", &num);

    if(num < 1){
        printf("Numero invalido.\n");
        return;
    }

    /*
    para reduzir o número de acessos ao disco, vamos ler um pagina 
    para a memória de uma vez
    */
    long pagina_id = (num - 1)/FATOR_BLOCO;/* ex: bloco 5. (5-1)/4 = pag. 1*/

    long offset = pagina_id * FATOR_BLOCO * sizeof(blocoMin); /*num. da pag. * tam. da pag.*/

    /* posiciona a cabeça de leitura, se retornar -1 significa que falhou*/
    if(fseek(arq, offset, SEEK_SET) != 0){
        printf("Erro: Bloco fora dos limites do arquivo.\n");
        return;
    }

    /* le o registro inteiro daquela posição */
    size_t lidos = fread(buffer_pagina, sizeof(blocoMin), FATOR_BLOCO, arq);
    /*calcula o exato bloco que é para ser lido*/
    int indice_no_buffer = (num - 1) % FATOR_BLOCO;

    if(indice_no_buffer<lidos)
        imprimir_bloco_completo(&buffer_pagina[indice_no_buffer]);
    else
        printf("Erro: Bloco %u não existe\n", num);
}

/*função G, busca n primeiros blocos de um endereço*/
void consulta_g(FILE *arq){
/*
    irá printar em ordem do bloco mais recente minerado pelo endereço 
    pela maneria que foi inserido na lista encadeada(no inicio)
*/
    /*parâmetro usados*/
    int minerador_input, n;
    
    carregar_indice('G', arq);

    /* entrada de dados */
    printf("\nDigite o endereco do minerador (0-255): ");
    scanf("%d", &minerador_input);
    printf("Quantos blocos deseja ver: ");
    scanf("%d", &n);

    if(minerador_input < 0 || minerador_input > 255){
        printf("Endereco invalido.\n");
        return;
    }

    /* acesso direto à tabela Hash */
    /* a chave é o próprio endereço */
    NoIndice *atual = tabela_endereco[minerador_input];

    if(!atual){
        printf("Nenhum bloco encontrado para o minerador %d\n", minerador_input);
        return;
    }

    blocoMin b;
    int cont = 0;
    /*varredura da lista encadeada*/
    while(atual && cont < n){
        unsigned int id = atual->id_bloco;
        long offset = (long)(id - 1) * sizeof(blocoMin);
        /*busca no disco*/
        fseek(arq, offset, SEEK_SET);
        if(fread(&b, sizeof(blocoMin), 1, arq) == 1){
            imprimir_bloco_completo(&b);
        }
        atual = atual->prox;
        cont++;
    }
    if(cont<n){
        printf("\tFIM DA LISTA: encontrados apenas %d blocos\n", cont);
    }
}

/*função H, n primeiros ordenado por quant. de transações*/
void imprimir_n_primeiro_ordenados(FILE *arq){
    unsigned int n, lidos = 0;
    printf("Digite a quantidade N de blocos para analisar: ");
    scanf("%u", &n);

    if(n < 0){
        printf("Numero invalido.\n");
        return;
    }

    BlocoOrdenavel *vetor = (BlocoOrdenavel *)malloc(n * sizeof(BlocoOrdenavel));
    if(!vetor){
        printf("Erro: Memória insuficiente na RAM.\n");
        return;
    }
    /*reseta o cursor de leitura*/
    rewind(arq);

    for(int i = 0 ; i<n ; i++){
        if(fread(&vetor[i].b, sizeof(blocoMin), 1, arq) != 1)
            break; /*o arquivo acabou antes de chegar em N*/
        /*faz calculo da contagem e guarda na struct*/
        vetor[i].qtd = contar_transações(&vetor[i].b);
        lidos++;
    }
    qsort(vetor, lidos, sizeof(BlocoOrdenavel), transacoes_crescente);

    printf("---------PRIMEIROS %u BLOCOS ORDENADOS POR TRANSAÇÕES---------\n", lidos);
    for(int i=0 ; i<lidos ; i++){
        printf("\nQuantidade de transações: %d", vetor[i].qtd);
        imprimir_bloco_completo(&vetor[i].b);
    }
    free(vetor);
}

/*função I, busca bloco dado um nonce*/
void consulta_i(FILE *arq){
    unsigned int nonce_buscado;
    carregar_indice('I', arq);
    printf("Digite o nonce: ");
    scanf("%u", &nonce_buscado);

    int idx = nonce_buscado % TAM_HASH_NONCE;
    NoIndice *atual = tabela_nonce[idx];
    if(!atual){
        printf("O nonce não foi encontrado.\n");
        return;
    }

    printf("\n----------Blocos com Nonce %u----------\n", nonce_buscado);
    blocoMin b;
    int encontrou = 0;

    while(atual){
        /*verifica se é o nonce buscado*/
        if(atual->chave == nonce_buscado){
            fseek(arq, (long)(atual->id_bloco - 1) * sizeof(blocoMin), SEEK_SET);
            fread(&b, sizeof(blocoMin), 1, arq);
            imprimir_bloco_completo(&b);
            encontrou = 1;
        }
        atual = atual->prox;
    }
    if(!encontrou)
        printf("O nonce não foi encontrado. Apenas colisões.\n");
}

