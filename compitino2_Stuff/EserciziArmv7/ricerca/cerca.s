	.text
	.global cerca
	.type cerca, %function

	@ r0 contiene x, r1 contiene *v, r2 contiene i, r3 contiene j
cerca:  
	cmp r2, r3				@ i - j 
	movgt r0, #-1			@ i>j, preparo -1 in r0. GT sta per greater than, per numeri con segno 
	movgt pc, lr			@ ritorno -1 (elemento non trovato)

	push {r0-r4, lr}		@ mi salvo i valori di ingresso ed r4, perché devo andare in ricorsione
	add r12, r2, r3			@ calcolo (i+j)
	lsr r12, r12, #1		@ m = (i+j)/2

	ldr r4, [r1,r12,lsl #2]	@ r4 <-- V[m], shift logico a sinistra di 2, cioè moltiplico per 4
	cmp r4, r0				@ r4 - r0
	beq  ritorna			@ trovato?
	addlt r2, r12, #1		@ se V[m] < x calcolo m+1 in r2. LT sta per less than, per numeri con segno
	subgt r3, r12, #1		@ se V[m] > x calcolo m-1 in r3
	bl cerca				@ continuo a cercare
	mov r12, r0				@ mi salvo il valore di ritorno

ritorna: 					@ in r12 ho il valore di ritorno che e' l'indice che contiene x o -1
	pop {r0-r4, lr}			@ ripristino lo stack	
	mov r0, r12				@ preparo il valore di ritorno
	mov pc, lr				@ ritorno

