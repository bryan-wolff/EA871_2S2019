/*
 * Atividade8.c
 *
 * Author : Bryan Wolff
 * RA: 214095
 *
 */ 

#include <avr/io.h>
#define  F_CPU 16000000UL
#include <avr/interrupt.h>
#include <util/delay.h>

/*		
*		Com o prescaler de 256 e um periodo de tempo efetivo do temporizador de 0,002s, utilizei a seguinte formula para saber o valor que ira em OCR2A:
*		Compare match register = [16.000.000Hz / (prescaler * ( frequencia desejada)] - 1
*		Compare match register = [16.000.000Hz / (prescaler * ( 1 / periodo de tempo desejado)] - 1
*		Compare match register = [16.000.000Hz / (256 * ( 1 / 0,002)] - 1 = 124
*		Logo, o valor do compare match register A (OCR2A) deve ser = 124
*		Logo, o programa chamara a interrupcao TIMER2_COMPA a cada periodo definido de 0,002s 
*		Portanto, eh preciso chamar 390 vezes a rotina de interrupcao para ter um atraso de 0,78s
*		E 250 vezes para um atraso de 0,5s
*		Utilizarei as variaveis "y" e "z" para incrementar a cada chamada de interrupção para saber quando devo piscar o led
*/


//cadeia de caracteres para ser transmitida pela UART
char msg[] = " Atividade 8 – Interrupcoes temporizadas tratam concorrencia entre tarefas! \n";

//variavel usada para percorrer os elementos do vetor da mensagem
unsigned int x = 0; //associada a msg

//variavel usada para referencia do tamanho das mensagens para saber o maximo onde x deve percorrer para parar
unsigned int max_x = sizeof(msg);

//variaveis usadas para saber quantas vezes ocorreu o periodo de 0,002s para saber quando acender o led
volatile int y = 0;
volatile int z = 0;

/*	Interrupcao do timer 2 associada ao CTC	 */

ISR(TIMER2_COMPA_vect){
 	y++;
 	z++;
 	if (y == 250){ //se a interrupcao for chamada 250 vezes, temos um tempo percorrido de 0,5s, logo o LED incorporado deve acender/apagar

 		//Utilizada para piscar o led incorporado ao arduino (PB5)
  		if ((PINB & 0b00100000) == 0b00000000) PORTB |= 0b00100000; //se o led estava apagado anteriormente, ele acenderá
  		else PORTB &= 0b11011111; //caso estava aceso anteriormente, ele apagará
 		y = 0; //restaura o valor de y para comecar uma proxima contagem posteriormente
 	}

 	if (z == 390){//se a interrupcao for chamada 390 vezes, temos um tempo percorrido de 0,78s, logo o LED do pino 12 deve acender/apagar
 		
 		//Utilizada para piscar o led externo no pino 12 (PB4)
  		if ((PINB & 0b00010000) == 0b00000000) PORTB |= 0b00010000; //se o led estava apagado anteriormente, ele acenderá
  		else PORTB &= 0b11101111; //caso estava aceso anteriormente, ele apagará
 		 z = 0; //restaura o valor de z para comecar uma proxima contagem posteriormente
 	}
  }


int main(void)
{	
	//CONFIGURAÇÂO DO TEMPORIZADOR 2
	TCCR2A = (TCCR2A |= 0b00000010) & 0b00001110; //configura o temporizador para modo de operação CTC. OC0B e OC0A desconectado
	TCCR2B = (TCCR2B |= 0b00000110) & 0b00110110;	//Aqui define um prescaler de 256
	TIMSK2 = (TIMSK2 |= 0b00000010) & 0b11111010;	//habilita apenas a interrupção associada ao Timer/Counter2 Output Compare Match A
	OCR2A = 124; // carrega o valor 124 no registrador de comparação
	
	//CONFIGURACAO DO BAUD RATE
	//como o baud rate eh de 38400, isto é, deverá ser colocado em UBRR0 o valor 0x0019 em hexadecimal
	UBRR0 = 0x0019;
	
	//CONFIGURACAO DOS REGISTRADORES DE CONFIGURACAO E STATUS DA UART
	UCSR0A &= 0b11111100; //velocidade normal, desativar o modo de comunicacao multi-processador
	UCSR0B = 0b00001000; //desativar as interrupcoes relacionadas a USART, ativar somente trasmissor e desativar o nono bit de dado
	UCSR0C = 0b00110110; //para USART assincrona, paridade impar, um bit de parada, 8 bit de dados
	
	asm("sei"); //habiita as interrupções
	
	DDRB |= 0b00110000; //configura as portas dos leds (PB4 e PB5) para saida
	
    while (1) 
    {		
			//TRANSMISSAO DA MENSAGEM
			while (x < max_x){ 
				//o buffer de transmissao TXB só pode ser escrito se a flag UDRE0 (bit 5 de UCSR0A) estiver setada
				if ((UCSR0A & 0b00100000) == 0b00100000) {
					UDR0 = msg[x];
					x++;
				}
			}
		//ao final da tramissao x devera voltar para a posição inicial para trasmitir novamente na próxima vez a mensagem		
		x = 0;
		_delay_ms(5000);	
	}
}

