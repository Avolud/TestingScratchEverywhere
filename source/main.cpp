#include <gccore.h>

void init_gamecube_controllers() {
    // Inicjalizacja fizycznych portów padów GameCube
    PAD_Init();

#ifdef HW_RVL
    // Kod Wiimote/Nunchuk uruchomi się TYLKO na Nintendo Wii
    WPAD_Init();
#endif
}

void update_gamecube_input() {
    // Skanowanie przycisków na GameCube
    PAD_ScanPads();
    
    // Odczyt z pierwszego kontrolera (Port 1)
    u16 pressed = PAD_ButtonsDown(0);
    s8 stick_x = PAD_StickX(0);
    s8 stick_y = PAD_StickY(0);

    // Mapowanie na akcje ze Scratcha:
    if (pressed & PAD_BUTTON_A) {
        // Przycisk A -> Spacja / Skok / Kliknięcie
    }
    if (pressed & PAD_BUTTON_B) {
        // Przycisk B -> Atak / Anuluj
    }
    if (pressed & PAD_BUTTON_START) {
        // START -> Enter / Pauza
    }

    // Ruch analogiem z uwzględnieniem martwej strefy (Deadzone)
    if (stick_x > 18)  { /* Strzałka w prawo */ }
    if (stick_x < -18) { /* Strzałka w lewo */ }
    if (stick_y > 18)  { /* Strzałka w górę */ }
    if (stick_y < -18) { /* Strzałka w dół */ }
}