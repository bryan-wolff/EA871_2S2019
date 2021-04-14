#include<stdio.h>
char buffer[5];
int i,j,k,x;
void adicionar_buffer(char c){
  buffer[i]=c;
  if(i==5)i=0;
  i++;
  k++;
}
void remover_buffer() {
  buffer[j]=0;
  if(j==5)j=0;
  j++;
  k--;
}
void imprimir_buffer() { 
  int x = j;
  do{
    printf("%c",buffer[x]);
    if(x==5)x=0;
    if(x!=i-1)printf(" ");
    
    x++;
  }while(x!=i);
  printf("\n");
}
int main() {
  char c;
  do {
  scanf("%c",&c);
    if (c == '\n') break;
    if((47<c)&&(c<58)&&(k!=5))adicionar_buffer(c);  
    if(((64<c) && (c<91)) || ((96<c)&&(c<122))&&(k!=0))remover_buffer();
    if(k==0)printf("\n");
    if(k!=0)imprimir_buffer();
 }while(1);
 return 0;
}
