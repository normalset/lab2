	.text
	.global Mergesort
	.type	Mergesort, %function
@ r0: a, r1: p, r2: r
Mergesort:
	cmp r1, r2 @ controllo p>r
	bgt fine	@ se p < r
	add r3, r1, r2 @aggiungo in r3
	mov r3, r3, lsr#2 @divido per due

	push {r0,r1,r2,r3,lr} @salvo i valori
	mov r2, r3 @sposto r3 in r2
	bl Mergesort	@richiamo la funzione
	pop {r0,r1,r2,r3,pc}	@riprendo i valori dallo stack
	
	push {r0,r1,r2,r3,lr} @salvo i valori sullo stack
	add r2, r3, #1 @metto in r2=p -> q+1
	bl Mergesort @richiamo la funzione
	pop {r0,r1,r2,r3,pc} @riprendo i valori dallo stack
	
	@preparo i registri per la chiamata di merge
	@ora r0 = a, r1= p , r2= r, r3 = q
	@devo avere r0 a, r1 p, r2 q, r3 r
	mov r4, r2	@uso r4 come temp per lo swap
	mov r2, r3
	mov r3, r4
	push {lr} @salvo il link register prima della chiamata
	bl merge @chiamo la funzione merge
	pop {pc} @ripristino il link register pre chiamata nel program counter
	

fine:
	mov pc, lr @chiudo la chiamata di funzione