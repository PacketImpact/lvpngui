#ifndef PWSTORE_H
#define PWSTORE_H

#include <QString>
#include <QByteArray>

void secureStringClear(QString &s);

/*
 * Slightly-better-than-plaintext authenticated password encryption.
 * It uses a machine fingerprint as a key and a random IV stored with
 * the encrypted data, and encrypts with AES-GCM.
 * If it cannot decrypt, an empty string is returned.
 */
class PwStore
{
public:
    PwStore();
    QByteArray encrypt(QString s);
    QString decrypt(QByteArray s);

private:
    QByteArray m_key;
};

#endif // PWSTORE_H
