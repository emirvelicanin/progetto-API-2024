// PROGETTO API DI  EMIR: 
//ricette: hash 
// magazzino:hash  
//ordini:  lista ( per ordini non completati) e albero rosso nero (completati in ordine di data)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct nodo_ingrediente { // qua ho messo le diverse quantità e scadenza dell'ingrediente
    int quantita;
    int scadenza;
    struct nodo_ingrediente* prossimo;
} nodo_ingrediente;

typedef struct ingrediente_nel_magazzino {
    char nome[20];
    nodo_ingrediente* ingrediente;
    struct ingrediente_nel_magazzino* prossimo;
} ingrediente_nel_magazzino;

typedef struct Ingrediente_ricetta {
    int quantita;
    ingrediente_nel_magazzino* ingrediente_nel_magazzino;// Puntatore  alla tabella hash del magazzino 
    struct Ingrediente_ricetta* prossimo;//mi serve per creare una lista per gli ingredienti della ricetta 
    // in caso di  collisioni con la tabella hash
} Ingrediente_ricetta;

typedef struct nodo_ricetta {
    char nome[20];
    int peso_ricetta;
    Ingrediente_ricetta* ingrediente;// ingredienti della ricetta
    struct nodo_ricetta* prossimo;
} nodo_ricetta;

typedef struct Tabella_hash {
    nodo_ricetta** puntatore;
} Tabella_hash;

typedef struct nodo_ordine {
    int peso;
    int tempo_arrivo;
    char nome_ordine[20];
    int quantita;
    struct nodo_ordine* prossimo;
} nodo_ordine;

typedef struct ordine {
    nodo_ricetta* nodo_ricetta; //puntatore alla ricetta nella tabella hash delle ricette
    int quantita;
    int tempo_arrivo;
    struct ordine* prossimo;
} ordine;// mi serve per la lista degli ordini in sospeso

typedef struct lista_ordini_in_sospeso {
    ordine* testa;
    ordine* coda;
} lista_ordini_in_sospeso;


// struttura per crearmi il nodo dell' albero rosso nero 
//per ordini completati, in ordine di data di data crescente
typedef struct nodo_albero {
    int peso; // Peso totale dell'ordine 
    int tempo_arrivo; 
    char nome_ordine[20]; 
    int quantita; // quantita ordinata
    struct nodo_albero* sinistra;
    struct nodo_albero* destra;
    struct nodo_albero* parente;
    int colore; 
} nodo_albero;

typedef struct albero_rosso_nero_per_ordine {
    nodo_albero* radice;
    nodo_albero* nil ;
} albero_rosso_nero_per_ordine;


typedef struct magazzino_hash {
    ingrediente_nel_magazzino** puntatore;
} magazzino_hash;

typedef struct Corriere {
    int intervallo_di_arrivo;
    int capacita_massima;
} Corriere;

// FUNZIONI PER RICETTE
long hash(char* stringa) { //funzione hash 
    unsigned  long hash = 5381;
    int num;
    while ((num = *stringa++)) {
        hash = ((hash << 5) + hash) + num;
    }
    return hash % 1569;}
//creo la  tabella hash
Tabella_hash* crea_tabella_hash() {
    Tabella_hash* tabella_hash = (Tabella_hash*)malloc(sizeof(Tabella_hash));
    tabella_hash->puntatore = (nodo_ricetta**)calloc(1569, sizeof(nodo_ricetta*));
    //creo  array e inizializzo a 0 le variabili con calloc, dimensione 1569
    return tabella_hash;}



void inserisci_ricetta(Tabella_hash* tabella_hash,    char* nome_ordine, nodo_ricetta** nuovo_nodo_puntatore) {
    long indice = hash(nome_ordine);
    nodo_ricetta* nodo_nuovo = (nodo_ricetta*)malloc(sizeof(nodo_ricetta));
    nodo_nuovo->ingrediente = NULL;
    nodo_nuovo->peso_ricetta = 0;  //imposto peso iniziale a  zero
    strcpy(nodo_nuovo->nome, nome_ordine);
    nodo_nuovo->prossimo = tabella_hash->puntatore[indice]; // inserisco all'inizio 
    tabella_hash->puntatore[indice] = nodo_nuovo;
    // Restituisco il nuovo nodo con nuovo_nodo_puntatore
    *nuovo_nodo_puntatore = nodo_nuovo;

}

