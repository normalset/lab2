	.data
fmt1:	.string "%d "
fmt2:	.string "\n"

	.text
	.global insertL
	.global printL
	.global destroyL
	.type insertL,  %function	
	.type printL,   %function
	.type destroyL, %function

@ --------------------------- insertL -------------------------------------------		
	@ r0 contiene L (elem_t **L), r1 contiene l'elemento da inserire (x)
insertL:
	push {r4-r6, lr}	@ salvo i registri non temporanei che usero'

	ldr r2, [r0]		@ metto in r2 il puntatore *L, il cui indirizzo è in r0
	cmp r2, #0			@ *L == NULL?
	beq instesta		@ se sì, inserisco in testa ( la lista è vuota )
	ldr r3, [r2]		@ metto in r3 (*L)->e, che si trova nella struct ad offset 0
	cmp r3, r1			@ e - x ?
	bge instesta		@ se e >= x, inserisco in testa ( elemento più piccolo o uguale al primo )
	mov r4, r2			@ *L in r4, r4 puntera' sempre all'elemento corrente
	mov r6, r1			@ mi ricordo x in r6

whileIL:
	ldr r0, [r4, #4]	@ carico s->next in r0
	cmp r0, #0			@ siamo alla fine?
	beq insfondo		@ si, inserisco in fondo

	ldr r1, [r0]		@ carico 'e' in r1
	cmp r1, r6			@ e - x ?
	movlt r4, r0		@ se e < x, s = s->next
	blt whileIL			@ se e < x, continuo
	@ in r4 ho s (l'elemento corrente)
	mov r5, r0			@ mi ricordo il valore di s->next in r5	per evitare di rileggerlo
	mov r0, #8			@ Preparo il parametro della malloc
						@ Devo allocare 8 bytes, supposta dimensione della struct
						@ 4 bytes per l'intero, 4 bytes per l'indirizzo al prossimo elemento
	bl malloc			@ in r0 mi restituisce il puntatore 'l'
	str r6, [r0]		@ l->e = x
	str r5, [r0, #4]	@ l->next = s->next
	str r0, [r4, #4]	@ s->next = l
	pop {r4-r6, pc}		@ ritorno
	
instesta:
	mov r4, r0			@ mi ricordo **L in r4
	mov r5, r2			@ mi ricordo *L in r5
	mov r6, r1			@ mi ricordo x in r6
	mov r0,	#8			@ sizeof(element_t)==8
	bl malloc			@ in r0 mi restituisce il puntatore l
	str r6, [r0]		@ l->e = x
	mov r1, r5			@ in r1 metto *L (che puo' essere NULL se la lista e vuota)
	str r1, [r0, #4]	@ l->next = *L
	str r0, [r4]		@ *L = l	
	pop {r4-r6, pc}		@ ritorno
	@ in r4 ho s (l'elemento corrente)
insfondo:
	mov r0, #8			@ devo allocare 8 bytes
	bl malloc			@ in r0 mi restituisce il puntatore l
	str r6, [r0]		@ l->e = x
	mov r1, #0			@ NULL in r1
	str r1, [r0, #4]	@ l->next = NULL
	str r0, [r4, #4]	@ s->next = l
	pop {r4-r6, pc}		@ ritorno

@ --------------------------- printL --------------------------------------------	
	@ r0 contiene L (elem_t *L)
printL:	
	push {r4, lr}
	mov r4, r0		@ r4 contiene L
whileL:
	cmp r4, #0		@ L e' NULL?
	beq fineprintL		@ si ho finito
	ldr r1, [r4], #4	@ r1 <- L->e e r4 <- (L->next)
	ldr r0, =fmt1		@ carico in r0 il formato per la printf
	bl printf		@ stampo
	ldr r4, [r4]		@ carico in r4 'e' L->next  (L=L->next)
	b whileL		@ continuo
		
fineprintL:
 	ldr r0, =fmt2		@ stampo '\n'
 	bl printf
 	pop {r4, pc}		@ ritorno
		
@ --------------------------- destroyL ------------------------------------------
	@ r0 contiene L (elem_t **L)

destroyL:
	cmp r0, #0		@ sanity check
	moveq pc, lr		@ ritorno

	push {r4,r5, lr}	@ salvo i registri non temporanei che usero'
	ldr r4, [r0]		@ in r4 carico *L (r4 e' la variabile s)
	mov r1, #0		@ r1 <- NULL
	str r1, [r0]		@ *L = NULL
	
whileDL:
	cmp r4, #0		@ s == 0 ?
	popeq {r4,r5, pc}	@ si ritorno
	ldr r5, [r4, #4]	@ r5 = s->next
	mov r0, r4		@ preparo il parametro della free 
	bl free			@ lo elimino
	mov r4, r5		@ s = s->next
	b whileDL