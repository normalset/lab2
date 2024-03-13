	.text
	.global myncmp
	.type	myncmp, %function
	
	@ argomenti: r0 contiene s1, r1 contiene s2, r2 contiene n
myncmp:	
	push {r4, r5}			@ salvo i registri non temporanei che usero'
							@ non uso funzioni quindi non mi salvo LR	
	mov r3, #0				@ r3 conterra' la lunghezza di s1
loop1:	ldrb r5, [r0, r3]	@ leggo il prossimo carattere di s1
	cmp r5, #0				@ e' \0?
	addne r3, r3, #1		@ no incremento il contatore
	bne loop1				@ continuo

	mov r4, #0				@ r4 conterra' la lunghezza di s2	
loop2:	
	ldrb r5, [r1, r4]		@ leggo il prossimo carattere di s2
	cmp r5, #0				@ e' \0?
	addne r4, r4, #1		@ no incremento il contatore
	bne loop2				@ continuo

	cmp r4, r3              @ r4 - r3 
	movlo r3, r4            @ se r4<r3, in r3 mi tengo la lunghezza minima (min)
	cmp r3, r2				@ eseguo r3 - r2
	blo diverse				@ se r3<r2 (cioe' min<n), allora sono necessariamente diverse 
	cmp r3, r2              @ min < n ?	
	movlo r2, r3			@ aggiorno n con il min se  n>min
	
	mov r3, #0 				@ ora r3 lo uso per l'indice del for (i)
for:	
	cmp r3, r2				@ i - n 
	beq uguali				@ i == n?
	ldrb r4, [r0], #1 		@ leggo s1[i]  (post incremento -> modifico r0)
	ldrb r5, [r1], #1		@ leggo s2[i]  (post incremento -> modifico r1)
	cmp r4, r5              @ r4 < r5?
	bne diverse				@ le stringhe sono diverse
	add r3, r3, #1 			@ i++
	b for					@ continuo
		
uguali:
	mov r0, #0				@ preparo il valore di ritorno 0 (uguali)
	pop {r4,r5}				@ ripristino i registri non temporanei
	mov pc, lr				@ ritorno (return 0)
diverse:
	mov r0, #1				@ preparo il valore di ritorno 1 (diverse)
	pop {r4,r5}				@ ripristino i registri non temporanei
	mov pc, lr				@ ritorno (return 1)


