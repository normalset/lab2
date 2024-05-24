//typedef messaggio , unviare solo i byte significativi del messaggio e non un biffer di lunghezza fissa
typedef struct messaggio {
    char type ; 
    unsigned int length ; //0 nel caso in cui il campo data non e' significativo
    char * data ; //dati effettivi
}messaggio ; 

//riceve un messaggio in formato csv e restituisce il messaggio del messaggio
messaggio read_msg(int);
//scrive un messaggio sulla socket
void write_msg(int , char , char * );