//
//  stub.c
//  Crypter
//
//  Created by Derek Ta on 11/15/14.
//
//

#include "stub.h"
struct StubData{
    unsigned char * pFileBuffer;
    unsigned long FileSize;
    unsigned char *pKey;
    unsigned int KeySize;
    char FileName[MAX_PATH];
};

void LoadStruct(struct StubData *sData)
{
    HRSRC hResource; // Hanndle to the resource in mem
    HGLOBAL hGlobal; //Handle to loaded resource
    
    //Get data for the file/filesize
    hResource = FindResource(NULL, MAKEINTRESOURCE(10), RT_RCDATA); // RT_RCDATA = Read/copy
    if (!hResource)
        exit(EXIT_FAILURE);
    sData->pFileBuffer = sizeOfResource(NULL, hResource);
    
    //Get data for the key/keysize
    hResource = FindResource(NULL, MAKEINTRESOURCE(20), RT_RCDATA);
    if(!hResource)
        exit(EXIT_FAILURE);
    sData->KeySize = sizeOfResource(NULL, hResource);
    // Load into memory
    hGlobal = LoadResource(NULL, hResource);
    if (!hGlobal)
        exit(EXIT_FAILURE);
    // Assign starting point of memory to the buffer
    sData->pKey = (unsigned char*)LockResource(hGlobal);
    if (!sData->pKey)
        exit(EXIT_FAILURE);
    GetModuleFileName(NULL, sData->FileName,MAX_PATH);
    
}

int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    
}