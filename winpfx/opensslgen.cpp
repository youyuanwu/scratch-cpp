

#include <stdio.h>

#include <windows.h>
#include <Wincrypt.h>

#include "openssl/ssl.h"
#include "openssl/pkcs12.h"

#include <vector>
#include <string>
#include <iostream>

PCCERT_CONTEXT CreateOurCertificate()
	{
	// CertCreateSelfSignCertificate(0,&SubjectName,0,0,0,0,0,0);
	HRESULT hr = 0;
	HCRYPTPROV hProv = NULL;
	PCCERT_CONTEXT p = 0;
	HCRYPTKEY hKey = 0;
	CERT_NAME_BLOB sib = { 0 };
	BOOL AX = 0;

	// Step by step to create our own certificate
	try
		{
		// Create the subject
		char cb[1000] = {0};
		sib.pbData = (BYTE*)cb; 
		sib.cbData = 1000;
		wchar_t*	szSubject= L"CN=Certificate";
		if (!CertStrToNameW(CRYPT_ASN_ENCODING, szSubject,0,0,sib.pbData,&sib.cbData,NULL))
			throw;
	

		// Acquire Context
		wchar_t* pszKeyContainerName = L"Container";

		if (!CryptAcquireContextW(&hProv,pszKeyContainerName,MS_ENHANCED_PROV_W,PROV_RSA_FULL,CRYPT_NEWKEYSET | CRYPT_MACHINE_KEYSET))
			{
			hr = GetLastError();
			if (GetLastError() == NTE_EXISTS)
				{
				if (!CryptAcquireContextW(&hProv,pszKeyContainerName,MS_DEF_PROV_W,PROV_RSA_FULL,CRYPT_MACHINE_KEYSET))
					{
					throw;
					}
				}
			else
				throw;
			}

		// Generate KeyPair
		if (!CryptGenKey(hProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE, &hKey))
			throw;

		// Generate the certificate
		CRYPT_KEY_PROV_INFO kpi = {0};
		kpi.pwszContainerName = pszKeyContainerName;
		kpi.pwszProvName = MS_DEF_PROV_W;
		kpi.dwProvType = PROV_RSA_FULL;
		kpi.dwFlags = CERT_SET_KEY_CONTEXT_PROP_ID;
		kpi.dwKeySpec = AT_KEYEXCHANGE;

		SYSTEMTIME et;
		GetSystemTime(&et);
		et.wYear += 1;

		CERT_EXTENSIONS exts = {0};
		p = CertCreateSelfSignCertificate(hProv,&sib,0,&kpi,NULL,NULL,&et,&exts);

		AX = CryptFindCertificateKeyProvInfo(p,CRYPT_FIND_MACHINE_KEYSET_FLAG,NULL) ;
/*		hCS = CertOpenStore(CERT_STORE_PROV_MEMORY,0,0,CERT_STORE_CREATE_NEW_FLAG,0);
		AX = CertAddCertificateContextToStore(hCS,p,CERT_STORE_ADD_NEW,0);
		AX = CryptFindCertificateKeyProvInfo(p,CRYPT_FIND_MACHINE_KEYSET_FLAG,NULL);*/
		}
	
	catch(...)
		{
            printf("gen key failed");
            return 0;
		}
	
	if (hKey)
		CryptDestroyKey(hKey);
	hKey = 0;
	
	if (hProv)
		CryptReleaseContext(hProv,0);
	hProv = 0;
	return p;
}

int exportPrivatekeyBlob(HCRYPTKEY hKey, std::vector<BYTE> & out){
    std::vector<BYTE> keyBlob;
    DWORD dwBlobLen; 
    
    if (!::CryptExportKey(
        hKey,
        0,
        PRIVATEKEYBLOB,
        0,
        NULL,
        &dwBlobLen))
    {
        printf("CryptExportKey 1 failed \n");
        return 1;
    }
    keyBlob.resize(dwBlobLen);
    if (!CryptExportKey(
        hKey,
        0,
        PRIVATEKEYBLOB,
        0,
        &keyBlob[0],
        &dwBlobLen))
    {
        printf("CryptExportKey 2 failed \n");
        return 1;
    }
	
	printf("Exported key len %d \n", dwBlobLen);

    out = std::move(keyBlob);
    return 0;
}