//ricerca ricetta 
nodo_ricetta* cerca_ricetta(Tabella_hash* tabella_hash,    char* nome_ordine) {
    long indice = hash(nome_ordine);
    nodo_ricetta* nodo_ricerca = tabella_hash->puntatore[indice];
    while (nodo_ricerca != NULL) {
        if (strcmp(nodo_ricerca->nome, nome_ordine) == 0) {// ricetta trovata 
            return nodo_ricerca;  }
        nodo_ricerca= nodo_ricerca->prossimo;// scorro la lista 
    }
    return NULL;
}
//elimina ricetta
void elimina_ricetta(Tabella_hash* tabella_hash,    char* nome_ordine) {
    long indice = hash(nome_ordine);
    nodo_ricetta* prec = NULL;
    nodo_ricetta* nodo_attuale = tabella_hash->puntatore[indice];
   

    while (nodo_attuale != NULL && strcmp(nodo_attuale->nome, nome_ordine) != 0) {
        prec = nodo_attuale;
        nodo_attuale = nodo_attuale->prossimo;}

    if (nodo_attuale == NULL) {
        return;  // Ricetta non trovata
    }
//ho trovato la ricetta e ora la elimino 
    if (prec == NULL) {
        tabella_hash->puntatore[indice] = nodo_attuale->prossimo;
    } else {
        prec->prossimo = nodo_attuale->prossimo;}

//elimino i diversi ingredienti della ricetta 
    Ingrediente_ricetta* ingrediente = nodo_attuale->ingrediente;
    while (ingrediente != NULL) {
        Ingrediente_ricetta* temp = ingrediente;
        ingrediente = ingrediente->prossimo;
        free(temp); }

    free(nodo_attuale);
}

//free tabella hash, alla fine del programma


void aggiungi_ingrediente(nodo_ricetta* nodo_ricetta, ingrediente_nel_magazzino* ingrediente_nel_magazzino, int quantita) {
    Ingrediente_ricetta* nuovo_ingrediente = (Ingrediente_ricetta*)malloc(sizeof(Ingrediente_ricetta));
    nuovo_ingrediente->ingrediente_nel_magazzino = ingrediente_nel_magazzino;  // puntatore  al nodo del magazzino 
    nuovo_ingrediente->quantita = quantita;
    nuovo_ingrediente->prossimo = nodo_ricetta->ingrediente;
    nodo_ricetta->ingrediente = nuovo_ingrediente;//lo metto come primo elemento della lista degli ingredienti della ricetta  
}

 // funzioni per magazzino .. sempre tabella hash.... funzioni simili a quelle per le ricette 

magazzino_hash* crea_magazzino() {
    magazzino_hash* tabella_hash = (magazzino_hash*)malloc(sizeof(magazzino_hash));
    tabella_hash->puntatore = (ingrediente_nel_magazzino**)calloc(1569, sizeof(ingrediente_nel_magazzino*));
    return tabella_hash;}

//inserisci ingrediente nel magazzino
void inserici_ingrediente_magazzino(nodo_ingrediente** testa, int quantita, int scadenza) {
    nodo_ingrediente* nodo_nuovo = (nodo_ingrediente*)malloc(sizeof(nodo_ingrediente));
    nodo_nuovo->quantita = quantita;
    nodo_nuovo->scadenza = scadenza;
    nodo_nuovo->prossimo = NULL;

    // Se la lista è vuota o l'ingrediente  ha una data di scadenza precedente al primo elemento nella lista
    if (*testa == NULL || (*testa)->scadenza > scadenza) {
        nodo_nuovo->prossimo = *testa;
        *testa = nodo_nuovo;
    } else {
        nodo_ingrediente* nodo_attuale = *testa;

        // Trovo la posizione corretta per inserire il nuovo ingrediente mantenendo l'ordine di scadenza
        while (nodo_attuale->prossimo != NULL && nodo_attuale->prossimo->scadenza < scadenza) {
            nodo_attuale = nodo_attuale->prossimo;
        }

        nodo_nuovo->prossimo = nodo_attuale->prossimo;
        nodo_attuale->prossimo = nodo_nuovo;}}


//cerca elemento nel magazzino
ingrediente_nel_magazzino* cerca_elemento_magazzino(magazzino_hash* tabella_hash,  char* nome) {
   

    long indice = hash(nome);
    ingrediente_nel_magazzino* nodo_ricerca = tabella_hash->puntatore[indice];
    while (nodo_ricerca != NULL) {//scorro la lista 
        if (strcmp(nodo_ricerca->nome, nome) == 0) {
            return nodo_ricerca;}
        nodo_ricerca = nodo_ricerca->prossimo; }
    return NULL;
}

