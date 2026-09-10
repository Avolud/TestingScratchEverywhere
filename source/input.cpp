#include <gccore.h>

// Struktura przechowywująca stan sterowania dla pojedynczego gracza
struct PlayerController {
    bool is_connected;
    bool button_a;      // Skok / Akcja / Spacja
    bool button_b;      // Atak / Anuluj
    bool button_start;  // Pause / Enter
    s8 stick_x;         // Gałka analogowa poziom (-128 do 127)
    s8 stick_y;         // Gałka analogowa pion (-128 do 127)
};

// Tablica ze stanem dla 4 graczy (Porty 1, 2, 3, 4)
PlayerController g_players[4];

void Input_Init() {
    // Inicjalizacja 4 portów padów GameCube
    PAD_Init();

    // Domyślne wyzerowanie stanów graczy
    for (int i = 0; i < 4; i++) {
        g_players[i].is_connected = false;
        g_players[i].button_a = false;
        g_players[i].button_b = false;
        g_players[i].button_start = false;
        g_players[i].stick_x = 0;
        g_players[i].stick_y = 0;
    }

    // Wiimote (WPAD) inicjalizujemy WYŁĄCZNIE gdy kompilujemy pod Wii
#ifdef HW_RVL
    WPAD_Init();
#endif
}

void Input_Update() {
    // Odświeżenie stanu szyny padów GameCube
    PAD_ScanPads();

    for (int i = 0; i < 4; i++) {
        // Sprawdzenie błędów połączenia (0 = Pad podłączony i gotowy)
        s32 result = PAD_ScanPads(); 
        u16 buttons_down = PAD_ButtonsDown(i);
        u16 buttons_held = PAD_ButtonsHeld(i);

        // Jeśli pad w danym porcie odpowiada
        if (PAD_ERR_NONE == 0) { 
            g_players[i].is_connected = true;

            // Przycisk wciśnięty w tej klatce
            g_players[i].button_a = (buttons_down & PAD_BUTTON_A);
            g_players[i].button_b = (buttons_down & PAD_BUTTON_B);
            g_players[i].button_start = (buttons_down & PAD_BUTTON_START);

            // Pobranie pozycji analogu (z uwzględnieniem martwej strefy > 15)
            s8 raw_x = PAD_StickX(i);
            s8 raw_y = PAD_StickY(i);

            g_players[i].stick_x = (raw_x > 15 || raw_x < -15) ? raw_x : 0;
            g_players[i].stick_y = (raw_y > 15 || raw_y < -15) ? raw_y : 0;
        } else {
            g_players[i].is_connected = false;
        }
    }
}