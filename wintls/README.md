
generate a test cert
```
New-SelfSignedCertificate -Type Custom -Provider "Microsoft Platform Crypto Provider" -Subject "CN=Patti Fuller" -TextExtension @("2.5.29.37={text}1.3.6.1.5.5.7.3.2","2.5.29.17={text}upn=pattifuller@contoso.com") -KeyExportPolicy NonExportable -KeyUsage DigitalSignature -KeyAlgorithm RSA -KeyLength 2048 -CertStoreLocation "Cert:\CurrentUser\My"
```

