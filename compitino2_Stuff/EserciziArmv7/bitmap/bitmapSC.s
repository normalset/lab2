	@
	@ Riceve in input un intero (lettura da standard input) e stampa la
	@ posizione dei bit ad 1 partendo dal bit piu' significativo (da
	@ sinistra verso destra). Il bit piu' significativo e' quello in
	@ posizione 0, il bit meno significativo e' quello in posizione 31.
	@
	.data
fmt0:	.string "inserisci un numero:\n"
fmt1:	.string "%d "
buf:	.fill 16	@ buffer da 16 byte
	
	.text
	.global main
	.type main, %function
	.type printbitad1, %function


main:	push {r4, r7, lr}	@ salvo i registri non temporanei che usero'
	ldr r0, =fmt0		@ carico il parametro della printf
	bl printf		@ eseguo la printf

	mov r0, #0		@ descrittore 0, standard input
	ldr r1, =buf		@ indirizzo di buf
	mov r4, r1		@ me lo ricordo in r4, mi servira' nella atoi
	mov r2, #16  		@ size (massima) in byte da leggere
	mov r7, #3		@ system call read
	svc 0			@ eseguo read(0, &buf, 16)

	cmp r0, #-1		@ controllo che non sia fallita
	popeq {r4, r7, pc}	@ ripristino i registri e ritorno -1
	mov r0, r4		@ preparo il parametro per la atoi
	bl atoi			@ invoco atoi, in r0 avro' l'intero in input
	bl printbitad1		@ in r0 ho l'intero 'n', invoco la stampa
	mov r0, #10		@ codice '\n' (man ascii)
	bl putchar		@ stampo '\n'
	mov r0, #0		@ ritorno successo (return 0)
	pop {r4,r7,pc}		@ ripristino i registri e ritorno