void rifornimento_ingrediente(magazzino_hash* tabella_hash,    char* nome, int quantita, int scadenza) {

    long indice = hash(nome);
    ingrediente_nel_magazzino* nodo_attuale = tabella_hash->puntatore[indice];

    while (nodo_attuale != NULL) {
        if (strcmp(nodo_attuale->nome, nome) == 0) {
            inserici_ingrediente_magazzino(&(nodo_attuale->ingrediente), quantita, scadenza);
            return;}
        nodo_attuale = nodo_attuale->prossimo;}
    
// nel caso non è presente l'ingrediente nel magazzino 
    ingrediente_nel_magazzino* nodo_nuovo = (ingrediente_nel_magazzino*)malloc(sizeof(ingrediente_nel_magazzino));
    strcpy(nodo_nuovo->nome, nome);
    nodo_nuovo->ingrediente = NULL;
    inserici_ingrediente_magazzino(&(nodo_nuovo->ingrediente), quantita, scadenza);
    nodo_nuovo->prossimo = tabella_hash->puntatore[indice];
    tabella_hash->puntatore[indice] = nodo_nuovo;}

void rimuovi__scaduti(magazzino_hash* tabella_hash, int data_attuale) {
    for (int i = 0; i < 1569; i++) {//scorro tutta la tabella hash
        ingrediente_nel_magazzino* nodo_attuale = tabella_hash->puntatore[i];

        while (nodo_attuale != NULL) {
            nodo_ingrediente* Ingrediente = nodo_attuale->ingrediente;
            nodo_ingrediente* ingrediente_precedente = NULL;

            while (Ingrediente != NULL) {
                if (Ingrediente->scadenza <= data_attuale) {//da cancellare 
                    nodo_ingrediente* cancella = Ingrediente;
                    if (ingrediente_precedente == NULL) {
                        nodo_attuale->ingrediente = Ingrediente->prossimo;//l'elemento successivo diventa il primo
                    } else {
                        ingrediente_precedente->prossimo = Ingrediente->prossimo;
                    }
                    Ingrediente = Ingrediente->prossimo;
                    free(cancella);
                } else {
                    ingrediente_precedente = Ingrediente;
                    Ingrediente = Ingrediente->prossimo;}}

            nodo_attuale = nodo_attuale->prossimo;}}}
//controllo se l'ordine può essere  preparato   e se si, lo preparo e  cancello gli ingredienti usati
bool ordine_preparabile(nodo_ricetta* nodo_ricetta, int ordinequantita, int data_attuale) {

    Ingrediente_ricetta* controllo_ingrediente_ricetta = nodo_ricetta->ingrediente;

 // controllo se disponibile
    while (controllo_ingrediente_ricetta != NULL) {
        int quantita_necessaria = controllo_ingrediente_ricetta->quantita * ordinequantita;
        ingrediente_nel_magazzino* ingrediente_nel_magazzino = controllo_ingrediente_ricetta->ingrediente_nel_magazzino;

        if (ingrediente_nel_magazzino == NULL) {
            return false; }

        int quantita_disponibile = 0;
        nodo_ingrediente* nodo_ingrediente = ingrediente_nel_magazzino->ingrediente;

        // Scorro fino a trovare un ingrediente non scaduto
        while (nodo_ingrediente != NULL && nodo_ingrediente->scadenza <= data_attuale) {
            nodo_ingrediente = nodo_ingrediente->prossimo;}

       
        while (nodo_ingrediente != NULL) {
            quantita_disponibile += nodo_ingrediente->quantita;
            if (quantita_disponibile >= quantita_necessaria) {
                break; // finisci  se hai abbastanza quantita disponibile
            }
            nodo_ingrediente = nodo_ingrediente->prossimo;}

        if (quantita_disponibile < quantita_necessaria) {
            return false; // non abbastanza ingredienti
        }

        controllo_ingrediente_ricetta = controllo_ingrediente_ricetta->prossimo;
        // vado all'ingrediente successivo della ricetta 
    }

    //  Consumo  ingredienti
    controllo_ingrediente_ricetta = nodo_ricetta->ingrediente;
    while (controllo_ingrediente_ricetta != NULL) {
        int quantita_necessaria = controllo_ingrediente_ricetta->quantita * ordinequantita;
        ingrediente_nel_magazzino* ingrediente_nel_magazzino = controllo_ingrediente_ricetta->ingrediente_nel_magazzino;

        nodo_ingrediente* nodo__ingrediente = ingrediente_nel_magazzino->ingrediente;
        nodo_ingrediente* precendente = NULL;

        // Scorri fino a trovare un ingrediente non scaduto
        while (nodo__ingrediente != NULL && nodo__ingrediente->scadenza <= data_attuale) {
            precendente = nodo__ingrediente;
            nodo__ingrediente = nodo__ingrediente->prossimo; }
        // Consuma gli Ingredientei non scaduti
        while (quantita_necessaria > 0 && nodo__ingrediente != NULL) {
            if (nodo__ingrediente->quantita <= quantita_necessaria) {// consumo tutto l'ingrediente 
                quantita_necessaria -= nodo__ingrediente->quantita;
                nodo_ingrediente* cancella = nodo__ingrediente;
                nodo__ingrediente = nodo__ingrediente->prossimo;//passo all'ingrediente dopo
                free(cancella);

                // Aggiorna lista
                if (precendente == NULL) {
                    ingrediente_nel_magazzino->ingrediente = nodo__ingrediente;
                } else {
                    precendente->prossimo = nodo__ingrediente;
                }} else {
                nodo__ingrediente->quantita -= quantita_necessaria;
                quantita_necessaria = 0;}}

        controllo_ingrediente_ricetta = controllo_ingrediente_ricetta->prossimo;
    }

    return true;// ordine preparato
}

