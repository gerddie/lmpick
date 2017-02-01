#include "qruntimeexeption.hh"

QRuntimeExeption::QRuntimeExeption(const QString msg):
        m_msg(msg)
{

}


void QRuntimeExeption::raise() const
{
        throw *this;
}
QRuntimeExeption *QRuntimeExeption::clone() const
{
        return new QRuntimeExeption(*this);
}

const QString& QRuntimeExeption::qwhat() const
{
        return m_msg;
}
