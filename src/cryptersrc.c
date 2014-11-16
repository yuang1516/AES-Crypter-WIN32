// Allocating the file to encrypt

// Data sent to the stub
struct StubData
{
	unsigned char * pFileBuffer;
	unsigned long FileSize; 
	unsigned char * pKey;
	unsigned long KeySize;
}
// LoadFile (to encrypt) into Memory -> Encrypt that buffer -> Pass buffer to StubData


// Retrieves pointer to file, pointer to area we are going to work
// with file, opens file 
// and allocates space on the heap. Maps the original file into 
// space on the heap and sData.pFileBuffer points to beginning
// of this location
void LoadFile(char *File, struct StubData *sData)
{
	// Load file into memory, file hasn't been loaded yet, StubData has
	unsigned long BytesRead;
	HANDLE hFile = CreateFile(File, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	printf("[*]Loading Portable Executable\n")

	if (hFile == INVALID_HANDLE_VALUE)
		error("Error - Cannot open file");
	sData ->FileSize = GetFileSize(hFile, NULL);
	if (sData->FileSize = INVALID_FILE_SIZE){
		CloseHandle(hFile);
		error("Error - Cannot retrieve file size")
	}

	// The stub will get a pointer to the end of the file's memory space
	sData -> pFileBuffer = (unsigned char *)malloc(sData->FileSize);
	if (!sData->pFileBuffer){
		CloseHandle(hFile);
		error("Error - Cannot allocate buffer memory");
	}
	ReadFile(hFile, sData ->pFileBuffer, sData->FileSize, &BytesRead, NULL);
	CloseHandle(hFile);
}


void AESEncrypt(struct StubData * sData)
{
	//Replace with AES Encryption later 
	int i, j;
	sData->pKey = "setpassword"
	sData->KeySize = strlen(sData->)
	for (i; i<sData->FileSize; i++)
	{
		*(sData ->pFileBuffer+i) ^= *(sData->pKey+j);
		j++;

		if(j>=sData->KeySize)
			j=0;
	}
}

void BuildEncryptedVers (struct StubData * sData)
{
	HRSRC hResource; 
	HGLOBAL hGlobal;
	HANDLE hFile, hUpdate;

	unsigned long rSize, BytesWritten;
	unsigned char * pBuffer;

	printf("[*]Building Crypted.exe\n");

	// Resource
	hResource = FindResource(NULL, MAKEINTRESOURCE(1), "STUB");
	if (!hResource)
		error("Error: could not find resource");
	rSize = SizeofResource(NULL, hResource);

	// Global
	hGlobal = LoadResource(NULL, hResource);
	if (!hGlobal)
		error("Error: Could not load resource");
	hFile = CreateFile("crypted.exe", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	
	// File Operations
	if (hFile == INVALID_HANDLE_VALUE)
	{
		free(pBuffer);
		free(sData->pFileBuffer);
		error("Error - Could not create file");
	}
	if (WriteFile(hFile, pBuffer, rSize, &BytesWritten, NULL)==0)
	{
		free(pBuffer);
		free(sData->pFileBuffer);
		error("Error - Could not write to file");
	}

    // Discard the original file
	CloseHandle(hFile);
	free(pBuffer);

    // Resources -> Where the data is stored
	hUpdate = BeginUpdateResource("crypted.exe", FALSE); // Updating the encrypted file's contents
	if(UpdateResource(hUpdate, RT_RCDATA, MAKEINTRESOURCE(10), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), sData->pFileBuffer, sData->FileSize) == 0){
		error("Error - Could not update resource");
	}

    if (UpdateResource(hUpdate, RT_RCDATA, MAKEINTRESOURCE(20), MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), sData->pKey, sData->KeySize)==0) // Updating the file's key -> key same for both encryption and decryption
    {
  		error("Error - Could not update resource");
    }
    EndUpdateResource(hUpdate, FALSE);
}




