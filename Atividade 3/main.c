/*
 * Atividade 3.c
 *
 * Created: 03/09/2019 21:26:22
 * Author : Bryan Wolff 
 * RA: 214095
 */ 
#define F_CPU 16000000UL // 16MHz clock speed
#include <util/delay.h> 
unsigned char *pddrc = (unsigned char*) 0x27; //DDRC
unsigned char *ppinc = (unsigned char*) 0x26; //PINC
unsigned char *pddrd = (unsigned char*) 0x2a; //DDRD
unsigned char *pportd = (unsigned char*) 0x2b; //PORTD
unsigned char *p_ucsr0b = (unsigned char*) 0xc1; //VART
unsigned int estado = 0; //define que numero deve estar no display
unsigned int botao = 1; //define se o botao foi pressionado alguma vez, 0 para sim e 1 para nao
unsigned char led = 0; //variavel intermediaria que carregará qual valor deve estar no portd
unsigned char *pportc = (unsigned char*) 0x28; //para configurar o pullup


int main(void)
{
*pddrc = *pddrc & 0b11111110; //zerar bit 0 para a corrente de entrada do botão
*pddrd = *pddrd | 0b11111110; //setar bits 1 ao 7 para saida da corrente para o display
*p_ucsr0b = 0; // desativa VART (pino TX sempre alto)
*pportc = *pportc | 0b00000001; //setar o bit 1 para configurar o pullup

while (1) {
	// verifica se o botao foi pressionado
	 if ((*ppinc & 0x01)==0x00) {
		_delay_ms(50); // delay de 50 ms
		if ((*ppinc & 0x01)==0x01) botao = 0;	//se o botão voltou para o repouso, entao ele foi pressionado
		}
		else botao = 1;	 //caso o botao nao foi pressionado

 switch (estado) {
/* Cada caso refere ao numero que o display deve mostrar, e caso o botao for pressionado o estado
sera incrementado e o display vai para o proximo caso que mostrara o proximo numero no display
caso não for pressionado, ficara um loop no mesmo estado para que permaneca os mesmos leds acesos no display*/

//Configuração dos LEDs do Display do PORTD (observação: como nao usamaremos o bit 0, as mascaras foram escolhidas de modo a não altera-lo)
// (bit 1 = a) -- (bit 2 = b)  --  (bit 3 = c)  --  (bit 4 = d)  --  (bit 5 = e)  --  (bit 6 = f)  --  (bit 7 = g)
//a variavel led eh utilizada como intermedio, para que as operações em tempo real de | e & nao afetem o display

		 case 0:
 			if (botao == 1){
			led = ((*pportd | 0b01111110) & 0b01111111); //para acender os leds abcdef sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
 			}
 			else if (botao == 0) estado = 1;
 			break;
 		case 1:
 			if (botao == 1){
			led = ((*pportd | 0b00001100) & 0b00001101); //para acender os leds  bc sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
 			}
 			else if (botao == 0) estado = 2;
 			break; 
 		case 2:
 			if (botao == 1){
			led = ((*pportd | 0b10110110) & 0b10110111); //para acender os leds abged sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
 			}
 			else if (botao == 0) estado = 3;
 			break; 
 		case 3:
 			if (botao == 1){
			led = ((*pportd | 0b10011110) & 0b10011111); //para acender os leds abgcd sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
 			}
 			else if (botao == 0) estado = 4;
 			break;			 
 		case 4:
 			if (botao == 1){
			led = ((*pportd | 0b11001100) & 0b11001101); //para acender os leds fgbc sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
 			}
 			else if (botao == 0) estado = 5;
 			break;
 		case 5:
 			if (botao == 1){
			led = ((*pportd | 0b11011010) & 0b11011011); //para acender os leds afgcd sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
 			}
 			else if (botao == 0) estado = 6;
 			break;
 		case 6:
 			if (botao == 1){
			led = ((*pportd | 0b11111010) & 0b11111011); //para acender os leds afgcde sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
  			}
 			else if (botao == 0) estado = 7;
			break;
 		case 7:
 			if (botao == 1){
			led = ((*pportd | 0b00001110) & 0b00001111); //para acender os leds abc sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
 			}
 			else if (botao == 0) estado = 8;
 			break;
 		case 8:
 			if (botao == 1){
			led = ((*pportd | 0b11111110) & 0b11111111); //para acender os leds abcdefg sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
 			}
 			else if (botao == 0) estado = 9;
 			break;
 		case 9:
 			if (botao == 1){
			led = ((*pportd | 0b11011110) & 0b11011111); //para acender os leds abgfcd sem mecher no bit 0 pois nao estamos usando
			*pportd = led;
 			}
 			else if (botao == 0) estado = 0;
 			break;
			}
	}
}
