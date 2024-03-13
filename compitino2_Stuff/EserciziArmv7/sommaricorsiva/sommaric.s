	@
	@ siano a e b due interi con b>=0, somma(a,b) e' definita come:
	@   somma(a, 0) = a
	@   somma(a,b)  = somma(a+1,b-1)
	@
	@ NOTA: a e b sono numeri piccoli!
	@
	.data	
fmt:	.string "risultato: %d\n"
fmt2:	.string "inserisci un numero:\n"
buf:	.fill 16	@ buffer da 16 byte
	
	.text
	.global main
	.type main, %function
	.type somma, %function
	.type leggi, %function

main:
	push {r4, lr}		@ salvo i registri non temporanei che usero'
	bl leggi		@ leggo il primo intero
	mov r4, r0		@ me lo ricordo in r4
	bl leggi		@ leggo il secondo intero
	mov r1, r0		@ in r1 metto il secondo (b)
	mov r0, r4		@ in r0 metto il primo (a)
	bl somma		@ sommo
	mov r1, r0		@ preparo i parametri della printf
	ldr r0, =fmt	
	bl printf		@ stampo il risultato
	mov r0, #0		@ ritorno successo
	pop {r4, pc}		@ ripristino i registri e ritorno

@ --------------------------------------------------------	
leggi:
	push {r4, r7, lr}	@ salvo i registri non temporanei che usero'
	ldr r0, =fmt2		@ preparo il messaggio di stampa
	bl printf
	mov r0, #0		@ metto in r0 0 (cioe' il descrittore dello stdin)
	ldr r1, =buf		@ in r1 l'indirizzo del buffer
	mov r4, r1		@ mi ricordo l'indirizzo del buffer in r4
	mov r2, #16		@ in r2 il numero massimo di byte da leggere 
	mov r7, #3		@ in r7 l'indice della syscall
	svc 0			@ eseguo la syscall
	cmp r0, #-1		@ controllo se e' fallita
	bleq exit		@ chiamo la funzione di libreria exit(-1)
	mov r0, r4		@ preparo il parametro per atoi
	bl atoi			@ converto la stringa in intero, risultato in r0
	pop {r4, r7, pc}	@ ritorno ripristinando i registri



	@ --------------------------------------------------------	
	@ in r0 ho a, in r1 ho b
somma:	cmp r1, #0		@ b==0? 
	moveq pc,lr		@ la somma e' in r0
	push {lr}		@ salvo l'indirizzo di ritorno
	add r0, r0, #1		@ a+1
	sub r1, r1, #1		@ b-1
	bl somma		@ somma(a+1, b-1)
	pop {pc}		@ restituisce il risultato in r0
	
