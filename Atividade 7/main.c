/*
 * Atividade7.c
 * Created: 03/10/2019 08:35:11
 * Author : Bryan Wolff
 * RA: 214095
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h> 

//registradores usados para definicao do Baud Rate
unsigned char *pUBRR0H = (unsigned char*) 0xC5;
unsigned char *pUBRR0L  = (unsigned char*) 0xC4;

//registradores de confiuracao e status
unsigned char *pUCSR0A = (unsigned char*) 0xC0;
unsigned char *pUCSR0B = (unsigned char*) 0xC1;
unsigned char *pUCSR0C = (unsigned char*) 0xC2;

//registrador usado para a transferencia de dados
unsigned char *pUDR0 = (unsigned char*) 0xC6;

//registrador usados para configurar e acender os leds
unsigned char *pddrb = (unsigned char*) 0x24;	//DDRB
unsigned char *pportb = (unsigned char*) 0x25;//PORTB

//buffer--------------------------------------------
volatile char buffer[10];
volatile char c; //carregará o caracter analisado
volatile int i = 0; //variaveis globais usadas para o buffer definindo a próxima posição livre do buffer
volatile int j = 0; //define a a posição do elemento mais antigo para ser removido
volatile int z = 0; //define a quantidade de elementos no buffer

//rotina para adicionar elemento ao buffer
void adicionar_buffer(char c){
	if (z<10) { //adicionar se o buffer nao tiver cheio
	buffer[i]=c;
	i++;
	if(i==10)i=0;
	z++;
	}
}

//rotina para remover elemento do buffer retornando o caracter que será analisado
char remover_buffer() {
	char k;
	k = buffer[j];
	j++;
	if(j==10)j=0;
	z--;
	return k;
}

//configuração dos registradores usados para acender o LED e para configurar a USART
void configuracao(){

//como o baud rate eh de baudrate 9600bps, pela tabela temos o decimal 103 e o hexadecimal 0x0067
//entao 0x00 ira para UBRR0H e 0x67 ira para UBRR0L
*pUBRR0H = 0x00;
*pUBRR0L = 0x67;

//configuracao dos registradores de configuracao e status
*pUCSR0A &= 0b11111100; //velocidade normal, desativar o modo de comunicacao multi-processador
*pUCSR0B = 0b10111000; //ativa interrupção do tipo recepção completa e UDR0 vazio, ativar trasmissor receptor, e desativar nono bit de dado
*pUCSR0C = 0b00001110; //para USART assincrona, sem uso de bit de paridade, 2 bit de parada, 8 bit de dados

*pddrb |= 0b00000111; //configura os pinos PB2 a PB0 para corrente de saida do led RGB

asm("sei"); //liga as interrupções

}

//mensagens para transmissão---------------------------
char r[] = "Comando: Acender LED - cor vermelha\n";
char g[] = "Comando: Acender LED - cor verde\n";
char b[] = "Comando: Acender LED - cor azul\n";
char ye[] = "Comando: Acender LED - cor amarela\n";
char ciano[] = "Comando: Acender LED - cor ciano\n";
char m[] = "Comando: Acender LED - cor magenta\n";
char w[] = "Comando: Acender LED - cor branca\n";
char other[] = "Comando incorreto\n";
char empty[] = "Vazio!\n";

char *pc; //ponterio usado para carregar o caracter no UDR0, e abaixo vamos começar apontando para o empty[]

volatile int y = 0; //variavel usada para saber qual mensagem sera trasmitida, y = 1 para trasmitir r[], 2 para g[], etc

//vetor com as cores do led RGB na ordem: apagado, vermelho, verde, azul, amarelo, ciano, magenta e branco.
unsigned char led[8] = {0b00000000,0b00000001,0b00000010,0b00000100,0b0000011,0b00000110,0b00000101,0b00000111};
	
//Rotina de Serviço de Interrupção associada a "Registrador de dados vazio" responsavél por transmitir os caracteres
ISR(USART_UDRE_vect) {
	*pUDR0 = *pc;
	*pc++; //incrementa para na proxima interrupcao trasnimir o proximo caracter do vetor
	if (*pc == 0x00) { // se acabar a string e detectar o caracter nulo \0, que em hexadecimal eh 0x00, q eh o o ultimo caracter da string devemos redefinir o endereço do ponteiro
					//o ponteiro apontara para a mensagem definida por y
		*pUCSR0B &= 0b11011111; //desabilita a interrupção registrador de dados vazio quando termina de mandar uma mensagem
		if (y==0) pc = &empty[0];
		else if (y==1)	pc = &r[0];
		else if (y==2)	pc = &g[0];
		else if (y==3)	pc = &b[0];
		else if (y==4)	pc = &ye[0];
		else if (y==5)	pc = &ciano[0];
		else if (y==6)	pc = &m[0];
		else if (y==7)	pc = &w[0];
		else if (y==8)	pc = &other[0];
	}
}

//Rotina de Serviço de Interrupção associada a "Recepção Completa", responsavél por inserir no buffer o novo caracter recebido
ISR(USART_RX_vect) {
	adicionar_buffer(*pUDR0);
}

//verifica se o buffer tem elemento para extrair-lo para saber qual COR acender e qual MENSAGEM deve ser transmitida
void verificar_caracter_do_buffer() {
	
	if (z > 0) { //se buffer não esá vazio vamos remover um caracter e armazenar na variavel c para compara-lo
		c = remover_buffer() ;
		if (c == 0x72) { //se for "r" em hexadecimal pela tabela ASCII eh 0x72
			y = 1;		//devemos trasmitir a mensagem 2, isto é, r[]
			*pportb = led[1]; //acende a cor vermelha
		}
		else if (c == 0x67) {//se for "g" em hexadecimal pela tabela ASCII eh 0x67
			y = 2;		//devemos trasmitir a mensagem 3, isto é, g[]
			*pportb = led[2]; //acende a cor verde
		}
		else if (c == 0x62) {//se for "b" em hexadecimal pela tabela ASCII eh 0x62
			y = 3;		//devemos trasmitir a mensagem 1, isto é, b[]
			*pportb = led[3]; //acende a cor azuk
		}
		else if (c == 0x79) {//se for "y" em hexadecimal pela tabela ASCII eh 0x79
			y = 4;		//devemos trasmitir a mensagem 4, isto é, ye[]
			*pportb = led[4]; //acende a cor amarela
		}
		else if (c == 0x63) {//se for "c" em hexadecimal pela tabela ASCII eh 0x63
			y = 5;		//devemos trasmitir a mensagem 5, isto é, ciano[]
			*pportb = led[5]; //acende a cor ciano
		}
		else if (c == 0x6d) {//se for "m" em hexadecimal pela tabela ASCII eh 0x6d
			y = 6;		//devemos trasmitir a mensagem 6, isto é, m[]
			*pportb = led[6]; //acende a cor magenta
		}
		else if (c == 0x77) {//se for "w" em hexadecimal pela tabela ASCII eh 0x77
			y = 7;		//devemos trasmitir a mensagem 7, isto é, w[]
			*pportb = led[7]; //acende a cor branca
		}	
		else y = 8;	 //caracter invalido entao temos que transmitir a mensagem 8, isto é, other[]
	_delay_ms(200);	
		
	}
	else y = 0; //buffer está vazio	 entao devemos trasmitir a mensagem 0, isto é, empty[]
	*pUCSR0B |= 0b00100000; //habilita interrupção registrador de dados vazio para iniciar a transmissao de uma nova mensagem
	*pportb = led[0]; //apaga o led
}  

int main(void) {	
	pc = &empty[0]; //faz o ponteiro iniciar apontando para o endereço da string de buffer vazio
	configuracao(); //ativa as configurações
    while (1) verificar_caracter_do_buffer(); 
}