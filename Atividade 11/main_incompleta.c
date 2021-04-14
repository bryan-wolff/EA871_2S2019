#include <avr/io.h>
#include <avr/interrupt.h>


/*EQUAÇÃO DO TEMPO:

((ICRL * (periodo do relógio)) + (numero de interrupções * (500*(tempo máximo de contagem))));

onde, 
- ICRL é o valor copiado do temporizador no momento em que o usuário apertou o botão;
- periodo do relogio = 256/16000000;
- numero de interrupções é a quantidade de vezes em que ocorreu a interrupção do temporizador entre o momento do led acender e a pessoa apertar o botão;
- tempo máximo de contagem = (256/16000000)* 500, em que 500 foi um valor calculado;
*/

int tempo1 = 0;                       //variavel que conta o tempo;
float contador = 0;                   //variavel que conta a quantidade de interruoções após o led acender;
float demora;                         //variavel que recebe o valor de resposta do usuário;
float periodo = 0.008;                //periodo de interrupção do temporizador;
float relogio = 0.000016;             //clock;

/*################### FUNÇÃO DE CONFIGURAÇÃO #######################*/

void configuracao (){
  
  Serial.begin(115200);               //configura taxa de transmissão (baud rate) em 115200 bps;
  DDRB = 0x20;                        //define pino 13 (bit 5 da porta B) como saída e pino 8 (bit 0 da Porta B) como entrada;
  
  TCCR1A = 0x00;                      //configura modo CTC;
  TCCR1B = 0b01001100;                //bit 6 habilita 'input capture edge select', bit 3 termina a configuração modo CTC e bits 0,1,2 define prescaler de 256;
  OCR1AH = 0x01;                      //configura o registrador de comparação do temporizador para 499;
  OCR1AL = 0xf3;                      // 
  TIMSK1 = 0b00100010;                //habilita interrupção input capture e interrupção por comparação;
  
  asm("sei");                         //habilita todas as interrupções;
}

/*################ PROGRAMA PRINCIPAL ##############*/

int main(void)
{
    configuracao();                                                                //chama função de configuração;
  
    while (1) 
    {
    }
}

/*############### INTERRUPÇÃO DO TEMPORIZADOR #############*/

ISR(TIMER1_COMPA_vect){
  
  tempo1++;                                                                         //incrementa variável tempo;
  contador++;                                                                       //incrementa variável contador;
  
  if(tempo1 == 500){                                                                //após 4s (quando tempo1 for igual a 500);
    PORTB = 0x20;                                                                   //o led acende;
    contador = 0;                                                                   //contador é zerado para que apartir daqui a quantidade das interrupções seja usada para determinar o tempo de reação;
  }
  
  if(tempo1 == 750){                                                                //se o tempo1 atigir o valor de 750 isso quer dizer que se passaram mais 2s (6s - 4s = 2s);
    PORTB = 0x00;                                                                   //logo, o led apaga;
    Serial.println("Evento não detectado: Preste mais atenção no LED!\0");          //e uma mensagem é transmitida indicando que o usuário não apertou o botão; 
    tempo1 = 0;                                                                     //tempo1 é zerado para que o programa recomece;
    contador = 0;                                                                   //contador também é zerado;
  }
}

/*############### iNTERRUPÇÃO DO BOTÃO ###############*/

ISR(TIMER1_CAPT_vect){
  
  if((PINB & 0x01) == 0x01){                                                        //se o botão for precionado;
    
    PORTB = 0x00;                                                                   //o led apaga;
    demora = (((float ICR1)* relogio) + (contador* periodo));                       //a variável demora recebe o valor do tempo de resposta du usuário ao ver o led acender;
    Serial.print(demora,3);                                                         //é transmitido o tempo de resposta com três casas decimais;
    Serial.println("s");                                                            //transmite unidade do tempo;
    tempo1 = 0;                                                                     //zera a variavel tempo1 para reiniciar o programa;
    contador = 0;                                                                   //o contador também é zerdao.
    
  }
}