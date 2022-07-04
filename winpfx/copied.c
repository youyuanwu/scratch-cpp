#include "windows.h"
#include "wincrypt.h"
#include "stdio.h"

/* This code example:
1) creates a Crypto Service Provider
2) generates keys
3) extracts public key
4) exports private key into PEM
5) destroys and recreates CSP
6) imports keys from the private key PEM into the newly created CSP.
It is not clear why, after loading private key from PEM,
it is possible to get both private and public keys from the
private key PEM using CryptExportKey(hPrivateKey,...), but
ExportPublicKeyInfo()  API call fails with "Key does not exist" error.
*/

int main()
{
	/* get provider */
	HCRYPTPROV hProv = 0;
	DWORD rc = CryptAcquireContext(&hProv,
		NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES,
		CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	if (!rc) goto bad;
	
	/* generate private key */
	DWORD flags = 1024 /*key length*/ << 16;
	flags |= CRYPT_EXPORTABLE;
	HCRYPTKEY hPrivateKey = 0;
	rc = CryptGenKey(hProv, AT_KEYEXCHANGE, flags, &hPrivateKey);
	if (!rc) goto bad;
	
	/* extract public key */
	DWORD size = 0;
	rc = CryptExportPublicKeyInfo(hProv,
		AT_KEYEXCHANGE, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		NULL, &size);
	if (!rc) goto bad;

	PCERT_PUBLIC_KEY_INFO pKeyInfo = 
		(PCERT_PUBLIC_KEY_INFO) LocalAlloc(0, size);
	rc = CryptExportPublicKeyInfo(hProv,
		AT_KEYEXCHANGE, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		pKeyInfo, &size);
	if (!rc) goto bad;

	HCRYPTKEY hPublicKey = 0;
	rc = CryptImportPublicKeyInfo(hProv,
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		pKeyInfo, &hPublicKey);
	if (!rc) goto bad;

	/* export private key */
	rc = CryptExportKey(hPrivateKey, 0, PRIVATEKEYBLOB, 0, 0, &size);
	if (!rc) goto bad;

	LPBYTE pPrivKeyBLOB = (LPBYTE) LocalAlloc(0, size);
	rc = CryptExportKey(hPrivateKey, 0, PRIVATEKEYBLOB, 0,
		pPrivKeyBLOB, &size);
	if (!rc) goto bad;

	/* save private key as PEM */

	/* DER */
	rc = CryptEncodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		PKCS_RSA_PRIVATE_KEY, pPrivKeyBLOB, 0, NULL, NULL, &size);
	if (!rc) goto bad;

	LPBYTE pDER = (LPBYTE) LocalAlloc(0, size);
	rc = CryptEncodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		PKCS_RSA_PRIVATE_KEY, pPrivKeyBLOB, 0, NULL, pDER, &size);
	if (!rc) goto bad;

	/* PEM */
	DWORD pemSize = 0;
	rc = CryptBinaryToStringA(pDER, size,
		CRYPT_STRING_BASE64HEADER, NULL, &pemSize);
	if (!rc) goto bad;

	LPSTR pPEM = (LPSTR)LocalAlloc(0, pemSize);
	rc = CryptBinaryToStringA(pDER, size,
		CRYPT_STRING_BASE64HEADER, pPEM, &pemSize);
	if (!rc) goto bad;
	printf("%s", pPEM);

	/*************************************************************/
	/* start fresh and import both keys from the private key PEM */
	/*************************************************************/

	CryptDestroyKey(hPublicKey);
	CryptDestroyKey(hPrivateKey);
	CryptReleaseContext(hProv, 0);

	rc = CryptAcquireContext(&hProv,
		NULL, MS_ENH_RSA_AES_PROV, PROV_RSA_AES,
		CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
	if (!rc) goto bad;

	/* back to DER */
	rc = CryptStringToBinaryA(pPEM, pemSize,
		CRYPT_STRING_BASE64HEADER, NULL, &size, NULL, NULL);
	if (!rc) goto bad;

	LocalFree(pDER);
	pDER = (LPBYTE)LocalAlloc(0, size);
	rc = CryptStringToBinaryA(pPEM, pemSize,
		CRYPT_STRING_BASE64HEADER, pDER, &size, NULL, NULL);
	if (!rc) goto bad;

	/* get private key blob */
	DWORD derSize = 0;
	rc = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		PKCS_RSA_PRIVATE_KEY, pDER, size, 0, NULL, NULL, &derSize);
	if (!rc) goto bad;

	LocalFree(pPrivKeyBLOB);
	pPrivKeyBLOB = (LPBYTE)LocalAlloc(0, derSize);
	rc = CryptDecodeObjectEx(X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		PKCS_RSA_PRIVATE_KEY, pDER, size, 0, NULL, pPrivKeyBLOB, &derSize);
	if (!rc) goto bad;

	/* import private key */
	rc = CryptImportKey(hProv, pPrivKeyBLOB, size, 0, CRYPT_EXPORTABLE, &hPrivateKey);
	if (!rc) goto bad;

	/* export public key */
	rc = CryptExportKey(hPrivateKey, 0, PUBLICKEYBLOB, 0, 0, &size);
	if (!rc) goto bad;

	LPBYTE pPubKeyBLOB = (LPBYTE)LocalAlloc(0, size);
	rc = CryptExportKey(hPrivateKey, 0, PUBLICKEYBLOB, 0, pPubKeyBLOB, &size);
	if (!rc) goto bad;
	
	/* import public key */
	rc = CryptImportKey(hProv, pPubKeyBLOB, size, 0, CRYPT_EXPORTABLE, &hPublicKey);
	if (!rc) goto bad;

	/* import public key */
	rc = CryptExportPublicKeyInfo(hProv,
		AT_KEYEXCHANGE, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		NULL, &size);
	if (!rc) goto bad;

	LocalFree(pKeyInfo);
	pKeyInfo = (PCERT_PUBLIC_KEY_INFO)LocalAlloc(0, size);
	rc = CryptExportPublicKeyInfo(hProv,
		AT_KEYEXCHANGE, X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		pKeyInfo, &size);
	if (!rc) goto bad;

	hPublicKey = 0;
	rc = CryptImportPublicKeyInfo(hProv,
		X509_ASN_ENCODING | PKCS_7_ASN_ENCODING,
		pKeyInfo, &hPublicKey);
	if (!rc) goto bad;

	/* clean up */
	if (hPublicKey) CryptDestroyKey(hPublicKey);
	if (hPrivateKey) CryptDestroyKey(hPrivateKey);
	if (hProv) CryptReleaseContext(hProv, 0);

	LocalFree(pPEM);
	LocalFree(pDER);
	LocalFree(pPrivKeyBLOB);
	LocalFree(pPubKeyBLOB);
	LocalFree(pKeyInfo);

	return 0;

bad:
	DWORD errorCode = GetLastError();
	DWORD dwFlg = FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS;
	LPTSTR lpMsgBuf = 0;
	FormatMessageA(dwFlg, 0, errorCode, 0, (LPTSTR)& lpMsgBuf, 0, NULL);
	printf("%s", lpMsgBuf);
	LocalFree(lpMsgBuf);

	if (hPublicKey) CryptDestroyKey(hPublicKey);
	if (hPrivateKey) CryptDestroyKey(hPrivateKey);
	if (hProv) CryptReleaseContext(hProv, 0);

	LocalFree(pPEM);
	LocalFree(pDER);
	LocalFree(pPrivKeyBLOB);
	LocalFree(pPubKeyBLOB);
	LocalFree(pKeyInfo);

	return -1;
}