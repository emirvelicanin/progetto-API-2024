# Prova Finale di Algoritmi e Principi dell'Informatica - A.A. 2023-2024

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://github.com/emirvelicanin/progetto-API-2024/blob/main/LICENSE)

Prova finale di algoritmi e principi dell'informatica per l'anno accademico 2023-2024.

Docente: Martineghi Davide

**Valutazione**: 30 LODE/30

## Obiettivo del progetto

Realizzare un programma in C per il funzionamento di una pasticceria migliorando il sistema di ordini.

Il programma non solo deve produrre un output corretto, ma deve rispettare dei vincoli di memoria e tempo CPU come in tabella:

| Valutazione | Memoria | Tempo |       Esito        |
|-------------|--------:|------:|:------------------:|
| 18          | 35  MiB | 14 s  | :white_check_mark: |
| 21          | 30  MiB | 11 s  | :white_check_mark: |
| 24          | 25  MiB |  9 s  | :white_check_mark: |
| 27          | 20  MiB |  6 s  | :white_check_mark: |
| 30          | 15  MiB |  4 s  | :white_check_mark: |
| 30L         | 14  MiB |  1 s  | :white_check_mark: |


La specifica completa del progetto è disponibile [qui](https://github.com/emirvelicanin/progetto-API-2024/blob/main/Documents/specifica.pdf).

I test sono disponibili [qui](https://github.com/emirvelicanin/progetto-API-2024/blob/main/test_cases.zip).

## Scelte progettuali e strutture Dati Utilizzate

## 🍰 Ricette e Magazzino
Le ricette e il Magazzino  sono state implementate tramite **tabella hash**
---
## 📦 Ordini
La gestione degli ordini richiede due approcci differenti:

### Ordini non completati
Rappresentati tramite **lista concatenata**, che permette una gestione semplice delle aggiunte e rimozioni.  

### Ordini completati
Organizzati in un **albero rosso-nero**, che mantiene automaticamente i nodi ordinati in base alla data di completamento.  

Questa scelta consente di ottenere uno storico ordinato degli ordini completati, con ricerche efficienti in funzione della data.

## Strumenti utilizzati

| Descrizione         | Strumento                                         |
|---------------------|---------------------------------------------------|
| IDE                 | [VisualStudioCode](https://code.visualstudio.com/)|
| Compilatore         | [gcc](https://gcc.gnu.org/)                       |
| Misurazione memoria | [Valgrind - Massif](https://valgrind.org/)        |    
| Sistema operativo   | mac0S                                             |

## Copyright e licenza

Il progetto è distribuito sotto [License: MIT](https://github.com/emirvelicanin/progetto-API-2024/blob/main/LICENSE)], si applicano le limitazioni descritte in tale licenza.
