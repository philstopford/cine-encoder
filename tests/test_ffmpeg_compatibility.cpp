#include <QtTest>

#include "ffmpegcompatibility.h"

class TestFFmpegCompatibility : public QObject
{
    Q_OBJECT

private slots:
    void usesCanonicalFfmpeg9Options();
    void supportsLegacyOptionNames();
    void removesUnsupportedOptionalOptions();
};

void TestFFmpegCompatibility::usesCanonicalFfmpeg9Options()
{
    const QString help = QStringLiteral(
        "  -spatial-aq        <boolean>    Set Spatial AQ\n"
        "  -temporal-aq       <boolean>    Set Temporal AQ\n");
    const QStringList input = {"-spatial_aq", "1", "-temporal_aq", "1", "-bf", "2"};

    QCOMPARE(FFmpegCompatibility::adaptNvencAqOptions(input, help),
             QStringList({"-spatial-aq", "1", "-temporal-aq", "1", "-bf", "2"}));
}

void TestFFmpegCompatibility::supportsLegacyOptionNames()
{
    const QString help = QStringLiteral(
        "  -spatial_aq        <boolean>    Set Spatial AQ\n"
        "  -temporal_aq       <boolean>    Set Temporal AQ\n");
    const QStringList input = {"-spatial-aq", "0", "-temporal-aq", "0"};

    QCOMPARE(FFmpegCompatibility::adaptNvencAqOptions(input, help),
             QStringList({"-spatial_aq", "0", "-temporal_aq", "0"}));
}

void TestFFmpegCompatibility::removesUnsupportedOptionalOptions()
{
    const QStringList input = {"-rc", "vbr", "-spatial-aq", "1",
                               "-temporal-aq", "1", "-bf", "2"};

    QCOMPARE(FFmpegCompatibility::adaptNvencAqOptions(input, QStringLiteral("other options")),
             QStringList({"-rc", "vbr", "-bf", "2"}));
}

QTEST_APPLESS_MAIN(TestFFmpegCompatibility)
#include "test_ffmpeg_compatibility.moc"
