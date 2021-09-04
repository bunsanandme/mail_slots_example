#ifndef MAILS_MAILSLOT_H
#define MAILS_MAILSLOT_H

#include <iostream>
using namespace std;

HANDLE hSlot;
TCHAR* SlotName = "\\\\.\\mailslot\\sample_mailslot";

BOOL WINAPI MakeSlot(LPTSTR lpszSlotName) {
    hSlot = CreateMailslot(lpszSlotName,0,MAILSLOT_WAIT_FOREVER, (LPSECURITY_ATTRIBUTES) nullptr);
    if(hSlot == INVALID_HANDLE_VALUE) {
        cout << "CreateMailSlot failed with " <<  GetLastError() << endl;
        return FALSE;
    }
    else cout << "Mailslot created successfully\n";
    return TRUE;
}

BOOL WriteSlot(HANDLE slot, LPTSTR lpszMessage) {
    BOOL fResult;
    DWORD cdWritten;

    fResult = WriteFile(slot,lpszMessage,(DWORD)(lstrlen(lpszMessage)+1)*sizeof(TCHAR),&cdWritten,(LPOVERLAPPED) nullptr);

    if(!fResult) {
        cout <<"WriteFile failed with " << GetLastError() << endl;
        return FALSE;
    }
    cout << "Slot written to successfully\n";
    return TRUE;
}

LPSTR ReadSlot() {
    DWORD cbMessage, cMessage, cbRead;
    BOOL fResult;
    LPTSTR lpszBuffer;
    TCHAR achID[80];
    DWORD cAllMessages;
    HANDLE hEvent;
    OVERLAPPED ov;

    cbMessage = cMessage = cbRead = 0;

    hEvent = ::CreateEvent( nullptr, FALSE, FALSE, TEXT("ExampleSlot"));
    if(nullptr == hEvent) return FALSE;
    ov.Offset = 0;
    ov.OffsetHigh = 0;
    ov.hEvent = hEvent;

    fResult = GetMailslotInfo( hSlot, (LPDWORD) nullptr, &cbMessage, &cMessage,(LPDWORD) nullptr);




    cAllMessages = cMessage;
    while (cMessage != 0) {
        ::StringCchPrintf((LPTSTR) achID,80,TEXT("\nMessage #%d of %d \n"), cAllMessages - cMessage +1, cAllMessages);

        lpszBuffer = (LPTSTR) GlobalAlloc(GPTR,lstrlen((LPTSTR) achID)*sizeof(TCHAR) + cbMessage);

        if(nullptr == lpszBuffer) return FALSE;
        lpszBuffer[0] = '\0';

        fResult = ::ReadFile(hSlot,lpszBuffer,cbMessage,&cbRead,&ov);

        if(!fResult) {
            cout << "ReadFile failed with " << GetLastError() << endl;
            ::GlobalFree((HGLOBAL) lpszBuffer);
            return FALSE;
        }

        ::StringCbCat(lpszBuffer, lstrlen((LPTSTR) achID)*sizeof(TCHAR) + cbMessage,(LPTSTR) achID);
        return lpszBuffer;
        cout << "Contents of the mailslot: " << lpszBuffer << endl;
        ::GlobalFree((HGLOBAL) lpszBuffer);
        fResult = GetMailslotInfo(hSlot,(LPDWORD) nullptr, &cbMessage, &cMessage,(LPDWORD) nullptr);
        if(!fResult) {
            cout << "GetMailslotInfo failed " << GetLastError() << endl;
            return FALSE;
        }
    }
    CloseHandle(hEvent);

}

#endif //MAILS_MAILSLOT_H