// crea  albero  per gli ordini completati in ordine di data ( pseudocodice presi dal libro Cormen)
nodo_albero* crea_nodo_per_ordine(int peso, int tempo_arrivo,    char* nome_ordine, int quantita) {
    nodo_albero* nodo_nuovo = (nodo_albero*)malloc(sizeof(nodo_albero));
    nodo_nuovo->peso = peso;
    nodo_nuovo->tempo_arrivo = tempo_arrivo;
    strcpy(nodo_nuovo->nome_ordine, nome_ordine);
    nodo_nuovo->quantita = quantita;
    nodo_nuovo->sinistra = NULL;
    nodo_nuovo->destra = NULL;
    nodo_nuovo->parente = NULL;
    nodo_nuovo->colore = 1; 
    return nodo_nuovo;}

void inizializza_albero(albero_rosso_nero_per_ordine* albero) {
    albero->nil  = (nodo_albero*)malloc(sizeof(nodo_albero));
    albero->nil ->peso = 0;
    albero->nil ->tempo_arrivo = 0;
    strcpy(albero->nil ->nome_ordine, "");
    albero->nil ->quantita = 0;
    albero->nil ->sinistra =NULL;
    albero->nil ->destra = NULL;
    albero->nil ->parente = NULL;
    albero->nil ->colore = 0; // Nero
    albero->radice = albero->nil ;}


void ruota_sinistrs(albero_rosso_nero_per_ordine* albero, nodo_albero* x) {
    nodo_albero* y = x->destra;
    x->destra = y->sinistra;
    if (y->sinistra != albero->nil ) {
        y->sinistra->parente = x;
    }
    y->parente = x->parente;
    if (x->parente == albero->nil ) {
        albero->radice = y;
    } else if (x == x->parente->sinistra) {
        x->parente->sinistra = y;
    } else {
        x->parente->destra = y;}
    y->sinistra = x;
    x->parente = y;}


void ruota_destra(albero_rosso_nero_per_ordine* albero, nodo_albero* x) {
    nodo_albero* y = x->sinistra;
    x->sinistra = y->destra;
    if (y->destra != albero->nil ) {
        y->destra->parente = x;
    }
    y->parente = x->parente;
    if (x->parente == albero->nil ) {
        albero->radice = y;
    } else if (x == x->parente->destra) {
        x->parente->destra = y;
    } else {
        x->parente->sinistra = y;}
    y->destra = x;
    x->parente = y;}



void aggiusta_inserimento_albero(albero_rosso_nero_per_ordine* albero, nodo_albero* nodo) {
    while (nodo->parente->colore == 1) {
        if (nodo->parente == nodo->parente->parente->sinistra) {
            nodo_albero* zio = nodo->parente->parente->destra;
            if (zio->colore == 1) {
                nodo->parente->colore = 0;
                zio->colore = 0;
                nodo->parente->parente->colore = 1;
                nodo = nodo->parente->parente;
            } else {
                if (nodo == nodo->parente->destra) {
                    nodo = nodo->parente;
                    ruota_sinistrs(albero, nodo);
                }
                nodo->parente->colore = 0;
                nodo->parente->parente->colore = 1;
                ruota_destra(albero, nodo->parente->parente);
            }
        } else {
            nodo_albero* zio = nodo->parente->parente->sinistra;
            if (zio->colore == 1) {
                nodo->parente->colore = 0;
                zio->colore = 0;
                nodo->parente->parente->colore = 1;
                nodo = nodo->parente->parente;
            } else {
                if (nodo == nodo->parente->sinistra) {
                    nodo = nodo->parente;
                    ruota_destra(albero, nodo);
                }
                nodo->parente->colore = 0;
                nodo->parente->parente->colore = 1;
                ruota_sinistrs(albero, nodo->parente->parente);}}}
    albero->radice->colore = 0;}


