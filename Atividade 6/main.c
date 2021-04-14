/*
 * Atividade6.c
 *
 * Author : Bryan Wolff
 * RA: 214095
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

//registradores usados para definicao do Baud Rate
unsigned char *pUBRR0H = (unsigned char*) 0xC5;
unsigned char *pUBRR0L  = (unsigned char*) 0xC4;

//registradores de confiuracao e status
unsigned char *pUCSR0A = (unsigned char*) 0xC0;
unsigned char *pUCSR0B = (unsigned char*) 0xC1;
unsigned char *pUCSR0C = (unsigned char*) 0xC2;

//registrador usado para a transferência de dados
unsigned char *pUDR0 = (unsigned char*) 0xC6;

//cadeia de caracteres para ser transmitida
char msg[] = "Out of the night that covers me,\n Black as the Pit from pole to pole,\n I thank whatever gods may be\n For my unconquerable soul.\n In the fell clutch of circumstance\n I have not winced nor cried aloud.\n Under the bludgeonings of chance\n My head is bloody, but unbowed.\n Beyond this place of wrath and tears\n Looms but the Horror of the shade,\n And yet the menace of the years\n Finds, and shall find, me unafraid.\n It matters not how strait the gate,\n How charged with punishments the scroll.\n I am the master of my fate:\n I am the captain of my soul.\n";

//mensagem de confirmação
char confirm[] = "Mensagem transmitida com sucesso!\n";

//variaveis usadas para percorrer os elementos do vetor das mensagens
unsigned int x = 0; //associada a msg
unsigned int y = 0; //associada a confirm

//variaveis usadas para referencia do tamanho das mensagens para saber o maximo onde x e y deve percorrer para parar
unsigned int max_x = sizeof(msg);
unsigned int max_y = sizeof(confirm);

int main(void)
{
	//como o baud rate eh de 38400, isto eh, 0x0019 em hexadecimal
	//entao 0x00 ira para UBRR0H e 0x19 ira para UBRR0L
	*pUBRR0H = 0x00;
	*pUBRR0L = 0x19;
	
	//configuracao dos registradores de configuracao e status
	*pUCSR0A &= 0b11111100; //velocidade normal, desativar o modo de comunicacao multi-processador
	*pUCSR0B = 0b00001000; //desativar as interrupcoes relacionadas a USART, ativar somente trasmissor e desativar o nono bit de dado
	*pUCSR0C = 0b00110110; //para USART assincrona, paridade impar, um bit de parada, 8 bit de dados	
   
    while (1){		
		while (x < max_x){ //transimssao da msg
		
		//o buffer de transmissao TXB só pode ser escrito se a flag UDRE0 (bit 5 de UCSR0A) estiver setada
		if ((*pUCSR0A & 0b00100000) == 0b00100000) 
			{				
				*pUDR0 = msg[x];
				x++;
			}
		}	
		while (y < max_y) //transmissao da confirmacao
		{
			//o buffer de transmissao TXB só pode ser escrito se a flag UDRE0 (bit 5 de UCSR0A) estiver setada
			if ((*pUCSR0A & 0b00100000) == 0b00100000) 
			{
				*pUDR0 = confirm[y];
				y++;
			}
		}
	x = 0;
	y = 0;
	_delay_ms(5000);		
	}
}


