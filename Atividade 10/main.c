/*
 * atividade10.c
 * Bryan Wolff		RA: 214095
 */ 

#include <avr/io.h>
#include <avr/interrupt.h> 

/*

Para atingir a frequência necessária de 62500Hz, tendo frequencia de saída no pino 11: 16mhz/ [(256 Ciclos do contador) * prescaler]
Logo, temos então a necessidade de escolher o prescaler de 1, para atingir a frequencia necessária. Partimos da interrupçao de overflow
Para atuar na geração da onda com a RSI, temos que a mesma será ativada com a frequencia (RSI) = (1/16MHz)*256 = 0,000016s. A partir disso,
temos a necessidade de a cada chamada da RSI inserir uma nova amostra, já que o periodo de amostras é (62500)^-1 = 0,000016s também.
Para o led incorporado, utilizaremos um contador para saber quantas vezes a RSI foi chamada para piscar o led nas frequencias pre-estabelecidas
(Esses detalhes estao comentados na própria RSI). Para as amostras, foi calculado as tensões médias a partir das equações e comportamento da onda
e utilizamos OCR2A = (Vm*256)/5 - 1 para saber qual é a amostra na unidade do registrador OCR2A. Para a USART, usaremos o BaudRate de 9600
2 bit de parada e nenhum bit de paridade. 

*/

//amostras-------------------------------------------
char senoidal[200] = {128, 132, 136, 140, 144, 148, 152, 156, 160, 164, 168, 172, 175, 179, 183, 186, 190, 193, 197, 200, 204, 207, 210, 213, 216, 219, 222, 224, 227, 229, 232, 234, 236, 238, 240, 242, 244, 246, 247, 249, 250, 251, 252, 253, 254, 254, 254, 255, 255, 255, 255, 254, 254, 254, 254, 254, 253, 252, 251, 250, 249, 247, 246, 244, 242, 240, 238, 236, 234, 232, 229, 227, 224, 222, 219, 216, 213, 210, 207, 204, 200, 197, 193, 190, 186, 183, 179, 175, 172, 168, 164, 160, 156, 152, 148, 144, 140, 136, 132, 128, 124, 120, 116, 112, 108, 104, 100, 96, 93, 89, 85, 81, 77, 74, 70, 67, 63, 60, 56, 53, 50, 47, 44, 41, 38, 35, 32, 30, 27, 25, 22, 20, 18, 16, 14, 12, 11, 9, 8, 7, 5, 4, 3, 3, 2, 1, 1, 1, 0, 0, 0, 1, 1, 1, 2, 3, 3, 4, 5, 7, 8, 9, 11, 12, 14, 16, 18, 20, 22, 25, 27, 30, 32, 35, 38, 41, 44, 47, 50, 53, 56, 60, 63, 67, 70, 74, 77, 81, 85, 89, 93, 96, 100, 104, 108, 112, 116, 120, 124, 128};
char triangular[200] = {0, 2, 4, 7, 9, 12, 14, 17, 19, 22, 25, 27, 30, 32, 35, 37, 40, 43, 45, 48, 50, 53, 55, 58, 60, 63, 66, 68, 71, 73, 76, 78, 81, 83, 86, 89, 91, 94, 96, 99, 101, 104, 107, 109, 112, 114, 117, 119, 122, 124, 127, 130, 132, 135, 137, 140, 142, 145, 147, 150, 153, 155, 158, 160, 163, 165, 168, 171, 173, 176, 178, 181, 183, 186, 188, 191, 194, 196, 199, 201, 204, 206, 209, 211, 214, 217, 219, 222, 224, 227, 229, 232, 235, 237, 240, 242, 245, 247, 250, 252, 255, 252, 250, 247, 245, 242, 240, 237, 235, 232, 229, 227, 224, 222, 219, 217, 214, 211, 209, 206, 204, 201, 199, 196, 194, 191, 188, 186, 183, 181, 178, 176, 173, 171, 168, 165, 163, 160, 158, 155, 153, 150, 147, 145, 142, 140, 137, 135, 132, 130, 127, 124, 122, 119, 117, 114, 112, 109, 107, 104, 101, 99, 96, 94, 91, 89, 86, 83, 81, 78, 76, 73, 71, 68, 66, 63, 60, 58, 55, 53, 50, 48, 45, 43, 40, 37, 35, 32, 30, 27, 25, 22, 19, 17, 14, 12, 9, 7, 4, 2};
char dente_serra[200] = {0, 2, 3, 4, 5, 7, 8, 9, 11, 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 28, 30, 31, 32, 34, 35, 36, 37, 39, 40, 41, 43, 44, 45, 46, 48, 49, 50, 51, 53, 54, 55, 57, 58, 59, 60, 62, 63, 64, 66, 67, 68, 69, 71, 72, 73, 75, 76, 77, 78, 80, 81, 82, 83, 85, 86, 87, 89, 90, 91, 92, 94, 95, 96, 98, 99, 100, 101, 103, 104, 105, 107, 108, 109, 110, 112, 113, 114, 115, 117, 118, 119, 121, 122, 123, 124, 126, 127, 128, 130, 131, 132, 133, 135, 136, 137, 139, 140, 141, 142, 144, 145, 146, 147, 149, 150, 151, 153, 154, 155, 156, 158, 159, 160, 162, 163, 164, 165, 167, 168, 169, 171, 172, 173, 174, 176, 177, 178, 179, 181, 182, 183, 185, 186, 187, 188, 190, 191, 192, 194, 195, 196, 197, 199, 200, 201, 203, 204, 205, 206, 208, 209, 210, 211, 213, 214, 215, 217, 218, 219, 220, 222, 223, 224, 226, 227, 228, 229, 231, 232, 233, 235, 236, 237, 238, 240, 241, 242, 243, 245, 246, 247, 249, 250, 251, 252, 254, 255} ;
char am[200] = {128, 141, 155, 168, 181, 192, 203, 211, 218, 223, 226, 226, 223, 218, 211, 201, 189, 176, 161, 145, 128, 110, 93, 76, 60, 46, 34, 23, 16, 10, 8, 9, 13, 19, 29, 41, 55, 71, 89, 108, 127, 147, 166, 185, 202, 217, 230, 241, 249, 253, 255, 253, 249, 241, 230, 217, 202, 185, 166, 147, 128, 108, 89, 71, 55, 41, 29, 19, 13, 9, 8, 10, 16, 23, 34, 46, 60, 76, 93, 110, 127, 145, 161, 176, 189, 201, 211, 218, 223, 226, 226, 223, 218, 211, 203, 192, 181, 168, 155, 141, 128, 114, 102, 91, 81, 72, 65, 60, 57, 55, 56, 58, 62, 67, 73, 81, 90, 99, 108, 118, 127, 137, 145, 153, 160, 166, 171, 175, 177, 178, 178, 177, 174, 171, 166, 161, 155, 148, 142, 135, 128, 121, 114, 108, 102, 97, 93, 89, 87, 86, 85, 86, 87, 89, 93, 97, 102, 108, 114, 121, 127, 135, 142, 148, 155, 161, 166, 171, 174, 177, 178, 178, 177, 175, 171, 166, 160, 153, 145, 137, 128, 118, 108, 99, 90, 81, 73, 67, 62, 58, 56, 55, 57, 60, 65, 72, 81, 91, 102, 114};

