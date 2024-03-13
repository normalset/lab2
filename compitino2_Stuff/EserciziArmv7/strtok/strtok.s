	.data
	@ memory word dove viene mantenuto lo stato della tokenizzazione
	@ tra chiamate successive, contiene il puntatore ad un carattere
	@ ed indica il punto da dove riprendere, se e' NULL il parsing e' finito 
stato:	.word 0
	
	.text
	.global mystrtok
	.type mystrtok, %function

	@ r0 contiene la stringa (o NULL nelle chiamate successive)
	@ r1 contiene il carattere di separazione
	@ r2 usato per leggere un carattere
	@ r12 contiene il puntatore allo stato
	
mystrtok:				@ sanity checks
	cmp r1, #0			@ il separatore non deve essere NULL
	moveq r0, #0   	    @ valore di ritorno NULL 
	moveq pc, lr 		@ return NULL

	ldr r12, =stato		@ carico il puntatore allo stato in r12
	cmp r0, #0			@ se l'argomento non e' NULL, comincia un nuovo parsing
	bne nuova			@ salto alla gestione di una nuova stringa
	ldr r0, [r12]		@ altrimenti, riprendo da dove ero arrivato (s=stato)
	cmp r0, #0			@ se non c'è parsing in sospeso ho finito
	moveq pc,lr			@ ritorno

nuova:
	ldrb r2, [r0]		@ carico un carattere dalla stringa
	cmp r2, r1			@ e' un separatore?
	bne iniziotok		@ no, cerco il prossimo separatore per identificare un token  
	add r0, #1			@ si, vado avanti
	b nuova				@ vado avanti finche' vedo un separatore

iniziotok:  			@ devo vedere se per caso è finita subito la stringa
						@(es. "token" o "token;;;")
	cmp r2, #0			@ verifico se la stringa è terminata
	moveq r0, #0		@ devo restituire NULL
	streq r0, [r12]		@ e salvare NULL nello stato per le prossime chiamate
	moveq pc, lr		@ ritorno

	@ la stringa non e' finita, 
	@ uso r3 per cercare il prossimo separatore

	add r3, r0, #1		@ mi sposto di uno
loop:
	ldrb r2, [r3]		@ carico il prossimo carattere
	cmp r2, r1			@ e' un separatore?
	beq separat			@ trovato separatore
	cmp r2, #0			@ no, sono a fine stringa?
	beq slash0			@ trovato fine stringa
	add r3,r3,#1		@ incremento il puntatore alla stringa 
	b loop				@ continuo
		
separat:  				@ token completo delimitato dal separatore
	mov r2, #0			@ preparo il valore di ritorno
	strb r2, [r3]		@ scrivo '\0' al posto del separatore nella stringa
	add r3,r3, #1		@ nuova posizione da dove riparte 
	str r3, [r12]		@ scrivo la nuova posizione nello stato
	mov pc, lr			@ r0 conteneva la posizione di inizio token, ritorno
	
	@ token completo terminato da fine stringa
	@ non ci sono altri token 
slash0:
	mov r3, #0			@ parsing completato 
	str r3, [r12]		@ salvo NULL nella variabile stato per le prossime chiamate
	mov pc, lr			@ r0 conteneva la posizione di inizio token, ritorno
	
