//////////////////////////////////////////////////////////////////////////////
/// @file main.cpp
/// @author Kai R. 
/// @brief PIO Debounce RPicoButton Beispiel für angeschlossene Taster
///        Die Abfrage erfolgt mit Hilfe der RPicoButton Klasse
/// 
/// @date 2021-11-15
///       Initiale Version
///
/// @date 2021-12-06
///       Beispiel um eine Timerfunktion ergänzt. Es können jetzt zwei 
///       Varianten des Programms compiliert werden. Je nachdem, ob
///       "WITH_TIMER" definiert wurde oder nicht.
///
/// @version 0.2
/// 
/// @copyright Copyright (c) 2021
/// 
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include <bitset>
#include "rpicobutton.hpp"

#define BASEPIN  12
#define PINCOUNT  4

#define WITH_TIMER  // Auskommentieren wenn Programm ohne Timer laufen soll.

#ifdef WITH_TIMER
bool rpt_callback(struct repeating_timer *t);
#endif
void print_state(uint32_t bstate, uint32_t pTime, std::initializer_list<uint32_t> bNum);

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
#ifdef WITH_TIMER
  struct repeating_timer timer;                          // Timerstruktur für Repeatingtimer 
#endif
  uint32_t pTime;

  stdio_init_all(); 
  sleep_ms(10000); // Warten damit Zeit ist, die serielle Konsole zu öffnen
  
  // Button Objekt erzeugen und initialisieren
  RPicoButton checkButtons(true, true, 100);  
  checkButtons.init(BASEPIN, PINCOUNT);

#ifdef WITH_TIMER
  /* 
  * add_repeating_timer_ms
  * Alle 1 ms schauen ob ein Taster gedrückt wurde.
  */
  add_repeating_timer_ms(1,rpt_callback,&checkButtons, &timer); 
#endif

  printf("Gestartet. Warte auf Input...\n");
  while(true) {
#ifndef WITH_TIMER
    checkButtons.tic();                            // Abfrage der Input Pins
#else    
    sleep_ms(1000);
    printf(".");
    fflush(stdout);
#endif    
    uint32_t state {checkButtons.pressed()};       // Checken ob etwas gedrückt und wieder losgelassen wurde
    if (state) {
      pTime = checkButtons.pressing_time();
    }
    switch (state) {                               // Auswerten. 0 (false) wenn nichts gedrückt
    case 1:
      print_state(state, pTime, {1});
      break;
    case 2:
      print_state(state, pTime, {2});
      break;
    case 3:
      print_state(state, pTime, {1, 2});
      break;
    case 4:
      print_state(state, pTime, {3});
      break;
    case 5:
      print_state(state, pTime, {1, 3});
      break;
    case 6:
      print_state(state, pTime, {2, 3});
      break;
    case 7:
      print_state(state, pTime, {1, 2, 3});
      break;
    case 8:
      print_state(state, pTime, {4});
      break;
    case 9:
      print_state(state, pTime, {1, 4});
      break;
    case 10:
      print_state(state, pTime, {2, 4});
      break;
    case 11:
      print_state(state, pTime, {1, 2, 4});
      break;
    case 12:
      print_state(state, pTime, {3, 4});
      break;
    case 13:
      print_state(state, pTime, {1, 3, 4});
      break;
    case 14:
      print_state(state, pTime, {2, 3, 4});
      break;
    case 15:
      print_state(state, pTime, {1, 2, 3, 4});
      break;
    default:
      break;
    }
  }
  return 0;     
}

#ifdef WITH_TIMER
//////////////////////////////////////////////////////////////////////////////
/// @brief Callbackfunktion zum Abfragen der Taster
/// 
/// @param t 
/// @return true 
/// @return false 
//////////////////////////////////////////////////////////////////////////////
bool rpt_callback(struct repeating_timer *t) {
  bool qFlag {false};
  RPicoButton *rpbClass  {static_cast<RPicoButton *>(t->user_data)};  // Caste user_data Pointer auf Datentyp buttonData
  rpbClass->tic();
  return true;
}
#endif

//////////////////////////////////////////////////////////////////////////////
/// @brief Ausgabe welche Tasten wie lange gedrückt wurden
/// 
/// @param bstate 
/// @param bptime 
/// @param bNum 
//////////////////////////////////////////////////////////////////////////////
void print_state(uint32_t bstate, uint32_t bptime, std::initializer_list<uint32_t> bNum) {
  const auto *tPtr = bNum.begin();             // Adresse des ersten Elementes aus der Übergabeliste
  char text[4][9] = {", " ," und ", " wurde "," wurden "};
  
  std::cout << "Taster Muster: " << std::bitset<PINCOUNT>(bstate);
  std::cout << " -> Taster " << *tPtr;
  switch (bNum.size())
  {
  case 1:
    std::cout << text[2]; 
    break;
  case 2:
    std::cout << text[1] << *(tPtr+1) << text[3];
    break;
  case 3:
    std::cout << text[0] << *(tPtr+1) << text[1] << *(tPtr+2) << text[3];
    break;
  case 4:
    std::cout << text[0] << *(tPtr+1) << text[0] << *(tPtr+2) << text[1] << *(tPtr+3) << text[3];
    break;
  default:
    break;
  }    
  std::cout << "für " << bptime << " ms gedrückt." << std::endl;
}
