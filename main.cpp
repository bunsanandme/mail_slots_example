#include <windows.h>
#include <strsafe.h>
#include <iostream>
#include <thread>
#include <sstream>

#include "mailslot.h"
using namespace std;

const int N = 100;

void thread1(int &a, int &b) {
    for(int i = 0; i < N; i++) {
        a = b + 3;
        b--;
        cout << "[Thread 1: a = " << a << ", b = " << b << ']' << endl;
    }
    MakeSlot(SlotName);
    HANDLE hFile;
    hFile = CreateFile(SlotName,GENERIC_WRITE,FILE_SHARE_READ,(LPSECURITY_ATTRIBUTES) nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,(HANDLE) nullptr);
    char* s = new char[100];
    itoa(b,s,10);
    WriteSlot(hFile, s);
    delete[] s;
}

void thread2() {
    LPSTR value = ReadSlot();
    std::stringstream s_str(value);
    int b;
    s_str >> b;
    for(int i = 0; i < N; i++) {
        b += 2;
        cout << "[Thread 2: b = " << b << ']' << endl;
    }
}

int main() {
    int a = 10;
    int b = 10;
    thread thr1(thread1, ref(a), ref(b));
    thr1.join();
    thread thr2(thread2);
    thr2.join();

    return 0;
}