//mensagens para transmissão---------------------------
char s[] = "Onda senoidal\n";
char t[] = "Onda triangular\n";
char d[] = "Onda dente-de-serra\n";
char a[] = "Onda AM\n";
char other[] = "Nenhuma forma de onda selecionada\n";

char *pc; //ponteiro usado para carregar o caracter no UDR0

char *pa; //ponteiro usado para carregar os valores da amostra no OCR2A

volatile int y = 0; //variavel usada para saber qual onde sera trasmitida, y = 1 para trasmitir senoidal, 2 para triangular, etc
//Será usada para saber qual será a frequencia do led incorporado

volatile int x = 0; //variavel utilizada como contador para percorrer as amostras auxiliar o ponteiro a percorrer o vetor de amostras

volatile char c; //carregará o caracter analisado

volatile int contador = 0; //variavel para contar quantas vezes a interrupção foi chamada

void configuracao(){

	//como o baud rate eh de baudrate 9600bps, pela tabela temos o decimal 103 e o hexadecimal 0x0067
	//entao 0x00 ira para UBRR0H e 0x67 ira para UBRR0L
	UBRR0H = 0x00;
	UBRR0L = 0x67;

	//configuracao dos registradores de configuracao e status
	UCSR0A &= 0b11111100; //velocidade normal, desativar o modo de comunicacao multi-processador
	UCSR0B = 0b10011000; //ativa interrupção do tipo recepção completa, ativar trasmissor receptor, e desativar nono bit de dado
	UCSR0C = 0b00001110; //para USART assincrona, sem uso de bit de paridade, 2 bit de parada, 8 bit de dados

	//CONFIGURAÇÃO DO TEMPORIZADOR 2
	TCCR2A = (TCCR2A |= 0b10000011) & 0b10001111; //define o modo fast mode PWM, não invertido, flag TOV setando em MAX com OC2A conectado no pino 11
	TCCR2B = (TCCR2B |= 0b00000001) & 0b00110001; //sem prescaler, frequencia do temporizador de 16Mhz
	TIMSK2 = (TIMSK2 |= 0b00000001) & 0b11111001; //habilita apenas a interrupcao associada ao Timer/Counter2 overflow flag

	DDRB |= 0b00101000; //configura a porta PB5 e PB2 onde está o led e a OC2A para corrente de saida
	
	asm("sei"); //habilita as interruções
}

