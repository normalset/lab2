	@
	@ Riceve in input un intero (lettura da standard input) e stampa la
	@ posizione dei bit ad 1 partendo dal bit piu' significativo (da
	@ sinistra verso destra). Il bit piu' significativo e' quello in
	@ posizione 0, il bit meno significativo e' quello in posizione 31.
	@
	.data
fmt1:	.string "%d "
	
	.text
	.global printbitad1
	.type printbitad1, %function

	@ in r0 abbiamo l'intero 'b'
printbitad1:
	push {r4-r6,lr}		@ salvo i registri non temporanei che usero'
	mov r4, r0		@ in r4 mi ricordo l'intero 'b'
	mov r5, #1		@ r5 lo uso per la variabile k
	mov r5, r5, lsl #31	@ k = 1<<31
	mov r6, #0		@ r6 e' indice del for i

for:	
	cmp r6, #32		@ ho finito?
	popeq {r4-r6, pc}	@ ripristino i registri non temporani e ritorno
	andS r0, r4, r5		@ (b & k)
	cmp r0, #0		@ controllo r0 
	ldrne r0, =fmt1		@ in r0 c'e' 1, stampo la posizione
	movne r1, r6		@ r1 <--- i
	blne printf		@ stampo
	mov r5, r5, lsr #1	@ k = k >> 1	
	add r6, r6, #1		@ ++i
	b for			@ continuo
	

