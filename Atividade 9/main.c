/*
 * Atividade 9
 *
 * Bryan Wolff		RA: 214095
 *
 */

/*
 *	Escolhendo o modo PWM n�o invertido, um prescaler de 8 e habilitando a interrup��o de overflow do timer2
 *	o Sinal PWM ser� gerado no pino 11 (OC2A), e a frequencia de saida deste sinal � 16mhz/[(256 ciclos do contador) * prescaler] = 7812,5Hz
 *	logo o periodo que o contador atingir� o overflow e chamar� a RSI � de {16mhz/[(256 ciclos do contador) * prescaler]}^-1 = 0,000128s
 *	O valor em OCR2A definir� a intensidade (0 para a minima, 255 para m�xima)
 *	A cada 31 vezes que a RSI for ativada ser� incrementado uma vez o valor de OCR2A para aumentar a intensidade, ou seja, a cada 31*0,000128 = 0,003968s
 *	Ser� usado a variavel "contador" para chamar 31 vezes a RSI, e o valor 31 foi usado pois ao passar por todas a intensidades temos 1s aproximadamente
 *	tudo isso devido � 31*255*0,000128 = 1,01184s, como o objetivo � 1s, temos um erro de aproximadamente 11,84 ms
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000UL

volatile int sentido = 0; //variavel referencial, onde 0 significa que deve crescer a intensidade e 1 diminuir
volatile int contador = 0; //variavel referencial para saber quantas vezes foi chamada a Rotina de Servi�o de Interrup��o (RSI)

int main(void)
{
		//CONFIGURA��O DO TEMPORIZADOR 2
		TCCR2A = (TCCR2A |= 0b10000011) & 0b10001111; //define o modo fast mode PWM, n�o invertido, flag TOV setando em MAX com OC2A conectado no pino 11
		TCCR2B = (TCCR2B |= 0b00000010) & 0b00110010; //define um precaler de 8
		TIMSK2 = (TIMSK2 |= 0b00000001) & 0b11111001; //habilita apenas a interrupcao associada ao Timer/Counter2 overflow flag
		OCR2A = 0; // carrega o valor 0 para iniciar com a intensidade mais fraca
		DDRB |= 0b00101000; //configura a porta PB3 onde est� o led para corrente de saida
		asm("sei"); //habilita todas as interrup��es

    while (1); //o programa fica aqui para aguardar a interrup��o
}

/* RSI associada ao overflow do timer2, com objetivo de variar a intensidade do LED*/
ISR(TIMER2_OVF_vect){
	contador++; //a variavel contador ser� incrementada, definindo quantas vezes chamamos essa RSI
	if (contador == 31) {//quando atingir 31 verificamos se devemos aumentar ou diminuir a intensidade, de acordo com a variavel sentido
	 	contador = 0; //se atingiu 31, reiniciamos a contagem
	 	if (sentido == 0){ //se o sentido esta em 0 devemos aumentar a intensidade
		 	OCR2A++; //aumentamos a intensidade
		 	if (OCR2A == 255) {
				 sentido = 1;
				 	if ((PINB & 0b00100000) == 0b00000000) PORTB |= 0b00100000;
				 	else PORTB &= 0b11011111;
				  }//se chegou no seu valor maximo devemos mudar o sentido para posteriormente diminuir a intensidade
	 	}
	 	else { //se o sentido esta em 1, devemos diminuir a intensidade
		 	OCR2A--; //diminuimos a intensidade
		 	if (OCR2A == 0) {
				 sentido = 0;
				 if ((PINB & 0b00100000) == 0b00000000) PORTB |= 0b00100000;
				 else PORTB &= 0b11011111;
				 } //se chegou no seu valor m�nimo devemos mudar o sentido para posteriormente aumentar a intensidade
	 	}
	}
}
