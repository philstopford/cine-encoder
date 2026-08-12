#ifndef FFMPEGCOMPATIBILITY_H
#define FFMPEGCOMPATIBILITY_H

#include <QString>
#include <QStringList>

class FFmpegCompatibility
{
public:
    static QStringList adaptArgumentsToHost(const QStringList &arguments);
    static QStringList adaptNvencAqOptions(const QStringList &arguments,
                                           const QString &encoderHelp);

private:
    static QString videoEncoder(const QStringList &arguments);
};

#endif
