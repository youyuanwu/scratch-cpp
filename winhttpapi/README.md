
```
New-SelfSignedCertificate -Type Custom -Subject "CN=Youyuan Wu Test" `
    -KeyExportPolicy NonExportable -KeyUsage DigitalSignature -KeyAlgorithm RSA -KeyLength 2048 -CertStoreLocation "Cert:\LocalMachine\My" `
    -Provider "Microsoft Platform Crypto Provider"

Get-ChildItem Cert:\LocalMachine\My\$thumbprint
```

This success:
netsh http add sslcert hostnameport=localhost:12356 appid='{4DBFB575-E1EF-4239-9A1D-E94CF84DC22D}' certhash=F5F37DB9023210DAE10D1E10247B7BA31ADD26FF certstorename=MY

netsh http delete sslcert hostnameport=localhost:12356