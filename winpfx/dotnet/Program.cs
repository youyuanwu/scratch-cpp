using System;
using System.IO;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;

namespace dotnet
{
    class Program
    {
        /// <summary>
        /// Parent cert is used to sign the child cert
        /// </summary>
        /// <param name="parentCert"></param>
        /// <returns></returns>
        static X509Certificate2 generateChildCert(X509Certificate2 parentCert)
        {
            using (RSA rsa = RSA.Create(2048))
            {
                CertificateRequest req = new CertificateRequest(
                    "CN=Valid-Looking Timestamp Authority",
                    rsa,
                    HashAlgorithmName.SHA256,
                    RSASignaturePadding.Pkcs1);

                req.CertificateExtensions.Add(
                    new X509BasicConstraintsExtension(false, false, 0, false));

                req.CertificateExtensions.Add(
                    new X509KeyUsageExtension(
                        X509KeyUsageFlags.DigitalSignature | X509KeyUsageFlags.NonRepudiation,
                        false));

                req.CertificateExtensions.Add(
                    new X509EnhancedKeyUsageExtension(
                        new OidCollection
                        {
                new Oid("1.3.6.1.5.5.7.3.8")
                        },
                        true));

                req.CertificateExtensions.Add(
                    new X509SubjectKeyIdentifierExtension(req.PublicKey, false));

                X509Certificate2 cert = req.Create(
                    parentCert,
                    DateTimeOffset.UtcNow,
                    DateTimeOffset.UtcNow.AddDays(90),
                    new byte[] { 1, 2, 3, 4 });

                var cert2 = RSACertificateExtensions.CopyWithPrivateKey(cert, rsa);

                if (cert2.HasPrivateKey)
                {
                    // this works.
                    Console.WriteLine("copied has private key");
                }
                return cert2;
                
            }
        }

        static X509Certificate2 GenParentCert()
        {
            using (RSA parent = RSA.Create(4096))
            {
                CertificateRequest parentReq = new CertificateRequest(
                    "CN=Experimental Issuing Authority",
                    parent,
                    HashAlgorithmName.SHA256,
                    RSASignaturePadding.Pkcs1);

                parentReq.CertificateExtensions.Add(
                    new X509BasicConstraintsExtension(true, false, 0, true));

                parentReq.CertificateExtensions.Add(
                    new X509SubjectKeyIdentifierExtension(parentReq.PublicKey, false));

                X509Certificate2 parentCert = parentReq.CreateSelfSigned(
                    DateTimeOffset.UtcNow.AddDays(-45),
                    DateTimeOffset.UtcNow.AddDays(365));
                return parentCert;
            }
        }

        /// <summary>
        /// cluster cert is not a CA cert (most likely). X509BasicConstraintsExtension is not set.
        /// So it cannot be used to process certificate signing request.
        /// </summary>
        /// <returns></returns>
        static X509Certificate2 ReadClusterCert()
        {
            // one can generate this from powershell
            string thumbprint = "D2D47595236E45CF56DFE1F322B4D793576F766C";
            var store = new X509Store(StoreName.My, StoreLocation.CurrentUser);

            try
            {
                store.Open(OpenFlags.ReadOnly);

                var certCollection = store.Certificates;
                var signingCert = certCollection.Find(X509FindType.FindByThumbprint, thumbprint, false);
                if (signingCert.Count == 0)
                {
                    throw new FileNotFoundException(string.Format("Cert with thumbprint: '{0}' not found in local machine cert store.", thumbprint));
                }
                Console.WriteLine($"found {signingCert.Count} certs from store");
                return signingCert[0];
            }
            finally
            {
                store.Close();
            }
        }


        static void Main(string[] args)
        {
            X509Certificate2 clusterCert = ReadClusterCert();

            if(clusterCert != null)
            {
                Console.WriteLine("cluster cert found");
            }

            Console.WriteLine("Hello World!");
            X509Certificate2 cert = generateChildCert(clusterCert);


            byte[] certificateBytes = cert.RawData;
            char[] certificatePem = PemEncoding.Write("CERTIFICATE", certificateBytes);
            Console.WriteLine(certificatePem);

            AsymmetricAlgorithm key = cert.GetRSAPrivateKey();
            if (cert.GetRSAPrivateKey() != null)
            {
                Console.WriteLine("has private rsa");
                key = cert.GetRSAPrivateKey();
            }

            if (cert.GetECDsaPrivateKey() != null)
            {
                Console.WriteLine("has private ecdsa");
                key = cert.GetRSAPrivateKey();
            }

            if (cert.GetDSAPrivateKey() != null)
            {
                Console.WriteLine("has private dsa");
                key = cert.GetRSAPrivateKey();
            }

            byte[] pubKeyBytes = key.ExportSubjectPublicKeyInfo();
            byte[] privKeyBytes = key.ExportPkcs8PrivateKey();
            char[] pubKeyPem = PemEncoding.Write("PUBLIC KEY", pubKeyBytes);
            char[] privKeyPem = PemEncoding.Write("PRIVATE KEY", privKeyBytes);

            Console.WriteLine(pubKeyPem);
            Console.WriteLine(privKeyPem);

        }
    }
}
