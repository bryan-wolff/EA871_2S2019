/* Contador de bits
 *
 * Este programa conta o numero de bits em um byte
 */

#include <stdio.h>

int main() {

  unsigned char entrada;
  unsigned int tmp;
  unsigned int n_bits;
  int i = 0;
  char d = 0x01; /* sera usado como mascara para isolar o bit */
  char c = 0x00; /* assumira o valor apenas do bit analisado, e os outros ser�o zerados, ent�o inicialmente defino como 0 */

  /* Ler entrada em hexadecimal */
  scanf("%x", &tmp);
  entrada = (unsigned char)tmp;

n_bits = 0;

/* a fun��o for verificar� um bit por vez, portanto ser�o feitas 8 analises
a analise ser� feita com o operador & (and) entre a entrada e a mascara, e sera carregado na variavel c este resultado
se c for igual a mascara, quer dizer que o bit analisado � 1, ent�o devemos incrementar a contagem
no final de cada analise, multiplico a mascara por 2 para assumir a mascar� do proximo bit para ser analisado posteriormente

 */
for (i = 0; i < 8; i=i+1) {
	c = entrada & d;
	if (c==d) n_bits = n_bits + 1;
	d = d*2;
 }

  /* Escrever numero de bits */
  printf("%d\n", n_bits);
  return 0;
}
