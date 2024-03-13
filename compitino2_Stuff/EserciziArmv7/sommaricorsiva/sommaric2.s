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
fmt3:   .string "%d"
a:	.word 10
b:	.word 20

	.text
	.global main
	.type main, %function
	.type somma, %function
	.type leggi, %function

main:
	push {r4, r5, r6, lr}	@ salvo i registri non temporanei che usero'
	ldr r0, =fmt2		@ preparo il messaggio di stampa: metto l'indirizzo di fmt2 in r0
	mov r6, r0		@ salvo l'indirizzo di fmt2 in r6 per usarlo nella seconda print
	bl printf		@ chiamo la printf per stampare "inserisci un numero:\n"
	ldr r0, =fmt3  		@ preparo in r0 la stringa di formattazione per leggere un intero dallo stdin
	ldr r1, =a 		@ metto in r1 l'indirizzo dell'intero a in cui memorizzare l'intero letto dallo stdin
	mov r5, r1		@ memorizzo l'indirizzo di a per non doverlo ricaricare dalla memoria, da usare dopo la seconda scanf, per invocare la funzione di somma
	mov r4, r0		@ memorizzo l'indirizzo della stringa di formattazione in r4 per non doverlo ricaricare dalla memoria
	bl scanf		@ chiamo la scanf per leggere a, senza controllare eventuali errori
	mov r0, r6		@ metto in r0 l'indirizzo della stringa di formattazione per la printf
	bl printf		@ chiamo nuovamente la printf per stampare "inserisci un numero:\n"
	mov r0, r4		@ muovo in r0 l'indirizzo della stringa di formattazione per la scanf
	ldr r1, =b 		@ metto in r1 l'indirizzo dell'intero b in cui memorizzare l'intero letto dallo stdin
	bl scanf		@ chiamo la scanf per leggere b, senza controllare eventuali errori
	ldr r0,[r5]		@ carico in r0 il numero a, l'indirizzo di a è in r5
	ldr r1, =b
	ldr r1,[r1]		@ carico in r1 il numero b, l'indirizzo di b è già in r1
	bl somma		@ sommo
	mov r1, r0		@ preparo i parametri della printf: muovo il risultato della somma ( in r0 ) in r1
	ldr r0, =fmt		@ metto l'indirizzo di fmt in r0
	bl printf		@ stampo il risultato
	mov r0, #0		@ ritorno successo
	pop {r4, r5, r6, pc}	@ ripristino i registri e ritorno
	@ NOTA: non uso r2, r3 e r12 al posto di r4, r5 ed r6 perché, non essendo preservati, dovrei salvarli e recuperarli prima e dopo ogni chiamata alla scanf e alla printf
	@ --------------------------------------------------------
	@ in r0 ho a, in r1 ho b
somma:
	cmp r1, #0		@ b==0?
	moveq pc,lr		@ la somma e' in r0
	push {lr}		@ salvo l'indirizzo di ritorno
	add r0, r0, #1	@ a+1
	sub r1, r1, #1	@ b-1
	bl somma		@ somma(a+1, b-1)
	pop {pc}		@ restituisce il risultato in r0