int main(void){
    configuracao(); //ativa as configurações
	while (1);
}

//rotina que quando chamada faz o led incorporado acender ou apagar
void LED(){ 
	contador = 0; //reinicia o contador
	if ((PINB & 0b00100000) == 0b00000000) PORTB |= 0b00100000; //se o led estava apagado anteriormente, ele acenderá
	else PORTB &= 0b11011111; //caso estava aceso anteriormente, ele apagará
}

/*----RSI associada ao overflow do Timer2----*/		//será chamada a cada 0,000016s
ISR(TIMER2_OVF_vect){
	
	/*-----Rotina para piscar o led-----*/ 
	if (y != 0) {	//só iniciará se a USART receber o primeiro caracter
		contador++;
		if ((y == 1)&&(contador == 0xf424))LED();		//se for a onda for senoidal o led vai piscar a cada 62500*0,000016 = 1s (o programa só aceitou hexadecimal aqui)
		else if ((y == 2)&&(contador == 31250))LED();	//se for a onda for triangular o led vai piscar a cada 31250*0,000016 = 500ms
		else if ((y == 3)&&(contador == 15625))LED();	//se for a onda for dente-de-serra o led vai piscar a cada 15625*0,000016 = 250ms
		else if ((y == 4)&&(contador == 7813))LED();	//se for a onda for senoidal o led vai piscar a cada 7813*0,000016 = 125ms
		else if (y == 5){ //se não foi detectado nenhuma onda, o led apagará
			contador = 0;
			PORTB &= 0b11011111; //apaga o led
			OCR2A = 0;	//não gera nenhuma onda
		}
	}
	/*-----Rotina para percorrer as amostras e colocar no OCR2A-----*/
	if ((y != 0)&&(y != 5)){ //será executada somente se necessário criar forma de onda
		OCR2A = *(pa + x); //x será somado ao endereço do ponteiro para poder carregar o valor deste endereço (amostra) no OCR2A
		x++; //x incrementará para posteriormente carregar a próxima amostra
		if (x == 200) x = 0; //se chegar no final de um vetor de amostras, x voltará a 0 para carregar a primeira amostra novamente e manter um ciclo
	}
}

//Rotina de Serviço de Interrupção associada a "Recepção Completa", responsável por inserir no buffer o novo caracter recebido
ISR(USART_RX_vect){
	c = UDR0;
	if (c == 0x73){		//se for "s" em hexadecimal pela tabela ASCII eh 0x73, devemos trasmitir s[]
		y = 1;
		pc = &s[0];
		pa = &senoidal[0];
	}
	else if (c == 0x74){	//se for "t" em hexadecimal pela tabela ASCII eh 0x74, devemos trasmitir t[]
		y = 2;
		pc = &t[0];
		pa = &triangular[0];
	}
	else if (c == 0x64){	//se for "d" em hexadecimal pela tabela ASCII eh 0x64, devemos trasmitir d[]
		y = 3;
		pc = &d[0];
		pa = &dente_serra[0];
	}
	else if (c == 0x61){	//se for "a" em hexadecimal pela tabela ASCII eh 0x79, devemos trasmitir a[]
		y = 4;
		pc = &a[0];
		pa = &am[0];
	}
	else {	// se for outro caracter devemos trasmitir a mensagem 5, isto é, other[]
		y = 5;
		pc = &other[0];
	}
	UCSR0B |= 0b00100000; //habilita interrupção registrador de dados vazio para iniciar a transmissao de uma nova mensagem
}

//Rotina de Serviço de Interrupção associada a "Registrador de dados vazio" responsável por transmitir os caracteres
ISR(USART_UDRE_vect){
	UDR0 = *pc; //carrega o caracter no UDR0 para transmitir
	pc++; //incrementa para na proxima interrupcao trasmitir o proximo caracter do vetor
	if (*pc == 0x00) { // se acabar a string e detectar o caracter nulo \0, que em hexadecimal eh 0x00, q eh o o ultimo caracter da string devemos parar de trasmitir
		UCSR0B &= 0b11011111; //desabilita a interrupção registrador de dados vazio quando termina de mandar uma mensagem
	}	
}