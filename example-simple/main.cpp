//////////////////////////////////////////////////////////////////////////////
/// @file main.cpp
/// @author Kai R. 
/// @brief PIO Debounce RPicoButton Beispiel für angeschlossene Taster
///        Die Abfrage erfolgt mit Hilfe der RPicoButton Klasse
/// 
/// @date 2021-11-15
/// @version 0.1
/// 
/// @copyright Copyright (c) 2021
/// 
//////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <iostream>
#include <bitset>
#include "rpicobutton.hpp"

//////////////////////////////////////////////////////////////////////////////
/// @brief Hauptprogramm
///        Ermitteln ob ein oder mehrere Taster gedrückt wurden oder nicht. 
///        Das kann für bis zu 16 Tastern gleichzeitig geschehen.  
///        Anschlussart der Taster (Pull Up oder Pull Down) und eine Debounce-Zeit 
///        ist einstellbar.
///
/// @return int 
//////////////////////////////////////////////////////////////////////////////

#define BASEPIN  12
#define PINCOUNT  4

void print_state(uint32_t bstate, uint32_t pTime, std::initializer_list<uint32_t> bNum);

int main() {
  uint32_t pTime;

  stdio_init_all(); 
  sleep_ms(10000); // Warten damit Zeit ist, die serielle Konsole zu öffnen
  
  // Button Objekt erzeugen und initialisieren
  RPicoButton checkButtons(true, true, 100);  
  checkButtons.init(BASEPIN, PINCOUNT);

  printf("Gestartet. Warte auf Input...\n");
  while(true) {
    checkButtons.tic();                            // Abfrage der Input Pins
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