// inserire  in un albero in ordine di data crescente 
void inserisci_albero(albero_rosso_nero_per_ordine* albero, int peso, int tempo_arrivo,    char* nome_ordine, int quantita) {
    nodo_albero* nodo_nuovo = crea_nodo_per_ordine(peso, tempo_arrivo, nome_ordine, quantita);
    nodo_albero* x = albero->radice;
    nodo_albero* y = albero->nil ;

    while (x != albero->nil ) {
        y = x;
        if (tempo_arrivo < x->tempo_arrivo) {//controlli data
            x = x->sinistra;
        } else {
            x = x->destra;}}
    nodo_nuovo->parente = y;
    if (y == albero->nil ) {
        albero->radice = nodo_nuovo;
    } else if (tempo_arrivo < y->tempo_arrivo) {
        y->sinistra = nodo_nuovo;
    } else {
        y->destra = nodo_nuovo;}

    nodo_nuovo->sinistra = albero->nil ;
    nodo_nuovo->destra = albero->nil ;
    nodo_nuovo->colore = 1; 

    aggiusta_inserimento_albero(albero, nodo_nuovo);}
void rbTransplant2(albero_rosso_nero_per_ordine* albero, nodo_albero* u, nodo_albero* v) {
    
    if (u->parente == albero->nil ) {
        albero->radice = v;
    } else if (u == u->parente->sinistra) {
        u->parente->sinistra = v;
    } else {
        u->parente->destra = v;}
    
    v->parente = u->parente;
}


void ellimina_aggiusta(albero_rosso_nero_per_ordine* albero, nodo_albero* x) {
    nodo_albero* w;
    while (x != albero->radice && x->colore == 0) {
        if (x == x->parente->sinistra) {
            w = x->parente->destra;
            if (w->colore == 1) {
                w->colore = 0;
                x->parente->colore = 1;
                ruota_sinistrs(albero, x->parente);
                w = x->parente->destra;}

            if (w->sinistra->colore == 0 && w->destra->colore == 0) {
                w->colore = 1;
                x = x->parente;
            } else {
                if (w->destra->colore == 0) {
                    w->sinistra->colore = 0;
                    w->colore = 1;
                    ruota_destra(albero, w);
                    w = x->parente->destra;
                }

                w->colore = x->parente->colore;
                x->parente->colore = 0;
                w->destra->colore = 0;
                ruota_sinistrs(albero, x->parente);
                x = albero->radice;
            }
        } else {
            w = x->parente->sinistra;
            if (w->colore == 1) {
                w->colore = 0;
                x->parente->colore = 1;
                ruota_destra(albero, x->parente);
                w = x->parente->sinistra;}

            if (w->destra->colore == 0 && w->sinistra->colore == 0) {
                w->colore = 1;
                x = x->parente;
            } else {
                if (w->sinistra->colore == 0) {
                    w->destra->colore = 0;
                    w->colore = 1;
                    ruota_sinistrs(albero, w);
                    w = x->parente->sinistra;}

                w->colore = x->parente->colore;
                x->parente->colore = 0;
                w->sinistra->colore = 0;
                ruota_destra(albero, x->parente);
                x = albero->radice;}}}
    x->colore = 0;}
//minimo albero
nodo_albero* minimo2(nodo_albero* nodo) {
    while (nodo->sinistra != NULL) {
        nodo = nodo->sinistra; }
    return nodo;}

void elimina_nodo_albero(albero_rosso_nero_per_ordine* albero, nodo_albero* z) {
    nodo_albero* x, *y;

    y = z;
    int colore_originale = y->colore;

    if (z->sinistra == albero->nil ) {
        x = z->destra;
        rbTransplant2(albero, z, z->destra);
    } else if (z->destra == albero->nil ) {
        x = z->sinistra;
        rbTransplant2(albero, z, z->sinistra);
    } else {
        y = minimo2(z->destra);
        colore_originale = y->colore;
        x = y->destra;
        if (y != z->destra) {
            rbTransplant2(albero, y, y->destra);
            y->destra = z->destra;
            y->destra->parente = y;}
        else {x->parente = y;}

        rbTransplant2(albero, z, y);
        y->sinistra = z->sinistra;
        y->sinistra->parente = y;
        y->colore = z->colore;}

    if (colore_originale == 0) {
        ellimina_aggiusta(albero, x);
    }
    free(z);}

void elimina_albero_rosso_nero(nodo_albero* nodo, albero_rosso_nero_per_ordine* albero) {
    if (nodo != albero->nil ) {
        elimina_albero_rosso_nero(nodo->sinistra, albero);
        elimina_albero_rosso_nero(nodo->destra, albero);
        free(nodo);}}

