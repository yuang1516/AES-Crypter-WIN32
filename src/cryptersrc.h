//
//  cryptersrc.h
//  Crypter
//
//  Created by Derek Ta on 11/15/14.
//
//

#ifndef Crypter_cryptersrc_h
#define Crypter_cryptersrc_h

void LoadFile(char *File, struct StubData *sData);
void AESEncrypt(struct StubData *sData);
void BuildEncryptedVers(struct StubData *sData);

#endif
