#include <stdio.h>
#include <string.h>
#include <ctype.h> //Função tolower()
#include <stdlib.h>
#include <stdio_ext.h>

#define TAM 26 //Tamanho da tabela hash

typedef char* cstring; //Uso mais fácil de um vetor de char;

//-------------FUNÇÕES DE LISTA-------------// 
typedef struct tipo_item{
	char *palavra;
	int qntd;
}item;

typedef struct tipo_celula{
	struct tipo_item dado;
	struct tipo_celula *prox;
	struct tipo_celula *ant;
}celula;

typedef struct tipo_lista{
	struct tipo_celula *primeiro;
}lista;

void inicializa_lista(lista *l){
	l->primeiro = (celula *)malloc(sizeof(celula));
	l->primeiro->prox=l->primeiro;
	l->primeiro->ant=l->primeiro;
}

int vazia(lista *l){
	return l->primeiro->prox == l->primeiro;
}

celula* buscar_lista(lista *l, cstring palavra){
	celula *ptr = l->primeiro->prox;

	while((ptr!=l->primeiro) && (strcmp(ptr->dado.palavra, palavra))){
		ptr=ptr->prox;
	}
	if(ptr!=l->primeiro){
		return ptr;
	}else{
		return NULL;
	}
}

int insere_ultimo(lista *l, cstring nova_palavra){
	celula *novo;
	celula *ret_busca = buscar_lista(l, nova_palavra);
	if(ret_busca == NULL){
		novo = (celula *)malloc(sizeof(celula));
		novo->dado.palavra = (char*)malloc(sizeof(char)*strlen(nova_palavra));
		strcpy(novo->dado.palavra, nova_palavra);
		novo->dado.qntd = 1;
		novo->prox=l->primeiro;
		novo->ant=l->primeiro->ant;
		l->primeiro->ant->prox=novo;
		l->primeiro->ant=novo;
		return 1;
	}else{
		ret_busca->dado.qntd++;
		return 0;
	}
}

void insere_ultimo_qntd(lista *l, cstring nova_palavra, int qntd){
	celula *novo;

	novo = (celula *)malloc(sizeof(celula));
	novo->dado.palavra = (char*)malloc(sizeof(char)*strlen(nova_palavra));
	strcpy(novo->dado.palavra, nova_palavra);
	novo->dado.qntd = qntd;
	novo->prox=l->primeiro;
	novo->ant=l->primeiro->ant;
	l->primeiro->ant->prox=novo;
	l->primeiro->ant=novo;
}

int buscar_valor_lista(lista *l, cstring palavra, struct tipo_item *x){
	celula *c = buscar_lista(l,palavra);

	if(c!=NULL){
		*x=c->dado;
		return 1;
	}else{
		return 0;
	}
}

void escreve_lista(struct tipo_lista *l){
	celula *ptr=l->primeiro->prox;
	while(ptr!=l->primeiro){
		printf("%s-", ptr->dado.palavra);
		ptr=ptr->prox;
	}
	printf("\n");
}

void escreve_mais_frequentes(lista* l){
	celula *aux = l->primeiro->prox;

	while(aux != l->primeiro){
		printf("Palavra: %s\nQntd: %d\n", aux->dado.palavra, aux->dado.qntd);
		aux = aux->prox;
		printf("\n");
	}
	printf("\n");
}

//-----------------------------------------//

//------------------HASH------------------//

//Parâmetro para funcao_hash: Código ascii da primeira letra em minúsculo da palavra

typedef struct tipo_hash{
	lista tabela[TAM];
}hash;

void inicializa_hash(hash *h){
	for(int i=0; i<TAM; i++)
		inicializa_lista(&(h->tabela[i]));
}

int funcao_hash(int chave){ //retorna a posição do hash com base em "Parâmetro para função_hash"
	return chave%TAM;
}

int char_to_int(cstring palavra){ //Retorna o número da tabela ascii da primeira letra, em minúsculo, da palavra
	return (int)tolower(palavra[0]);
}

int insere_hash(hash* h, cstring nova_palavra){
	return insere_ultimo(&(h->tabela[funcao_hash(char_to_int(nova_palavra))]), nova_palavra);
}

int buscar_hash(hash* h, cstring palavra, item* retorno){
	return buscar_valor_lista(&(h->tabela[funcao_hash(char_to_int(palavra))]), palavra, retorno);
}

void escreve_hash(hash* h){
	for(int i=0; i<TAM; i++){
		if(vazia(&(h->tabela[i])))
			continue;
		else escreve_lista(&(h->tabela[i]));
	}
}

