//////////////////////////////////////////////////////////////////////////////
/// @file main.cpp
/// @author Kai R. 
/// @brief Debounce RPicoButton Beispiel für angeschlossene Taster
///        Die Abfrage erfolgt mit Hilfe der RPicoButton Klasse
///        Die eigentliche Abfrage läuft im Core1, Das Hauptprogramm idlet herum. Mit Hilfe eines Timers
///        wird die FiFo Queue regelmäßig abgefragt.
/// 
/// @date 2021-11-15
/// @version 0.1
/// 
/// @copyright Copyright (c) 2021
/// 
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include "pico/util/queue.h"
#include "pico/multicore.h"

#include <iostream>
#include <iomanip>
#include <bitset>

#include "rpicobutton.hpp"

#define BASEPIN  12
#define PINCOUNT  4

typedef struct                  // Struktur für den Datenaustausch zwischen den Cores.
{
   uint16_t state   {0};        // RPicoButton Bitmaske nachdem der/die Taster losgelassen wurden
   uint32_t time_ms {0};        // Drückdauer des/der Taster (ms)
} buttonData;

queue_t rpbQueue;               // Queue für den Datenaustausch zwischen den Cores

//////////////////////////////////////////////////////////////////////////////
/// @brief Tasterabfrage mit Hilfe dieser Funktion auf Core1 ausführen
/// 
//////////////////////////////////////////////////////////////////////////////

void checkButtonC1() {
  buttonData qDataC1;                               // Datenstruktur für Queue für Core1 
 
  RPicoButton checkButtons(true, true, 100);        // Objekt erzeugen
  checkButtons.init(BASEPIN, PINCOUNT);             // Objekt initialisieren

  while(true) {
    checkButtons.tic();                             // Abfrage der Input Pins
    qDataC1.state =  checkButtons.pressed();
    if (qDataC1.state) {                            // Sende nur wenn state > 0 = Mindestens ein Taster wurde gedrückt
      qDataC1.time_ms = checkButtons.pressing_time();

      std::cout << std::endl << "Core1" << " Tastermuster: " << std::bitset<PINCOUNT>(qDataC1.state).to_string('-','X') 
      << " Dezimal: " << std::setw(2) << qDataC1.state << " Drückzeit: " << std::setw(4) << qDataC1.time_ms << " ms"<< std::endl;

      queue_add_blocking(&rpbQueue, &qDataC1);      // Sende Daten zu Core0
    }
  }
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Callbackfunktion zum Abfragen der FIFO Queue zur Übernahme der
///        Daten von Core1. Ausgabe der Daten auf der Konsole
/// 
/// @param t 
/// @return true 
/// @return false 
//////////////////////////////////////////////////////////////////////////////
bool rpt_callback(struct repeating_timer *t) {
  bool qFlag {false};
  buttonData *qData  {static_cast<buttonData *>(t->user_data)};  // Caste user_data Pointer auf Datentyp buttonData

  qFlag = queue_try_remove(&rpbQueue, qData);                    // Hole Daten von Core1
  if (qFlag) {
    std::cout << "Core0" << " Tastermuster: " << std::bitset<PINCOUNT>(qData->state).to_string('-','X') << " Dezimal: " << std::setw(2)
    << qData->state << " Drückzeit: " << std::setw(4) << qData->time_ms << " ms"<< std::endl;
  } 
  return true;
}

//////////////////////////////////////////////////////////////////////////////
/// @brief Hauptprogramm
///        Ermitteln ob ein oder mehrere Taster gedrückt wurden oder nicht. 
///        Das kann für bis zu 16 Tastern gleichzeitig geschehen.  
///        Anschlussart der Taster (Pull Up oder Pull Down) und eine Debounce-Zeit 
///        ist einstellbar.
///
/// @return int 
//////////////////////////////////////////////////////////////////////////////

int main() {
  buttonData qDataC0;                                    // Datenstruktur für Queue definieren / Global für Core0
  bool qFlag {false};
  struct repeating_timer timer;                          // Timerstruktur für Repeatingtimer 

  stdio_init_all(); 
  sleep_ms(10000);                                       // Warten damit Zeit ist, die serielle Konsole zu öffnen
 /* 
  * add_repeating_timer_ms
  * Alle 250ms schauen ob an der FIFO-QUEUE Daten von Core1 anliegen
  * Pointer von qDataC0 wird an user_data übergeben
  */
  add_repeating_timer_ms(250,rpt_callback,&qDataC0, &timer); 
  
  queue_init(&rpbQueue, sizeof(buttonData), 1);          // Initialisiere Queue für ein Datenelement mit der Größe von buttonData
  multicore_launch_core1(checkButtonC1);                 // Starte Tasturabfrage auf Core1
                                                       
  std::cout << "Gestartet. Warte auf Input..." << std::endl;
  while(true) {                                          // Endlosscheife nix machen außer jede Sekunde einen Punkt ausgeben.
    std::cout << ".";
    fflush(stdout);
    sleep_ms(1000);
  }
  return 0;     
}

