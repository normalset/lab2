	@
	@ Stampa le stringhe contenuto nel vettore envp
	@ passato come terzo argomento del main.
	@ envp e' un vettore di stringhe in cui nell'ultima
	@ posizione c'e' NULL.
	@
		.data
fmt:	.string "%s\n"
		.text
		.global main
		.type main, %function

		@ r0 contiene argc, r1 contiene argv, r2 contiene envp
main:
		push {r4, lr} 	@ mi salvo i registri non temporanei che usero'	
		mov r4, r2	@ r4 lo uso per ricordarmi l'indirizzo base di envp

loop:
		ldr r1,[r4],#4	@ equivale a r1=*envp++  (post-incremento)
		cmp r1, #0      @ r1==NULL? 
		beq end         @ ho finito
		ldr r0, =fmt	@ preparo il primo argomento della printf
		bl printf       @ r0 conterra' il valore di ritorno della printf,
						@ inoltre printf potrebbe usare tutti i registri
						@ temporanei (r0-r3 ed r12) e non ci
						@ posso fare affidamento.
		b loop 			
end:
		mov r0, #0      @ setto il valore di ritorno del main
		pop {r4, pc}	@ ritorno