int armazenar_palavras(hash *h, cstring nome_arquivo){
	FILE *arq = fopen(nome_arquivo, "r");
	char buffer[50];
	int palavras_novas = 0; //Esta variável aumenta conforme novas palavras são adicionadas ao hash e é retornada por esta função

	if(arq != NULL){
		while(1){
			fscanf(arq, "%s", buffer);
			if(feof(arq))
				break;
			palavras_novas += insere_hash(h, buffer);
		}
		return palavras_novas;
	}
	return -1; //Arquivo inexistente
}

int consultar_palavra(hash* h, cstring palavra){
	item *aux;
	if(buscar_hash(h, palavra, aux))
		return aux->qntd; //retorna a quantidade de vezes que a palavra apareceu caso encontre
	return 0; //retorna 0 caso não encontre
}

celula* mais_frequente_hash(hash* h){
	celula *aux;
	celula *ret;
	int maior, flag = 0;

	for(int i=0; i<TAM; i++){
		if(!vazia(&(h->tabela[i]))){
			aux = h->tabela[i].primeiro->prox;
			if(!flag){
				maior = aux->dado.qntd;
				ret = aux;
				flag = 1;
			}
			while(aux != h->tabela[i].primeiro){
				if(aux->dado.qntd > maior){
					ret = aux;
					maior = aux->dado.qntd;
				}
				aux = aux->prox;	
			}
		}
	}

	return ret;
}

celula* prox_menor(hash* h, int atual, lista* l){
	celula *aux, *ret;
	int menor, flag = 0;

	for(int i=0; i<TAM; i++){
		if(!vazia(&(h->tabela[i]))){
			aux = h->tabela[i].primeiro->prox;
			if(!flag){
				menor = aux->dado.qntd;
				ret = aux;
				flag = 1;
			}
			while(aux != h->tabela[i].primeiro){
				if(aux->dado.qntd <= atual){
					if(aux->dado.qntd >= menor && !buscar_lista(l, aux->dado.palavra)){
						ret = aux;
						menor = aux->dado.qntd;
					}
				}
				aux = aux->prox;
			}
		}
	}

	return ret;
}

lista* mais_frequentes(hash* h, int n){
	lista *retorno;
	celula *aux;
	int tam_retorno = 0, qntd_atual;

	inicializa_lista(retorno);
	aux = mais_frequente_hash(h);
	qntd_atual = aux->dado.qntd;
	insere_ultimo_qntd(retorno, aux->dado.palavra, aux->dado.qntd);
	tam_retorno++;

	for(int i=0; i<TAM; i++){
		if(tam_retorno == n) break;
		if(!vazia(&(h->tabela[i]))){
			aux = prox_menor(h, qntd_atual, retorno);
			insere_ultimo_qntd(retorno, aux->dado.palavra, aux->dado.qntd);
			qntd_atual = aux->dado.qntd;
			tam_retorno++;
		}
		if(i == TAM-1 && tam_retorno < n) i = 0;
	}

	return retorno;
}
//---------------------------------------//

void cortar_string(cstring s){ //Usado para adicionar \0 ao final de uma string
	s[strlen(s)-1] = '\0';
}

int main(){
	hash A;
	int opt, qntd_novas_palavras = 0, qntd_ele_mais_frequentes, tamanho_hash = 0;
	lista *palavras_mais_frequentes;
	char arquivo[50], consulta[50];

	inicializa_hash(&A);
	do{
		printf("[1] Armazenar palavras\n");
		printf("[2] Consultar palavra\n");
		printf("[3] Palavras mais frequentes\n");
		scanf("%d", &opt);

		switch(opt){			
			case 1:
				system("clear");
				printf("Escreva o nome do arquivo para leitura: ");
				__fpurge(stdin);
				fgets(arquivo, 50, stdin);
				cortar_string(arquivo);
				qntd_novas_palavras = armazenar_palavras(&A, arquivo);
				tamanho_hash += qntd_novas_palavras;
				if(qntd_novas_palavras != -1)
					printf("\nLeitura concluída.\nForam adicionadas %d novas palavras!\n", qntd_novas_palavras);
				else printf("ARQUIVO INEXISTENTE!\n");
				printf("\n");
				break;

			case 2:
				printf("\nEscreva a palavra que deseja consultar: ");
				__fpurge(stdin);
				fgets(consulta, 50, stdin);
				cortar_string(consulta);
				printf("Quantidade: %d\n", consultar_palavra(&A, consulta));
				printf("\n");
			break;

			case 3:
				printf("Insira a quantidade de elementos: ");
				scanf("%d", &qntd_ele_mais_frequentes);
				palavras_mais_frequentes = mais_frequentes(&A, qntd_ele_mais_frequentes);
				escreve_mais_frequentes(palavras_mais_frequentes);
			break;

			case 4:
				escreve_hash(&A);
			break;
		}
		__fpurge(stdin);
	}while(opt != 0);

}