int exportPublicKeyBlob(HCRYPTKEY hKey, std::vector<BYTE> & out)
{
    std::vector<BYTE> publickeyBlob;
    DWORD publicBlobLen; 

    if (!::CryptExportKey(
        hKey,
        0,
        PUBLICKEYBLOB,
        0,
        NULL,
        &publicBlobLen))
    {
        printf("CryptExportKey pub 1 failed");
        return 1;
    }
    publickeyBlob.resize(publicBlobLen);
    if (!CryptExportKey(
        hKey,
        0,
        PUBLICKEYBLOB,
        0,
        publickeyBlob.data(),
        &publicBlobLen))
    {
        printf("CryptExportKey pub 2 failed");
        return 1;
    }
    out = std::move(publickeyBlob);
    printf("public key len: %d \n", publicBlobLen);
    return 0;
}

int main(){

    PCCERT_CONTEXT pc = CreateOurCertificate();
    if(!pc)
    {
        printf("pc is null");
        return 1;
    }

    HCRYPTPROV hCryptProv;
    DWORD keySpec = AT_KEYEXCHANGE;
    BOOL shouldFreeProv;
    if (!CryptAcquireCertificatePrivateKey(
        pc,
        CRYPT_ACQUIRE_COMPARE_KEY_FLAG | CRYPT_ACQUIRE_CACHE_FLAG,
        NULL,
        &hCryptProv,
        &keySpec,
        &shouldFreeProv))
    {
        printf("failed to CryptAcquireCertificatePrivateKey %x \n", GetLastError());
        return 1;
    }else{
        printf("success CryptAcquireCertificatePrivateKey  \n");
    }

    HCRYPTKEY hKey;
	if (!CryptGetUserKey(hCryptProv, keySpec,&hKey))
    {
        printf("CryptGetUserKey failed \n");
        return 1;
    }
    std::vector<BYTE> keyBlob;
    if(exportPrivatekeyBlob(hKey, keyBlob)){
        return 1;
    }

    std::vector<BYTE> pubKeyBlob;
    if(exportPublicKeyBlob(hKey, pubKeyBlob))
    {
        return 1;
    }

    // openssl code:
    EVP_PKEY *pkey;
    X509 *cert;
    STACK_OF(X509) *ca = NULL;
    PKCS12 *p12;
    int i;
    //CRYPTO_malloc_init();
    //OpenSSL_add_all_algorithms();
    //SSLeay_add_all_algorithms();
    //ERR_load_crypto_strings();

    BIO* input = BIO_new_mem_buf((void*)&pubKeyBlob[0], pubKeyBlob.size());
    p12 = d2i_PKCS12_bio(input, NULL);

    std::string password = "password";

    if(!PKCS12_parse(p12, password.c_str(), &pkey, &cert, &ca))
    {
        printf("PKCS12_parse failed \n");
        return 1;
    }
    PKCS12_free(p12);

    if (cert)
    {
        BIO *boCert = BIO_new( BIO_s_mem() );

        if(!PEM_write_bio_X509(boCert, cert))
        {
            printf("Failed PEM_write_bio_X509 \n");
            return 1;
        }
        if (ca && sk_X509_num(ca))
        {
            for (i = 0; i < sk_X509_num(ca); i++)
            {
                PEM_write_bio_X509(boCert, sk_X509_value(ca, i));
            }
        }
        char *certStr;
        long len = BIO_get_mem_data(boCert, &certStr);

        // QSslCertificate localCertificate(QByteArray::fromRawData(certStr, len));
        // mySslSocket->setLocalCertificate(localCertificate);

        BIO_free_all(boCert);
    }

    // if (pkey)
    // {
    //     BIO *bo = BIO_new( BIO_s_mem() );
    //     PEM_write_bio_PrivateKey(bo, pkey, NULL, (unsigned char*)(password.toStdString().c_str()), password.length(), NULL, (char*)(password.toStdString().c_str()));

    //     char *p;
    //     long len = BIO_get_mem_data(bo, &p);

    //     QSslKey key(QByteArray::fromRawData(p, len), QSsl::Rsa);
    //     mySslSocket->setPrivateKey(key);
    //     BIO_free_all(bo);
    // }

}