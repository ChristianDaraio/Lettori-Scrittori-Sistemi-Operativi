#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_LETTORI 10
#define MAX_SCRITTORI 5

int costo_biglietto = 15; // risorsa condivisa
int numero_lettori = 0; // numero lettori attivi
pthread_mutex_t mutex; // protegge l'accesso alla variabile numero_lettori
sem_t semaforo_scrittura;

float generaRandom(float min, float max) {
    float x = rand() / (float)RAND_MAX;
    return min + x * (max - min);
}

void schermoIniziale() {
    printf("+----------------------------------------------------BENVENUTO--------------------------------------------------+\n");
    printf("|Questa applicazione simula un sistema di controllo delle tariffe di un biglietto aereo in continuo aggiornamento|\n");
    printf("+----------------------------------------------------------------------------------------------------------------+\n");

}

void *lettore(void *id_thread) {
    sleep(generaRandom(1, 3)); // sleep per simulare un ritardo di lettura
    unsigned long tid = (unsigned long)id_thread;
    pthread_mutex_lock(&mutex); // inizio sezione critica
    numero_lettori++;
    if (numero_lettori == 1) {
        // verifica se si tratta del primo lettore.Esso attende che termini la fase di scrittura
        sem_wait(&semaforo_scrittura);
    }
    pthread_mutex_unlock(&mutex); // fine sezione critica
    printf("L'utente n.%lu sta visualizzando la tariffa del biglietto con destinazione <Londra>. Costo biglietto -> %d euro\n", tid, costo_biglietto);
    pthread_mutex_lock(&mutex);
    numero_lettori--;
    if (numero_lettori == 0) {
        // verifica se si tratta dell'ultimo lettore.In caso affermativo rilascia il semaforo per l'operazione di scrittura.
        sem_post(&semaforo_scrittura);
    }
    pthread_mutex_unlock(&mutex);
}

void *scrittore(void *id_thread) {
    sleep(generaRandom(0, 3)); // sleep per simulare un ritardo di scrittura
    unsigned long tid = (unsigned long)id_thread;
    sem_wait(&semaforo_scrittura); // semaforo di attesa per la fase di scrittura
    costo_biglietto = rand() %100 + 15; // generazione casuale di un nuovo importo tra 15 e 115
    printf("L'operatore n.%lu della compagnia aerea ha aggiornato il costo del biglietto per Londra\n", tid);
    sem_post(&semaforo_scrittura); // rilascio del semaforo
}

int main() {
    srand(time(NULL)); //creazione seme per successiva generazione numero random
    schermoIniziale();
    pthread_t lettori[MAX_LETTORI], scrittori[MAX_SCRITTORI]; // creazione dei descrittori dei threads
    pthread_mutex_init(&mutex, NULL); //inizializzazione del mutex
    sem_init(&semaforo_scrittura, 0, 1);  // inizializzazione del semaforo

    // Creazione Threads (dipendenti)
     for (unsigned long i = 0; i < MAX_SCRITTORI; i++) {
        pthread_create(&scrittori[i], NULL, scrittore, (void *)i);
    }

    // Creazione Threads (utenti)

    for (unsigned long i = 0; i < MAX_LETTORI; i++) {
        pthread_create(&lettori[i], NULL, lettore, (void *)i);
    }

    // Join sui threads in esecuzione
    for (unsigned long j = 0; j < MAX_SCRITTORI; j++) {
        pthread_join(scrittori[j], NULL);
    }

    for (unsigned long h = 0; h < MAX_LETTORI; h++) {
        pthread_join(lettori[h], NULL);
    }
    // Libera le risorse
    sem_destroy(&semaforo_scrittura);
    pthread_mutex_destroy(&mutex);
    return 0;
}
