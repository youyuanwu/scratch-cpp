#pragma once

#include <stdio.h>

#include <windows.h>
#include <Wincrypt.h>

#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)

PCCERT_CONTEXT findCert(){

    HCERTSTORE  hSystemStore;              // System store handle
    PCCERT_CONTEXT  pDesiredCert = NULL;
    // PCCERT_CONTEXT  pCertContext;

    hSystemStore = CertOpenStore(
     CERT_STORE_PROV_SYSTEM, // System store will be a 
                             // virtual store
     0,                      // Encoding type not needed 
                             // with this PROV
     NULL,                   // Accept the default HCRYPTPROV
     CERT_SYSTEM_STORE_CURRENT_USER,
                             // Set the system store location in the
                             // registry
     L"MY");                 // Could have used other predefined 
                             // system stores
                             // including Trust, CA, or Root
    if(hSystemStore)
    {
    printf("Opened the MY system store. \n");
    }
    else
    {

        printf("Cannot Opened the MY system store. \n");
        return pDesiredCert;
    }

    // find my cert
    pDesiredCert=CertFindCertificateInStore(
      hSystemStore,
      MY_ENCODING_TYPE,             // Use X509_ASN_ENCODING
      0,                            // No dwFlags needed 
      CERT_FIND_SUBJECT_STR,        // Find a certificate with a
                                    // subject that matches the 
                                    // string in the next parameter
      L"Patti Fuller",              // The Unicode string to be found
                                    // in a certificate's subject
      NULL);                        // NULL for the first call to the
                                    // function 
                                    // In all subsequent
                                    // calls, it is the last pointer
                                    // returned by the function
    if(pDesiredCert)
    {
        printf("The desired certificate was found. \n");
    }
    else
    {
        printf("The desired certificate was not found. \n");
        return pDesiredCert;
    }

    if(hSystemStore)
        CertCloseStore(
            hSystemStore, 
            0);

    return pDesiredCert;
}