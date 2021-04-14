/*
 * Atividade5.c
 *
 * Created: 18/09/2019 14:21:07
 * Author : bryan
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL // 16MHz clock speed
#include <util/delay.h>
#include <avr/interrupt.h> 

extern void DELAY (short int q);

volatile int t = 0;		//variavel usada para percorrer o vetor do tempo do delay
volatile int B = 0;	//muda ao for pressionado o botao B, 0 para horario, 1 para anti-horario
unsigned char *ppinc = (unsigned char*) 0x26; //PINC

ISR(INT0_vect) {		//Rotina de Serviço de Interrupção do Botao A associada ao INT0
	
	//incrementa a variavel t para referencia de qual tempo será usado
	t = t + 1;
	if (t==4) t = 0; // será zerado se exceder
}

ISR(PCINT1_vect) {		//Rotina de Serviço de Interrupção do Botao B associada ao PC0
	//muda o sentido: horario b=0, anti-horario b=1
			
	if ((*ppinc & 0x01)==0x00){		//ativar na borda de descida 
		if (B == 0) B = 1; 
		else B = 0;
	}
}
int main(void)
{		
    	unsigned char *pddrc = (unsigned char*) 0x27;	//PDDRC
    	unsigned char *pddrd = (unsigned char*) 0x2a;	//DDRD
		unsigned char *pddrb = (unsigned char*) 0x24;	//DDRB
		unsigned char *pportb = (unsigned char*) 0x25;	//PORTB
		
		unsigned char *pEICRA = (unsigned char*) 0x69;	//External Interrupt Control Register A
		unsigned char *pEIMSK = (unsigned char*) 0x3d;	//external interrupt mask register
		unsigned char *pPCICR = (unsigned char*) 0x68;  //pin change interrupt control register
		unsigned char *pPCMSK1 = (unsigned char*) 0x6c; //pin change mask register 1
		
		unsigned int l = 0;		//variavel usar para percorrer o vetor das cores do led
		
		//vetor com as cores do led RGB na ordem: apagado, vermelho, verde, azul, amarelo, ciano, magenta e branco.
		unsigned char led[8] = {0b00000000,0b00000001,0b00000010,0b00000100,0b0000011,0b00000110,0b00000101,0b00000111};
		
		//configuração dos registratores DDR
		*pddrc &= 0b11111110; //zerar bit 0 para corrente de entrada do Botao B 
		*pddrb |= 0b00000111; //setar bits 0 ao 2 para saida da corrente para o LED RGB
		*pddrd &= 0b11111011; //zerar bit 2 PORTD para corrente de entrada do Botão A 
		
		//configuração da interrupção externa INT0
		*pEICRA |= 0b00000011; //seta os dois bits menos significativos e a INT0 será ativada na borda de subida do pino 2 da porta D
		*pEIMSK |= 0b00000001; //seta o bit menos significativo e habilita a interrupção INT0
		
		//configuração interrupção externa PCINT8 - PC0
		*pPCICR |= 0b00000010;    //Ativa interrupção no PORTC
		*pPCMSK1 |= 0b00000001;    //Ativa interrupção somente no pino PC0 que é PCINT8
		asm("sei"); //liga as interrupções
		
    while (1) 
    {
		*pportb = led[l]; //aqui acenderá a cor do led conforme a variavel "l" for percorrida entre 0 e 7
		
		//aqui será chamado o delay conforme o valor da variavel t
		//t = 0 para 1000ms, 1 para 500ms, 2 para 250ms e 3 para 125ms
		if (t==0) DELAY(1000);
		else if (t==1) DELAY(500);
		else if (t==2) DELAY(250);
		else DELAY(125);
		
		if (B==1) { //se o sentido for anti-horario, "l" devera decrementar p/ percorrer as cores no sentido anti-horario
				if (l==0) l = 8; //se "l" for chegar em 0, sera definido como 8 para decrementar corretamente no ciclo de cores existente
			l = l - 1;
		}
		else {	//se o sentido for horario, "l" devera incrementar p/ percorrer as cores no sentido horario
				l = l + 1;
			if (l==8) l = 0; //se "l" chegar em 8, sera definido como 0 para incrementar corretamente no ciclo de cores existente
		}
	}
}

