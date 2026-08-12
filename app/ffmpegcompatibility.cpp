#include "ffmpegcompatibility.h"

#include <QProcess>
#include <QRegularExpression>

namespace {

bool hasOption(const QString &help, const QString &option)
{
    const QRegularExpression expression(
        QStringLiteral("(?:^|\\n)\\s*-") + QRegularExpression::escape(option)
        + QStringLiteral("(?:\\s|$)"));
    return expression.match(help).hasMatch();
}

void adaptOption(QStringList &arguments, const QString &canonical,
                 const QString &legacy, const QString &help)
{
    const bool supportsCanonical = hasOption(help, canonical.mid(1));
    const bool supportsLegacy = hasOption(help, legacy.mid(1));

    for (qsizetype i = 0; i < arguments.size();) {
        if (arguments.at(i) != canonical && arguments.at(i) != legacy) {
            ++i;
            continue;
        }

        if (supportsCanonical) {
            arguments[i] = canonical;
            i += 2;
        } else if (supportsLegacy) {
            arguments[i] = legacy;
            i += 2;
        } else {
            arguments.removeAt(i);
            if (i < arguments.size())
                arguments.removeAt(i); // option value
        }
    }
}

} // namespace

QString FFmpegCompatibility::videoEncoder(const QStringList &arguments)
{
    for (qsizetype i = 0; i + 1 < arguments.size(); ++i) {
        if (arguments.at(i) == QStringLiteral("-c:v")
            || arguments.at(i) == QStringLiteral("-vcodec"))
            return arguments.at(i + 1);
    }
    return {};
}

QStringList FFmpegCompatibility::adaptNvencAqOptions(const QStringList &arguments,
                                                      const QString &encoderHelp)
{
    QStringList adapted = arguments;
    adaptOption(adapted, QStringLiteral("-spatial-aq"),
                QStringLiteral("-spatial_aq"), encoderHelp);
    adaptOption(adapted, QStringLiteral("-temporal-aq"),
                QStringLiteral("-temporal_aq"), encoderHelp);
    return adapted;
}

QStringList FFmpegCompatibility::adaptArgumentsToHost(const QStringList &arguments)
{
    const QString encoder = videoEncoder(arguments);
    if (!encoder.endsWith(QStringLiteral("_nvenc")))
        return arguments;

    QProcess probe;
    probe.setProcessChannelMode(QProcess::MergedChannels);
    probe.start(QStringLiteral("ffmpeg"),
                {QStringLiteral("-hide_banner"), QStringLiteral("-h"),
                 QStringLiteral("encoder=") + encoder});

    // Canonical hyphenated names work across supported FFmpeg releases. If the
    // host cannot be queried, use those instead of retaining removed aliases.
    if (!probe.waitForStarted(3000) || !probe.waitForFinished(3000)) {
        QStringList canonical = arguments;
        canonical.replaceInStrings(QStringLiteral("-spatial_aq"),
                                   QStringLiteral("-spatial-aq"));
        canonical.replaceInStrings(QStringLiteral("-temporal_aq"),
                                   QStringLiteral("-temporal-aq"));
        return canonical;
    }

    return adaptNvencAqOptions(arguments, QString::fromLocal8Bit(probe.readAll()));
}
