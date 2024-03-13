	@
	@ versione lasciata per esercizio che implementa la funzione strlen
	@ strlen viene invocata 2 volte per la lunghezza di s1 e di s2
	@
	.text
	.global myncmp
	.type	myncmp, %function
	.type   strlen, %function
	
	@ argomenti: r0 contiene s1, r1 contiene s2, r2 contiene n
myncmp:	
	push {r4-r7, lr}	@ salvo i registri non temporanei che usero'
	
	mov r4, r0			@ mi salvo s1
	mov r5, r1			@ mi salvo s2
	mov r6, r2			@ mi salvo n

	bl strlen			@ calcolo la lunghezza di s1
	mov r7, r0			@ la memorizzo temporaneamente in r7

	mov r0, r5			@ preparo per calcolare strlen(s2)
	bl strlen			@ calcolo la lunghezza di s2
	mov r12, r0			@ la memorizzo temporaneamente in r12
	mov r0, r4			@ ripristino i valori iniziali
	mov r1, r5
	mov r2, r6
	
	mov r3,	r7			@ r3 contiene strlen(s1)
	mov r4, r12			@ r4 contiene strlen(s2)
	
	cmp r4, r3          @ r4 - r3 
	movlo r3, r4        @ se r4<r3, in r3 mi tengo la lunghezza minima (min)
	cmp r3, r2			@ eseguo r3 - r2
	blo diverse			@ se r3<r2 (cioe' min<n), allora sono necessariamente diverse 
	movlo r2, r3		@ aggiorno n con il min se  n>min
	
	mov r3, #0 			@ ora r3 lo riuso per l'indice del for (i)
for:	
	cmp r3, r2			@ i - n 
	beq uguali			@ i == n?
	ldrb r4, [r0], #1 	@ leggo s1[i]  (post incremento -> modifico r0)
	ldrb r5, [r1], #1	@ leggo s2[i]  (post incremento -> modifico r1)
	cmp r4, r5          @ r4 < r5?
	bne diverse			@ le stringhe sono diverse
	add r3, r3, #1 		@ i++
	b for				@ continuo
		
uguali:
	mov r0, #0			@ preparo il valore di ritorno 0 (uguali)
	pop {r4-r7, pc}		@ ripristino i registri non temporanei e ritorno

diverse:
	mov r0, #1			@ preparo il valore di ritorno 1 (diverse)
	pop {r4-r7, pc}		@ ripristino i registri non temporanei e ritorno

@ ------------------------ strlen ----------------------------------
strlen:
	mov r3, #0
loop:
	ldrb r1, [r0, r3]	@ leggo il prossimo carattere: pre-incremento
	cmp r1, #0		@ e' \0?
	addne r3, r3, #1	@ no incremento il contatore
	bne loop		@ continuo
	mov r0, r3		@ valore di ritorno
	mov pc, lr		@ ritorno
