#include "pwstore.h"

#include <QCryptographicHash>
#include <QSettings>
#include <QList>
#include <QNetworkInterface>

#include <cryptopp/osrng.h>
#include <cryptopp/gcm.h>
#include <cryptopp/aes.h>
#include <cryptopp/authenc.h>

#ifdef _WIN32
#include <windows.h>
#include <intrin.h>

#define IV_SIZE 16
#define KEY_SIZE 16

QByteArray fingerprint();

PwStore::PwStore() {
    m_key = fingerprint().left(KEY_SIZE);
}

QByteArray PwStore::encrypt(QString s) {
    std::string ciphertext;

    // Random IV
    QByteArray iv;
    iv.resize(IV_SIZE);
    CryptoPP::OS_GenerateRandomBlock(false, (byte*)iv.data(), IV_SIZE);

    CryptoPP::GCM<CryptoPP::AES>::Encryption enc;
    enc.SetKeyWithIV((byte*)m_key.data(), m_key.length(), (byte*)iv.data(), iv.length());

    CryptoPP::AuthenticatedEncryptionFilter aef( enc,
        new CryptoPP::StringSink( ciphertext )
    );

    QByteArray sBytes(s.toUtf8());

    aef.Put((byte*)sBytes.data(), sBytes.size());
    aef.MessageEnd();

    return iv + QByteArray(ciphertext.data(), ciphertext.length());
}

QString PwStore::decrypt(QByteArray s) {
    QByteArray ciphertext, iv;
    iv = s.left(IV_SIZE);
    ciphertext = s.mid(IV_SIZE);

    std::string plaintext;

    CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
    dec.SetKeyWithIV((byte*)m_key.data(), m_key.length(), (byte*)iv.data(), iv.length());

    try {
        CryptoPP::AuthenticatedDecryptionFilter adf( dec,
            new CryptoPP::StringSink( plaintext )
        );

        adf.Put((byte*)ciphertext.data(), ciphertext.size());
        adf.MessageEnd();
    }
    catch (CryptoPP::HashVerificationFilter::HashVerificationFailed) {
        return QString();
    }

    QByteArray plaintextBytes(QByteArray::fromStdString(plaintext));

    return QString::fromUtf8(plaintextBytes);
}

void secureStringClear(QString &s) {
    int l = s.length();
    for (int i=0; i<l; i++) {
        s[i] = '\0';
    }
    s.clear();
}

QByteArray getMacs() {
    foreach(QNetworkInterface netInterface, QNetworkInterface::allInterfaces()) {
        if (netInterface.flags() & QNetworkInterface::IsLoopBack) {
            continue;
        }
        return netInterface.hardwareAddress().toLatin1();
    }
    return QByteArray();
}

QByteArray getMachineName() {
   static wchar_t computerName[1024];
   DWORD size = 1024;
   GetComputerName( computerName, &size );
   return QString::fromWCharArray(computerName).toLatin1();
}

QByteArray getMachineGUID() {
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Cryptography", QSettings::NativeFormat);
    return settings.value("MachineGuid", "0").toByteArray();
}

QByteArray getVolumeHash() {
   DWORD serialNum = 0;

   GetVolumeInformation( L"c:\\", NULL, 0, &serialNum, NULL, NULL, NULL, 0 );

   return QByteArray((char*)&serialNum, 4);
}

QByteArray fingerprint() {
    const int rounds = 1000;

    QByteArray buffer;
    buffer += getVolumeHash();
    buffer += getMachineGUID();
    buffer += getMachineName();
    buffer += getMacs();

    for (int i=0; i<rounds; i++) {
        buffer = QCryptographicHash::hash(buffer, QCryptographicHash::Sha1);
    }

    return buffer;
}

#endif
