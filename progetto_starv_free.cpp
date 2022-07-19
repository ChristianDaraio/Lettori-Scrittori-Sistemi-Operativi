#include<pthread.h>
# include<bits/stdc++.h>
#include<semaphore.h>
#include <unistd.h>
#include<stdio.h>
using namespace std;


void schermoIniziale();
void *lettori(void *);
void *scrittori(void *);
float generaRandom(int min, int max);


// variabili globali
const int MAX_LETTORI = 20;
const int MAX_SCRITTORI = 5;
int risorsa_condivisa = 15, conta_lettori = 0;

sem_t sem_contatore; // semaforo utilizzato nell'aggiornamento di "conta_lettori"
sem_t sem_verifica;  //semaforo utilizzato per verificare se i lettori/scrittori sono ammessi o meno alla sezione critica.
sem_t sem_starv; // un semaforo aggiuntivo per evitare la priorità ai lettori e prevenire la starvation.
 
//Memorizzazione degli ID thread presupponendo un massimo di 20 lettori e 5 scrittori.
pthread_t l[20],s[5];


float generaRandom(int min, int max) {
    float x = rand() / (float)RAND_MAX;
    return (min + x) * (max + min);
}

void schermoIniziale() {
    printf("+----------------------------------------------------BENVENUTO--------------------------------------------------+\n");
    printf("|Questa applicazione simula un sistema di controllo delle tariffe di un biglietto aereo in continuo aggiornamento|\n");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");

}

//LETTORI
void *lettori(void *i){
    sleep(generaRandom(0, 5)); // sleep per simulare un ritardo di lettura
    //ZONA_INGRESSO
    
    //in attesa del rilascio di altri lettori o scrittori    
    sem_wait(&sem_starv);

    printf("L'utente n. %d e' nella ZONA_INGRESSO\n",i);
    sem_wait(&sem_contatore);
    conta_lettori++;
    if(conta_lettori==1){
        sem_wait(&sem_verifica); //attende se lo scrittore sta scrivedo
    }
    
    sem_post(&sem_contatore);
    sem_post(&sem_starv);    

    //ZONA_CRITICA
    printf("L'utente n. %d sta visualizzando la tariffa del biglietto con destinazione <Londra>. Costo biglietto = %d euro\n",i,risorsa_condivisa);

    //ZONA_USCITA

    printf("L'utente n. %d e' nella ZONA_USCITA\n",i);
    sem_wait(&sem_contatore);
    conta_lettori--;
    if(conta_lettori==0){
        sem_post(&sem_verifica);
    }

    sem_post(&sem_contatore);

}

// SCRITTORI
void *scrittori(void *i){
    sleep(generaRandom(0, 5)); // sleep per simulare un ritardo di lettura
    //ZONA_INGRESSO
    //attende che il lettore in esecuzione termini di aggiornare "conta_lettori" o che il thread scrittore termini
    
    sem_wait(&sem_starv); 
    printf("L'operatore n. %d e' nella ZONA_INGRESSO\n",i);

    //attende se qualche lettore sta leggendo o se lo scrittore sta scrivendo
    sem_wait(&sem_verifica);

    //ZONA_CRITICA

    risorsa_condivisa = rand() %100 + 15; // generazione casuale di un nuovo importo tra 15 e 115
    printf("L'operatore n. %d della compagnia aerea ha aggiornato il costo del biglietto per Londra. Valore = %d euro\n",i,risorsa_condivisa);
    
    //ZONA_USCITA

    printf("L'operatore n. %d e' nella ZONA_USCITA\n",i);

    sem_post(&sem_verifica);
    sem_post(&sem_starv);
}


int main(){
    //creazione seme per successiva generazione numero random
    srand(time(NULL));
    schermoIniziale();
    //inizializzazione semafori
    sem_init(&sem_contatore,0,1);
    sem_init(&sem_verifica,0,1);
    sem_init(&sem_starv,0,1);

    //Creazione dei threads
    for(unsigned long i = 0; i < MAX_LETTORI; i++) {
         pthread_create(&l[i],NULL,lettori,(void *)i);
    }
    for(unsigned long j = 0; j < MAX_SCRITTORI; j++) {
         pthread_create(&s[j],NULL,scrittori,(void *)j);
    }
    
    //Join sui threads
    for(int i=0; i < MAX_LETTORI; i++) {
         pthread_join(l[i],NULL);
    }
    for(int j=0; j < MAX_SCRITTORI; j++) {
         pthread_join(s[j],NULL);
    }
    
return 0;
}