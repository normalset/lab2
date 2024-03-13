	.text
	.global mygetenv
	.type	mygetenv, %function
	
	@ argomenti: r0 contiene la stringa che cerco
	@ 	     r1 contiene l'ambiente (envp)

mygetenv:
	cmp r0, #0 			@ sanity check, se r0==NULL ritorno NULL.
						@ Lo contiene già, quindi non devo fare niente 
	moveq pc, lr		@ ritorno NULL
	cmp r1, #0			@ sanity check
	moveq r0, #0		@ Se r0==NULL ritorno NULL
	moveq pc, lr		@ ritorno NULL

	push {r4-r8, lr}       	@ registri da preservare

	@ calcolo la lunghezza (len) della stringa in ingresso

	mov r3, #0		@ contatore (len=0)
loop:
	ldrb r2, [r0, r3]	@ leggo il prossimo carattere di s1
	cmp r2, #0			@ e' \0?
	addne r3, r3, #1	@ no incremento il contatore
	bne loop			@ continuo

	@ uso i registri da r4 ad r8 per salvarmi i dati che mi servono
	
	mov r4, r0		@ r4 contiene la stringa da cercare
	mov r5, r1		@ r5 contiene l'ambiente
	mov r6, r3		@ r6 contiene len
					@ r7 lo uso per ricordarmi left 
					@ r8 lo uso per ricordarmi right
	
while:
	ldr r0, [r5], #4	@ carico in r0 env[i] (e' un puntatore). Post incremento
	cmp r0, #0		@ siamo alla fine (l'ultima entry di env e' NULL)
	beq notfound
	mov r1, #61		@ '=' ha codice 61 (vedere man ascii)
	bl mystrtok
	cmp r0, #0		@ 
	beq while		@ errore, vado avanti 
	mov r7, r0		@ salvo left (cioe' la chiave di ricerca)

	mov r0, #0		@ seconda chiamata a mystrtok con NULL r1 contiene sempre '='
	bl mystrtok
	cmp r0, #0
	beq while		@ errore, vado avanti 
	mov r8, r0		@ salvo right (cioe' il valore)

	@ devo chiamare myncmp, preparo i parametri	
	mov r0, r4		@ setto la stringa da cercare
	mov r1, r7		@ setto in r1 left 
	mov r2, r6		@ setto in r2 la lunghezza
	bl myncmp
	cmp r0, #0		@ trovata?
	bne while		@ no vado avanti

	@ trovata 
	mov r0, r8		@ setto il valore di ritorno a right
	pop {r4-r8, pc} @ ritorno pulendo lo stack
	
notfound:
	mov r0, #0		@ mi assicuro che in r0 ci sia NULL
	pop {r4-r8, pc} @ ritorno pulendo lo stack
		

