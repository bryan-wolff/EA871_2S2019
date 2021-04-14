;
; Atividade 4 assembly.asm
;
; Created: 05/09/2019 08:47:12
; Author : Bryan Wolff RA: 214095
;
.cseg
sbi DDRB,5 ;configura DDRB para acender o LED

rotina_zero: ;rotina responsavel pelo backup dos registradores que foram utilizados
	push r16
	push r17
	push r18
	push r19
	push r20
	push r24
	push r25
	call piscar
	pop r25
	pop r24
	pop r20
	pop r19
	pop r18
	pop r17
	pop r16
	rjmp rotina_zero

piscar: ;rotina responsavel por chamar o delay, acender e apagar o led	
	sbi PORTB,5
	call delay
	cbi PORTB,5
	call delay
	ret

	delay:		;passagem do parametro do delay
		ldi r24,0xf4	;aqui é carregado o parametro menos significativo do delay
		ldi r25,0x01	;aqui é carregado o parametro mais significativo do delay

			delay_1ms: ; aqui é uma rotina de 1ms de delay
						ldi r20,1 ;esse valor define quantos ms essa rotina vai gastar aprox. então esse parametro é variavel, mas usaremos 1ms
					start:
						dec r20  ; diminui o valor de r20, que contém o número de vezes que o programa será executado
						cpi r20,255  ; compara com 255
						breq dec_parametro_menos_significativo ; se for igual o programa vai decrementar o parametro
						ldi r17,7 ; carrega o valor de parametro inicial de r17
						ldi r18,2 ; carrega o valor de parametro inicial de r18
						ldi r19,6 ; carrega o valor de parametro incial de r19
					loop:
						dec r18; inicia o loop decrementando o valor de r18
						cpi r18,0; caso r18 for igual a zero o loop 4 será executado, se não o loop 2 será chamado
						breq loop4
					loop2:
						dec r17; decrementa r17
						ldi r16,245 ; carrega 245 em r16
						cpi r17,0 ;compara r17 com 0
						breq loop ;se for diferente de zero prossegue como o decremento de r16 se não ele volta ao loop
					loop3:
						dec r16 ; decrementa r16 
						cpi r16,0 ; compara com zero
						breq loop2 ; vai voltar decrementar r16 de novo
						rjmp loop3 ; executa o pulo
					loop4:
						dec r19 ; decrementa o valor de r19
						cpi r19,0; compara com zero
						breq start  ; se for igual a zero retorna para o começo do programa
						ldi r16,247 ; se for diferente atribui 252 a r16
					loop5:
						dec r16	; decrementa o valor de r16
						inc r16	;foi incrementado e decrementado para adicionar dois clocks ao loop5
						dec r16
						cpi r16,0 ; compara com zero
						breq loop4 ; se for igual volta a loop 4 para decrementar r19
						rjmp loop5 ; se for diferente executa loop 5
				dec_parametro_mais_significativo:	;aqui decrementa o parametro mais significativo (r25) setando os menos significativos (r24)
					dec r25
					ldi r24,255
					cpi r25,255
					breq end	;se acabar os paramentros mais significativos o delay ja foi finalizado
					rjmp delay_1ms ;caso não acabe, o programa repete o delay de 1ms
				dec_parametro_menos_significativo: ;rotina que chamara a rotina de delay de 1ms a quantidade de vezes que o parametro menos significativo (r24 )exigir
					dec r24
					cpi r24,0	;se acabar os parametros menos significativos, será chamado a rotida para decrementar o parametro + significativo
					breq dec_parametro_mais_significativo
					rjmp delay_1ms ;caso não acabe, o programa repete o delay de 1ms
				end:	ret