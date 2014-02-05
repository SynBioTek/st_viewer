/*
    Copyright (C) 2012  Spatial Transcriptomics AB,
    read LICENSE for licensing terms.
    Contact : Jose Fernandez Navarro <jose.fernandez.navarro@scilifelab.se>

*/

#ifndef ERROR_H
#define ERROR_H

#include <QDebug>

#include <QObject>
#include <QString>

//To add a new error, the qHash() functions needs to be called with the error descfiption text, then add
//the error hash to the error enum class so it can be parsed and recognized, amm also add it to the translation file...

//this abstract class is the base class for errors, it contains different information ( level, type(hash), name and description)
class Error : public QObject
{
    Q_OBJECT

public:
    explicit Error(QObject* parent = 0);
    Error(const QString& name, const QString& description = QString(), QObject* parent = 0);
    virtual ~Error();
    inline const QString& name() const
    {
        return m_name;
    }
    inline const QString& description() const
    {
        return m_description;
    }

protected:
    inline void name(const QString& name)
    {
        m_name = name;
    }
    inline void description(const QString& description)
    {
        m_description = description;
    }

private:
    QString m_name;
    QString m_description;
};

#endif // ERROR_H //