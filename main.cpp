#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

#define NUM0 0x30
#define NUM1 0x31
#define NUM2 0x32
#define NUM3 0x33
#define NUM4 0x34
#define NUM5 0x35
#define NUM6 0x36
#define NUM7 0x37
#define NUM8 0x38
#define NUM9 0x39
#define F3 0x72

bool Wait(const unsigned long &Time)
{
    auto Tick = clock_t(float(clock()) / float(CLOCKS_PER_SEC) * 1000.f);
    if(Tick < 0)
        return false;
    auto Now = clock_t(float(clock()) / float(CLOCKS_PER_SEC) * 1000.f);
    if(Now < 0)
        return false;
    while( (Now - Tick) < Time )
    {
        Now = clock_t(float(clock()) / float(CLOCKS_PER_SEC) * 1000.f);
        if(Now < 0)
            return false;
    }
    return true;
}

int getKeyPress() {
    int numKeys[10] = {NUM0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9};
    for (int i = 0; i < 10; i++) {
        if (GetAsyncKeyState(numKeys[i]) & 0x8000) {
            while (GetAsyncKeyState(numKeys[i]) & 0x8000) {
                Wait(1);
            }
            return i;
        }
    }
    return -1;
}

bool getCardNum(int cardNum[]) {
    int counter = 0;
    for (int i = 0; i < 500; i++) {
        int key = getKeyPress();
        if (key != -1) {
            cardNum[counter] = key;
            counter ++;
        }
        else {
            Wait(1);
        }
        if (counter >= 10) {
            return true;
        }
    }
    if (counter != 0) {
        cardNum[0] = -1;
    }
    return false;
};

bool checkEligibility(const int cardNum[]) {
    int eligibleNums[] = {2, 0, 0, 0, 0};
    for (int i = 0; i < 5; i++) {
        if (cardNum[i] != eligibleNums[i]) {
            return false;
        }
    }
    return true;
}

void resetCardNum(int cardNum[]) {
    for (int i = 0; i < 10; i++) {
        cardNum[i] = 0;
    }
    cardNum[0] = -1;
}

void pressKey(int key) {
    INPUT ip;
    ip.type = INPUT_KEYBOARD;
    ip.ki.wScan = 0;
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;
    ip.ki.wVk = key;
    ip.ki.dwFlags = 0;
    SendInput(1, &ip, sizeof(INPUT));
    Wait(500);
    ip.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip, sizeof(INPUT));
}

int main() {
    int cardNum[10] = {-1, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    string nijiiroPath;
    ifstream nijiiroPathFile("nijiiro_path.txt");
    string cardsDatFileName = "cards.dat";
    if (nijiiroPathFile.good()) {
        getline(nijiiroPathFile, nijiiroPath);
        nijiiroPathFile.close();
    }
    else {
        cout << "nijiiro_path.txt not found, quit in 5 seconds" << endl;
        Wait(5000);
        return 0;
    }

    cout << "Program started" << endl;

    fs::path cardsDatFilePath = nijiiroPath;
    cardsDatFilePath /= cardsDatFileName;

    if (!fs::exists(cardsDatFilePath)) {
        cout << "cards.dat not found, quit in 5 seconds" << endl;
        Wait(5000);
        return 0;
    }

    while (true) {
        if (getCardNum(cardNum) and checkEligibility(cardNum)) {
            std::cout << "Card number detected: ";

            std::ostringstream os;
            for (int i : cardNum) {
                os << i;
            }
            std::string cardNumString(os.str());
            std::cout << cardNumString << std::endl;

            if (fs::exists(cardsDatFilePath)) {
                ofstream cardsDatFile(cardsDatFilePath);
                cardsDatFile << cardNumString;
                cardsDatFile.close();
                pressKey(F3);
            }
            else {
                std::cout << "cards.dat not found" << std::endl;
            }

            resetCardNum(cardNum);
            Wait(500);
        }
        else {
            if (cardNum[0] == -1) {
                resetCardNum(cardNum);
            }
            else {
                std::cout << "Invalid card number: ";
                for (int i : cardNum) {
                    std::cout << i;
                }
                std::cout << std::endl;
                resetCardNum(cardNum);
                Wait(500);
            }
        }
    }
}
