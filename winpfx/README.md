# WinPfx
Generated and export pfx using c/c++ win32 apis.

# powershell way

```
-KeyExportPolicy NonExportable
```

generate a test cert exportable
```
New-SelfSignedCertificate -Type Custom -Subject "CN=Youyuan Wu Test" `
    -KeyExportPolicy NonExportable -KeyUsage DigitalSignature -KeyAlgorithm RSA -KeyLength 2048 -CertStoreLocation "Cert:\CurrentUser\My" `
    -Provider "Microsoft Platform Crypto Provider"
New-SelfSignedCertificate -Type Custom -Subject "CN=Youyuan Wu Test" `
    -KeyExportPolicy Exportable -KeyUsage DigitalSignature -KeyAlgorithm RSA -KeyLength 2048 -CertStoreLocation "Cert:\CurrentUser\My"`
    -Provider "Microsoft Software Key Storage Provider"
New-SelfSignedCertificate -Type Custom -Subject "CN=Youyuan Wu Test" `
    -KeyExportPolicy Exportable -KeyUsage DigitalSignature -KeyAlgorithm RSA -KeyLength 2048 -CertStoreLocation "Cert:\CurrentUser\My"`
    -Provider "Microsoft Enhanced RSA and AES Cryptographic Provider" // not found
```
Default `Microsoft Software Key Storage Provider` is used. This does not work with win api and sf. 
See https://github.com/microsoft/service-fabric/issues/773
Use `-Provider "Microsoft Platform Crypto Provider"` makes the key gen fail, since it does not support key export?


```
$thumbprint= "8EE57F5BEDF21596FCFAC65808E98B93E0A89FB4"
```

remove cert
```
Get-ChildItem Cert:\CurrentUser\My\$thumbprint | Remove-Item
```

export cert
There is no password so any string will work?
```
$mypwd = ConvertTo-SecureString -String "1234" -Force -AsPlainText
Get-ChildItem -Path Cert:\CurrentUser\My\$thumbprint | Export-PfxCertificate -FilePath .\mypfx.pfx -Password $mypwd
```

install PSPKI module
```
Install-Module -Name PSPKI
Import-Module PSPKI
```
convert pfx. This errors out on provider invalid
Hitting issue: `https://github.com/PKISolutions/PSPKI/issues/154`
```
Convert-PfxToPem -InputFile .\mypfx.pfx -Outputfile .\mypfx.pem -Password $mypwd
```
get providers
```
Get-CryptographicServiceProvider | Format-List -Property *
```


this can convert to pem. But does not work for private key?
```
certutil.exe -encode .\mypfx.pfx .\mypfx.pem
```

verify pub priv key match using openssl:
```
openssl x509 -noout -modulus -in server.crt| openssl md5
openssl rsa -noout -modulus -in server.key| openssl md5
```

TODO:
Openssl cannot use the keys. Maybe need to try openssl lib to parse windows blobs.

# Install openssl