lista_ordini_in_sospeso* aggiungi_ordine(nodo_ricetta* nodo_ricetta, magazzino_hash* magazzino, albero_rosso_nero_per_ordine* albero, lista_ordini_in_sospeso* lista_ordini_in_sospeso, int quantita, int tempo_attuale, Corriere Corriere) {
    // Controllo e preparo l'ordine se può essere preparato con gli Ingredientei disponibili nel magazz
    if (ordine_preparabile(nodo_ricetta, quantita, tempo_attuale)) {
        
        inserisci_albero(albero, nodo_ricetta->peso_ricetta, tempo_attuale, nodo_ricetta->nome, quantita);// Inserisci l'ordine nell'albero rosso-nero degli ordini in ordine di data
    } else {
        // Se non è possibile soddisfare l'ordine ----> inserisco nella lista degli ordini in sospeso
        ordine* ordine_da_aggiungere = (ordine*)malloc(sizeof(ordine));
        ordine_da_aggiungere->nodo_ricetta = nodo_ricetta;
        ordine_da_aggiungere->quantita = quantita;
        ordine_da_aggiungere->tempo_arrivo = tempo_attuale;
        ordine_da_aggiungere->prossimo = NULL;

        if (lista_ordini_in_sospeso->testa == NULL) {// se lista vuota
            lista_ordini_in_sospeso->testa = ordine_da_aggiungere;
            lista_ordini_in_sospeso->coda = ordine_da_aggiungere;
        } else {// se non vuota lo inserisco in fondo, dato che la data è sicuramente la meno recente
            lista_ordini_in_sospeso->coda->prossimo = ordine_da_aggiungere;
            lista_ordini_in_sospeso->coda = ordine_da_aggiungere;}}

    return lista_ordini_in_sospeso;}

nodo_albero* data_minima(nodo_albero* nodo) {
    while (nodo->sinistra->sinistra != NULL) { 
        nodo = nodo->sinistra;
    }
    return nodo;}
// creo la lista in ordine di peso dell'ordine preparato in ordine decrescente per quando arriva il corriere 
nodo_ordine* inserisci_lista_peso(nodo_ordine* testa, int peso, int tempo_arrivo,    char* nome_ordine, int quantita) {
    nodo_ordine* nodo_nuovo = (nodo_ordine*)malloc(sizeof(nodo_ordine));
    nodo_nuovo->peso = peso;
    nodo_nuovo->tempo_arrivo = tempo_arrivo;
    strcpy(nodo_nuovo->nome_ordine, nome_ordine);
    nodo_nuovo->quantita = quantita;
    nodo_nuovo->prossimo = NULL;

    if (testa == NULL || (peso * quantita) > (testa->peso * testa->quantita)) {// se vuota o peso più grande del primo elemento
        nodo_nuovo->prossimo = testa;
        return nodo_nuovo;}
//trovo la posizione  in cui devo inserirlo 
    nodo_ordine* nodo_attuale = testa;
    while (nodo_attuale->prossimo != NULL && (peso * quantita) <= (nodo_attuale->prossimo->peso * nodo_attuale->prossimo->quantita)) {
        nodo_attuale = nodo_attuale->prossimo;}

    nodo_nuovo->prossimo = nodo_attuale->prossimo;
    nodo_attuale->prossimo = nodo_nuovo;

    return testa;}

// trovo la data più piccola  ---> aggiungo nella lista  per peso questo elemento---> cancello questo ordine dall'albero della data  -->  e riparti 
    // finisco in due casi: 1)  finiscono gli elementi dell'albero data  oppure 2) finisce spazio camioncino
    //stampo lista peso(che sarebbero gli ordini che ho sul camioncino ) e cancello la lista  peso 
void arrivo_corriere(albero_rosso_nero_per_ordine* albero, Corriere Corriere) {
    int capacita_usata  = 0;
    nodo_ordine* lista_peso  = NULL;
    nodo_albero* nodo_attuale;

    if (albero->radice != albero->nil ) {
        nodo_attuale = data_minima(albero->radice);// cerco la  data minima 
        while (nodo_attuale != NULL 
        && (capacita_usata  + (nodo_attuale->peso * nodo_attuale->quantita) <= Corriere.capacita_massima)) {
            lista_peso  = inserisci_lista_peso(lista_peso , nodo_attuale->peso, nodo_attuale->tempo_arrivo, nodo_attuale->nome_ordine, nodo_attuale->quantita);
            capacita_usata  =capacita_usata+ nodo_attuale->peso * nodo_attuale->quantita;// aumenti peso magazzinp
            elimina_nodo_albero(albero, nodo_attuale);
            if (albero->radice != albero->nil ) {//rifaccio lo stesso ragionamento da capo
                nodo_attuale = data_minima(albero->radice);
            } else {
                nodo_attuale = NULL;}}}


    if (capacita_usata  == 0) {
        printf("camioncino vuoto\n");
    } else {//stampo la lista peso  e allo stesso tempo la elimino
        nodo_ordine* elem_lista = lista_peso ;
        while (elem_lista != NULL) {
            printf("%d %s %d\n", elem_lista->tempo_arrivo, elem_lista->nome_ordine, elem_lista->quantita);
            nodo_ordine* elimina = elem_lista;
            elem_lista = elem_lista->prossimo;
            free(elimina);}}}


