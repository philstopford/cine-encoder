#ifndef FFMPEGCOMPATIBILITY_H
#define FFMPEGCOMPATIBILITY_H

#include <QString>
#include <QStringList>

class FFmpegCompatibility
{
public:
    enum class SubtitleFilterStatus {
        Available,
        Missing,
        Unavailable
    };

    static QStringList adaptArgumentsToHost(const QStringList &arguments);
    static QStringList adaptNvencAqOptions(const QStringList &arguments,
                                           const QString &encoderHelp);
    static SubtitleFilterStatus subtitleFilterStatus();
    static bool hasSubtitleFilter(const QString &filters);

private:
    static QString videoEncoder(const QStringList &arguments);
};

#endif
