#ifndef QRUNTIMEEXEPTION_HH
#define QRUNTIMEEXEPTION_HH

#include <QString>
#include <QException>

class QRuntimeExeption : public QException
{
public:
        QRuntimeExeption(const QString msg);
        QRuntimeExeption(const QRuntimeExeption& other) = default;

        void raise() const override;
        QRuntimeExeption *clone() const override;
        const QString& qwhat() const;
private:
        QString m_msg;
};

#endif // QRUNTIMEEXEPTION_HH