void controllo_ordini_in_sospeso(magazzino_hash* magazzino, albero_rosso_nero_per_ordine* ordinealbero, lista_ordini_in_sospeso* lista_ordini_in_sospeso, int tempo_attuale, Corriere Corriere) {
    ordine *prec = NULL, *nodo_attuale = lista_ordini_in_sospeso->testa;
    //scorro tutta la lista 
    while (nodo_attuale != NULL) {
        nodo_ricetta* nodo_ricetta = nodo_attuale->nodo_ricetta; // Uso direttamente il nodo_ricetta
        if (ordine_preparabile(nodo_ricetta, nodo_attuale->quantita, tempo_attuale)) {
            inserisci_albero(ordinealbero, nodo_ricetta->peso_ricetta, nodo_attuale->tempo_arrivo, nodo_ricetta->nome, nodo_attuale->quantita);

            // Rimuovi l'ordine dalla lista degli ordini in sospeso
            if (prec == NULL) {
                lista_ordini_in_sospeso->testa = nodo_attuale->prossimo;
            } else {
                prec->prossimo = nodo_attuale->prossimo;}
            if (nodo_attuale == lista_ordini_in_sospeso->coda) {
                lista_ordini_in_sospeso->coda = prec; }
            ordine* cancella = nodo_attuale;
            nodo_attuale = nodo_attuale->prossimo;
            free(cancella);
        } else {
            prec = nodo_attuale;
            nodo_attuale = nodo_attuale->prossimo;// controllo tutta la lista degli ordini in sospeso
        }}}


// cerca ricetta nell'albero degli ordini  complatati 
// utilizzando una funzione ricorsiva
bool cerca_negli_ordini_completati(nodo_albero* nodo,    char* nome_ordine, albero_rosso_nero_per_ordine* albero) {
    if (nodo == albero->nil ) {
        return false; // Ricetta non trovata perchè l'albero è vuoto
    }
    int controllo = strcmp(nome_ordine, nodo->nome_ordine);
    if (controllo == 0) {
        return true; // Ricetta trovata
    }
    return cerca_negli_ordini_completati(nodo->sinistra, nome_ordine, albero) || cerca_negli_ordini_completati(nodo->destra, nome_ordine, albero);}

//controllo quando devo rimuovere una ricetta se ci sono ordini di quella ricetta
bool posso_eliminare_ricetta(lista_ordini_in_sospeso* lista_ordini_in_sospeso, albero_rosso_nero_per_ordine* albero, nodo_ricetta* nodo_ricetta) {
    ordine* elem_lista = lista_ordini_in_sospeso->testa;
    
    // Verifico se ci sono ordini in sospeso nella lista degli ordini in sospeso
    while (elem_lista != NULL) {
        if (elem_lista->nodo_ricetta == nodo_ricetta) {
            return true; // Ci sono ordini in sospeso
        }
        elem_lista = elem_lista->prossimo;}

    // Verifico se la ricetta esiste nell'albero rosso-nero degli ordini completati
    if (cerca_negli_ordini_completati(albero->radice, nodo_ricetta->nome, albero)) {
        return true; // La ricetta è presente nell'albero
        }

    return false; // Nessun ordine in sospeso o completato per questa ricetta... posso eliminarla
}



