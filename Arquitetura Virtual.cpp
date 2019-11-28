/*
@Autores: Giorno Givanna, Leandro

Arquitetura de 16 bits

Códigos das instruções:

	LD 		0001 -> "1" - Tipo P (Carregar da memória para o registrador)
	STR 	0010 -> "2" - Tipo P (Carregar na memória)
	
	SM 		0101 -> "5" - Tipo N (Soma)
	SB 		0110 -> "6" - Tipo N (Subtração)
	XVSC 	0111 -> "7" - Tipo N (Divisão)
	XXX   	1000 -> "8" - Tipo N (Multiplicação)

Modelo das instruções Tipo P:

		op code		Endereço do Registrador		Endereço de memória
		4 bits          	4 bits            	      8 bits
	     
Modelo das instruções Tipo N:

		op code		Endereço de Destino		Endereço do Registrador 1	Endereço do Registrador 2
		4 bits			  4 bits				 	4 bits			    		 4 bits

*/


#include <iostream>
#include <stdio.h>
#include <cmath>

#define EVER ;;
#define OLD 0x00
#define LD 0x01
#define STR 0x02
#define SM 0x05
#define SB 0x06
#define XVSC 0x07
#define XXX 0x08

using namespace std;

int memProg[10];

int memDad[255];
int dp[8];

typedef struct ch
{
	bool valid;
	unsigned int tag;
	int data[4];	
}Ch;

Ch memCache[2];

void executar(int SLA);
int cache(int pc);
int loadCache(int pc);

int main()
{
	int pc = 0;
	int SLA = 0;
	for(int i = 0; i < 100; i++)
	{
		memProg[i] = 0;
		memDad[i] = 0;
	}
	for(int i = 0; i < 8; i++)
	{
		dp[i] = 0;
	}
	for(int i = 0; i < 2; i++)
	{
		memCache[i].valid = false;
		memCache[i].tag = 0;
	}
	
	/***************************Entrada de dados********************************/
	memProg[0] = 0x1000;	//Load do reg0 para a memoria 0
	memProg[1] = 0x1101;	//Load do reg1 para a memoria 1
	memProg[2] = 0x1202;	//Load do reg2 para a memoria 2
	memProg[3] = 0x5310;	//Soma entre reg1 e reg0 armazena em reg3
	memProg[4] = 0x6432;	//Sub entre reg3 e reg2 armazena em reg4
	memProg[5] = 0x7501;	//Div entre reg0 e reg1 armazena em reg5
	memProg[6] = 0x8620;	//Mult entre reg2 e reg0 armazena em 6
	memProg[7] = 0x2600;	//Store entre reg6 e memoria 0
	memProg[8] = 0x1000;	//Load entre reg0 e memoria 0 
	memProg[9] = 0x1100;	//Load entre reg1 e memoria 0
	/***************************************************************************/
	memDad[0] = 100;
	memDad[1] = 50;
	memDad[2] = 32;	
	
	for(int i=0; memProg[i]!=0; i++){
		
		SLA = cache(pc);
		executar(SLA);
		pc += 1;
		
		
		cout<<"\n******** Instrucao "<<i+1<<" ************\n";
		cout<<"\Registrador: \n";
		for(int j=0;j<8;j++)
			cout<<"dp["<<j<<"]: "<<dp[j]<<endl;
	}
	
	return 0;
}

void executar(int SLA)
{
		if((SLA>>12) < 0x05) 								//TIPO P
		{
			if(SLA>>12 == LD)
			{
				dp[(SLA&0x0F00)>>8] = memDad[SLA&0x00FF];
			}

			if(SLA>>12 == STR)
			{
				memDad[SLA&0x00FF] = dp[(SLA&0x0F00)>>8];
			}
		}
	else if(((SLA>>12) >= 0x05) & ((SLA>>12) < 0x0A)) 		//TIPO N
		{
			if(SLA>>12 == SM)
			{
				dp[(SLA&0x0F00)>>8] = dp[(SLA&0x00F0)>>4] + dp[SLA&0x000F];
			}
			if(SLA>>12 == SB)
			{
				dp[(SLA&0x0F00)>>8] = dp[(SLA&0x00F0)>>4] - dp[SLA&0x000F];
			}
			if(SLA>>12 == XXX)
			{
				dp[(SLA&0x0F00)>>8] = dp[(SLA&0x00F0)>>4] * dp[SLA&0x000F];
			}
			if(SLA>>12 == XVSC)
			{
				dp[(SLA&0x0F00)>>8] = dp[(SLA&0x00F0)>>4] / dp[SLA&0x000F];
			}

		}
}

int cache(int pc){
	int word;
	int line;
	int tag;
	
	word = pc & 0x03;
	line = pc >>2;
	line &= 0x01;
	tag = pc>>2;
	
	if((memCache[line].valid) && (memCache[line].tag == tag)){
		cout<<"\n\n\nProcura na cache = hit"<<endl;
		return memCache[line].data[word];
	}
	else{
		cout<<"\n\n\nProcura na cache = miss"<<endl;
		return loadCache(pc);
	}
	return 0;
}

int loadCache(int pc){
	int word;
	int line;
	int tag;
	
	word = pc & 0x03;
	line = pc >>2;
	line &= 0x01;
	
	for(int i=0; i<2; i++){
		memCache[i].valid = true;
		memCache[i].tag = (pc>>2)+i;
		tag = memCache[i].tag;
		for(int j=0; j<4; j++){
			memCache[i].data[j] = memProg[tag*4+j];
		}
	}
	return memProg[(tag-1)*4+word];
}

