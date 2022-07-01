#include <stdio.h>

#include <windows.h>
#include <Wincrypt.h>

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

int main() {

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

    CRYPT_DATA_BLOB pfxBlob;

    pfxBlob.pbData = (BYTE*)keyBlob.data();
    pfxBlob.cbData = (DWORD)keyBlob.size();
	
	DWORD derBlobLen;
	std::vector<BYTE> pPrivDer;
	
	if(!CryptEncodeObjectEx(X509_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, pfxBlob.pbData, 0, NULL, NULL, &derBlobLen))
    {
        printf("CryptEncodeObjectEx 1 failed \n");
        return 1;
    }
    pPrivDer.resize(derBlobLen);
    if(!CryptEncodeObjectEx(X509_ASN_ENCODING, PKCS_RSA_PRIVATE_KEY, pfxBlob.pbData, 0, NULL, pPrivDer.data(), &derBlobLen))
    {
        printf("CryptEncodeObjectEx 2 failed \n");
        return 1;
    }
/* Convert DER to PEM */
    DWORD dwLen = 0;
    pfxBlob.pbData = (BYTE*)pPrivDer.data();
    pfxBlob.cbData = (DWORD)pPrivDer.size();
    if (!CryptBinaryToStringA(pfxBlob.pbData, pfxBlob.cbData, CRYPT_STRING_BASE64, NULL, &dwLen))
    {
        printf("CryptBinaryToStringA 1 failed \n");
        return 1;
    }
        
    std::vector<char> private_strOut;
    private_strOut.resize(dwLen);
    if (!CryptBinaryToStringA(pfxBlob.pbData, pfxBlob.cbData, CRYPT_STRING_BASE64, &private_strOut[0], &dwLen))
    {
        printf("CryptBinaryToStringA 2 failed \n");
        return 1;
    }

    std::cout << "-----BEGIN PRIVATE KEY-----" << std::endl;
    std::cout << std::string(private_strOut.begin(), private_strOut.end());
    std::cout << "-----END PRIVATE KEY-----" << std::endl;

    // public key
    HCRYPTPROV hCryptPubProv;
    if (!CryptAcquireContext(&hCryptPubProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
        printf("CryptAcquireContext failed");
        return 1;
    }
    HCRYPTKEY hCertPubKey;
    // Get the public key information for the certificate.
    if( !CryptImportPublicKeyInfo(
        hCryptPubProv, 
        X509_ASN_ENCODING, 
		&pc->pCertInfo->SubjectPublicKeyInfo, 
        &hCertPubKey))
    {
        printf("CryptImportPublicKeyInfo failed");
        return 1;
    }
    std::vector<BYTE> publickeyBlob;
    
    if(exportPublicKeyBlob(hCertPubKey, publickeyBlob))
    {
        return 1;
    }

	/* Convert PKCS12 to DER */
    pfxBlob.pbData = (BYTE*)publickeyBlob.data();
    pfxBlob.cbData = (DWORD)publickeyBlob.size();
	
	// export public key info
    // this fails not sure why
	// DWORD  pkiLen;
	// CERT_PUBLIC_KEY_INFO pki;
    // if(!CryptExportPublicKeyInfo(hCryptPubProv, AT_KEYEXCHANGE,
    //                                      X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
    //                                      NULL, &pkiLen))
    // {
    //     printf("CryptExportPublicKeyInfo 1 failed %x", GetLastError());
    //     return 1;
    // }
	// publickeyBlob.resize(pkiLen);
	// if(!CryptExportPublicKeyInfo(hCryptPubProv, AT_KEYEXCHANGE,
    //                                      X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
    //                                      &pki, &pkiLen))
    // {
    //     printf("CryptExportPublicKeyInfo 2 failed");
    //     return 1;
    // }
    // printf("public key info len %d", pkiLen);
	// pfxBlob.pbData = (BYTE*)pki.PublicKey.pbData;
    // pfxBlob.cbData = (DWORD)pki.PublicKey.cbData;
	

    DWORD derPublicBlobLen;
	std::vector<BYTE> publicDer;
	if(!CryptEncodeObjectEx(X509_ASN_ENCODING, RSA_CSP_PUBLICKEYBLOB, pfxBlob.pbData, 0, NULL, NULL, &derPublicBlobLen))
    {
        printf("CryptEncodeObjectEx 1 failed: %x", GetLastError());
        return 1;
    }
    publicDer.resize(derPublicBlobLen);
    if(!CryptEncodeObjectEx(X509_ASN_ENCODING, RSA_CSP_PUBLICKEYBLOB, pfxBlob.pbData, 0, NULL, publicDer.data(), &derPublicBlobLen))
    {
        printf("CryptEncodeObjectEx 2 failed");
        return 1;
    }
/* Convert DER to PEM */
    DWORD publicdwLen = 0;
	pfxBlob.pbData = (BYTE*)publicDer.data();
    pfxBlob.cbData = (DWORD)publicDer.size();
    if (!CryptBinaryToStringA(pfxBlob.pbData, pfxBlob.cbData, CRYPT_STRING_BASE64HEADER, NULL, &publicdwLen))
    {
        printf("CryptBinaryToStringA 1 failed %x", GetLastError());
        return 1;
    }

    std::vector<char> strOut;
    strOut.resize(publicdwLen);
    if (!CryptBinaryToStringA(pfxBlob.pbData, pfxBlob.cbData, CRYPT_STRING_BASE64HEADER, &strOut[0], &publicdwLen))
    {
       printf("CryptBinaryToStringA 2 failed");
    }

    std::cout << "Public Key:" << std::endl;
    std::cout << std::string(strOut.begin(), strOut.end()) << std::endl;
}