void leggi_terminale(Tabella_hash* catalogo_ricette, magazzino_hash* magazzino, lista_ordini_in_sospeso* lista_ordini_in_sospeso, Corriere Corriere) {
    char commando[20];
    int tempo_attuale = 0;
    albero_rosso_nero_per_ordine albero;
    inizializza_albero(&albero);
    int conta_rifornimenti=0;//ogni 20 rifornimenti elimino scaduti


    while (fscanf(stdin, "%s", commando) != EOF || strlen(commando) == 0) {
//leggo commando "aggiungi_ricetta"
        if (strcmp(commando, "aggiungi_ricetta") == 0) {
            char nome_ordine[20];
            char Ingredientenome[20];
            int quantita;
            char c;

            if(fscanf(stdin, "%s", nome_ordine))
            ;

            nodo_ricetta* esiste = cerca_ricetta(catalogo_ricette, nome_ordine); // Controllo se la ricetta esiste già
            

            if (esiste != NULL) {
                printf("ignorato\n");
                while ((c = getchar()) != '\n' && c != EOF) {
                    if(fscanf(stdin, "%s %d", Ingredientenome, &quantita))
                    ; // Finisco  di leggere tutta la riga
                }
            } else {
                nodo_ricetta* nodo_nuovo;
                inserisci_ricetta(catalogo_ricette, nome_ordine,&nodo_nuovo); 
                nodo_nuovo->peso_ricetta = 0;
                while ((c = getchar()) != '\n' && c != EOF) {//leggo gli ingredienti della  ricetta 
                    if( fscanf(stdin, "%s %d", Ingredientenome, &quantita))
                    ;
                    nodo_nuovo->peso_ricetta=nodo_nuovo->peso_ricetta+quantita;
                    ingrediente_nel_magazzino* ingrediente_nel_magazzino = cerca_elemento_magazzino(magazzino, Ingredientenome);
                    if (ingrediente_nel_magazzino == NULL) {
                        // Creare un nuovo nodo dell'inventario
                        rifornimento_ingrediente(magazzino, Ingredientenome, 0, 0); // Aggiungi l'ingrediente 0 e 0 
                        ingrediente_nel_magazzino = cerca_elemento_magazzino(magazzino, Ingredientenome); // Ricerco di nuovo il nodo
                    }
                    aggiungi_ingrediente(nodo_nuovo, ingrediente_nel_magazzino, quantita);}
                printf("aggiunta\n");}
//se leggo commando "ordine"
        } else if (strcmp(commando, "ordine") == 0) {
            char nome_ordine[20];
            int quantita;

            if(fscanf(stdin, "%s %d", nome_ordine, &quantita))
            ;
            nodo_ricetta* nodo_ricetta = cerca_ricetta(catalogo_ricette, nome_ordine);

            if (nodo_ricetta == NULL) {
                printf("rifiutato\n"); // Ricetta non presente
            } else {
                lista_ordini_in_sospeso = aggiungi_ordine(nodo_ricetta, magazzino, &albero, lista_ordini_in_sospeso, quantita, tempo_attuale, Corriere);
                printf("accettato\n");}
//se leggo commando "rimuovi_ricetta"
        } else if (strcmp(commando, "rimuovi_ricetta") == 0) {
            char nome_ordine[20];

            if (fscanf(stdin, "%s", nome_ordine))
            ;

            nodo_ricetta* ricetta_rimuovere = cerca_ricetta(catalogo_ricette, nome_ordine);

            if (ricetta_rimuovere == NULL) {
                printf("non presente\n");
            } else if (posso_eliminare_ricetta(lista_ordini_in_sospeso, &albero, ricetta_rimuovere)) {
                printf("ordini in sospeso\n");
            } else {// non ci sono ordini con questa ricetta, posso procedere alla eliminazione
                elimina_ricetta(catalogo_ricette, nome_ordine);
                printf("rimossa\n");}
//se leggo commando "rifornimento"
        } else if (strcmp(commando, "rifornimento") == 0) {
            char c;
            char Ingredientenome[20];
            int quantita, scadenza;
            while ((c = getchar()) != '\n' && c != EOF) {
                if (fscanf(stdin, "%s %d %d", Ingredientenome, &quantita, &scadenza))
                ;
                rifornimento_ingrediente(magazzino, Ingredientenome, quantita, scadenza);}
            conta_rifornimenti++;
            if(conta_rifornimenti==20){//ogni 20 rifornimenti elimino scaduti 
                conta_rifornimenti=0;
                rimuovi__scaduti(magazzino,tempo_attuale);}
            controllo_ordini_in_sospeso( magazzino, &albero, lista_ordini_in_sospeso, tempo_attuale, Corriere); // Controllo se ci sono ordini in sospeso che possono essere preparati dopo il rifornimento
            printf("rifornito\n");}

        tempo_attuale++;

        // Controllo se può passare il corriere 
        if (tempo_attuale % Corriere.intervallo_di_arrivo == 0) {
            arrivo_corriere(&albero, Corriere);}}
   //finito di leggere il file, guardo se deve passare il corriere
    if ((tempo_attuale + 1) % Corriere.intervallo_di_arrivo == 0) {
        arrivo_corriere(&albero, Corriere);}

    elimina_albero_rosso_nero(albero.radice, &albero);
    free(albero.nil );}

int main() {
    Tabella_hash* catalogo_ricette = crea_tabella_hash();//creo tabella hash per ricette
    Corriere Corriere;
    magazzino_hash* magazzino=crea_magazzino();// creo tabella hash per magazzino

    lista_ordini_in_sospeso* lista_ordini_in_sospeso1 = (lista_ordini_in_sospeso*)malloc(sizeof(lista_ordini_in_sospeso));
    lista_ordini_in_sospeso1->testa = NULL;
    lista_ordini_in_sospeso1->coda = NULL;

    if(fscanf(stdin, "%d %d", &Corriere.intervallo_di_arrivo, &Corriere.capacita_massima))         //leggo valori corriere
    ;           
    leggi_terminale(catalogo_ricette, magazzino, lista_ordini_in_sospeso1, Corriere);

    return 0;} 