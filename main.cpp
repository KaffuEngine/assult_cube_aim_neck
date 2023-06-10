#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <psapi.h>
#include <tchar.h>
#include <cmath>
#define M_PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679 // define M_PI manually

using namespace std;

//00620B48 entity table base

struct Coords {
    float x, z, y;
};



int main()
{
    HWND TheGame = FindWindowA("SDL_app", 0);
    DWORD GameProcessId_PID = 0;
    GetWindowThreadProcessId(TheGame, &GameProcessId_PID);
    HANDLE GameHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, GameProcessId_PID);

    HMODULE modules[1024];

    DWORD cbNeeded;
    (EnumProcessModules(GameHandle, modules, sizeof(modules), &cbNeeded));

    for (int i = 0; i <= (cbNeeded / sizeof(HMODULE)); i++)
    { 
        TCHAR szModName[MAX_PATH];
        (GetModuleFileNameEx(GameHandle, modules[i], szModName, sizeof(szModName) / sizeof(szModName[MAX_PATH])));
        (_tcsicmp(_tcsrchr(szModName, '\\') + 1, TEXT("ac_client.exe")) == 0);
        uintptr_t enemy_entity = (uintptr_t)modules[i] + 0x187C10;
        uintptr_t player_entity = (uintptr_t)modules[i] + 0x17B0B8;


        while (true) {
            while (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
                int player_enemy_Address;
                ReadProcessMemory(GameHandle, (LPCVOID)enemy_entity, &player_enemy_Address, sizeof(DWORD), 0);
                int player_enemy;
                ReadProcessMemory(GameHandle, (LPCVOID)(player_enemy_Address + 0x0), &player_enemy, sizeof(int), 0);//getting player enemy
                int player_enemy_health;
                ReadProcessMemory(GameHandle, (LPCVOID)(player_enemy + 0xEC), &player_enemy_health, sizeof(int), 0);//getting enemy playe health
                Coords player_enemy_eyes_pos;
                ReadProcessMemory(GameHandle, (LPCVOID)(player_enemy + 0x4), &player_enemy_eyes_pos, sizeof(Coords), 0);//getting enemy playe coords
                Coords player_enemy_pos;
                ReadProcessMemory(GameHandle, (LPCVOID)(player_enemy + 0x28), &player_enemy_pos, sizeof(Coords), 0);//getting enemy playe coords
                //--------
                int playerAddress;
                ReadProcessMemory(GameHandle, (LPCVOID)player_entity, &playerAddress, sizeof(DWORD), 0);
                Coords player_eyes_pos;
                ReadProcessMemory(GameHandle, (LPCVOID)(playerAddress + 0x4), &player_eyes_pos, sizeof(Coords), 0);//getting playe coords
                Coords player_pos;
                ReadProcessMemory(GameHandle, (LPCVOID)(playerAddress + 0x28), &player_pos, sizeof(Coords), 0);//getting playe coords
                float player_YView;
                ReadProcessMemory(GameHandle, (LPCVOID)(playerAddress + 0x38), &player_YView, sizeof(float), 0);//getting playe YView
                float player_XView;
                ReadProcessMemory(GameHandle, (LPCVOID)(playerAddress + 0x34), &player_XView, sizeof(float), 0);//getting playe XView



                // calculate distance and height difference
                float distance = sqrt(pow(player_pos.x - player_enemy_pos.x, 2) + pow(player_pos.z - player_enemy_pos.z, 2));
                float enemy_eye = player_enemy_eyes_pos.y; // y-coordinate of enemy's eye between the ground
                float my_eye = player_eyes_pos.y; // y-coordinate of your eye between the ground
                float height_diff = abs(my_eye - enemy_eye); // difference in height between your eye and enemy's eye
                float y_view_angle = atan(height_diff / distance) * 180 / M_PI;
                if (my_eye > enemy_eye) {
                    y_view_angle = -y_view_angle;
                }
                cout << "Y view angle in enemy's eye: {" << y_view_angle << "} degrees "<<endl;
                float ESP_Y = (y_view_angle);
                BOOL result_y = WriteProcessMemory(GameHandle, (void*)(playerAddress + 0x38), &ESP_Y, sizeof(float), NULL);
                if (result_y == FALSE) {
                    cout << "Error writing to process memory: " << GetLastError() << endl;
                    break;
                }

                float dx = player_enemy_pos.x - player_pos.x;
                float dz = player_enemy_pos.z - player_pos.z;
                float dist = sqrt(dx * dx + dz * dz);
                float pitch = asin((player_enemy_pos.y - player_pos.y) / dist) * 180.0 / M_PI;
                float yaw = atan2(dz, dx) * 180.0 / M_PI;
                float x_view_angle = yaw + 90.0;

                if (x_view_angle <= 0) {
                    x_view_angle += 360.0;
                }

                cout << "X view angle in enemy's eye: {" << x_view_angle << "} degrees " << endl;
                cout << "The distance bettween your enemy is: [" << distance << "] cm\n\n\n"<<endl;


                float ESP_X = (x_view_angle);
                BOOL result_x = WriteProcessMemory(GameHandle, (void*)(playerAddress + 0x34), &ESP_X, sizeof(float), NULL);
                if (result_x == FALSE) {
                    cout << "Error writing to process memory: " << GetLastError() << endl;
                    break;
                }



            }
        }



            


        




        

    }



    CloseHandle(GameHandle);
    return 0;
}
