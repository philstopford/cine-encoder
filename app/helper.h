/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: helper.h
 COMMENT: Utility functions for file handling, UI operations, and string
          manipulation. Provides static helper methods used throughout the
          application for common tasks.
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QString>
#include <QVector>


class Helper : public QObject
{
public:
    explicit Helper();
    ~Helper() override;

    /**
     * @brief File dialog types for different file selection scenarios
     */
    enum class FileDialogType : uint8_t {
        OPENVFILES,     ///< Open video files
        OPENAFILES,     ///< Open audio files  
        OPENSFILES,     ///< Open subtitle files
        SELECTFOLDER    ///< Select a folder
    };

    /**
     * @brief Desktop environment types for platform-specific behavior
     */
    enum class DesktopEnv : uint8_t {
        GNOME,    ///< GNOME desktop environment
        OTHER,    ///< Other desktop environments
        UNDEF     ///< Undefined/not detected
    };

    /**
     * @brief Prepare a list of strings for FFmpeg command line by escaping special characters
     * @param stringList List of strings to process
     * @return Processed list safe for FFmpeg usage
     */
    static QStringList makeStringsFFMPEGReady(const QStringList& stringList);
    
    /**
     * @brief Escape special characters in a filename/path for FFmpeg
     * @param fileString Original file path or name
     * @return Escaped string safe for FFmpeg filters
     */
    /**
     * @brief Escape special characters in a filename/path for FFmpeg
     * @param fileString Original file path or name
     * @return Escaped string safe for FFmpeg filters
     */
    static QString makeFileStringFFMPEGReady(const QString& fileString);

    /**
     * @brief Escape special characters in a filename/path for FFmpeg filter syntax
     * @param fileString Original file path or name
     * @return Escaped string safe for FFmpeg filters like subtitles='...'
     */
    static QString makeFileStringFFMPEGFilterReady(const QString& fileString);

    /**
     * @brief Detect the current desktop environment (Linux only)
     */
    static void detectEnv();
    
    /**
     * @brief Get the detected desktop environment
     * @return Current desktop environment
     */
    static DesktopEnv getEnv();
    
    /**
     * @brief Parse CSS string with variable substitution
     * @param list CSS string potentially containing variables (@varname=value)
     * @return Parsed CSS with variables substituted
     */
    static QString getParsedCss(const QString &list);
    
    /**
     * @brief Open a file selection dialog
     * @param dialogType Type of files to select
     * @param title Dialog window title
     * @param path Initial directory path
     * @param result[out] Selected file paths
     */
    static void openFileDialog(FileDialogType dialogType,
                               const QString  &title,
                               const QString  &path,
                               QStringList    &result);
    
    /**
     * @brief Convert time in seconds to HH:MM:SS format
     * @param time Time in seconds (float)
     * @return Formatted time string
     */
    static QString timeConverter(float time);
    
    /**
     * @brief Convert time in seconds to HH:MM:SS format
     * @param time Time in seconds (double)
     * @return Formatted time string
     */
    static QString timeConverter(double time);
    
    /**
     * @brief Get system language code
     * @return Two-letter language code (e.g., "en", "fr")
     */
    static QString getSysLanguage();
    
    /**
     * @brief Elide text to fit within a widget's width
     * @param w Widget to measure width from
     * @param text Text to elide
     * @param elide Elide mode (left, middle, right)
     * @return Elided text with ellipsis
     */
    static QString elideText(QWidget *w,
                             const QString &text,
                             Qt::TextElideMode elide);
    
    /**
     * @brief Recalculate channel count from string representation
     * @param ch Channel count string
     * @return Standardized channel count string
     */
    static QString recalcChannels(const QString &ch);
    
    /**
     * @brief Get CSS stylesheet for the specified theme
     * @param theme_index Theme index (0-based)
     * @return CSS stylesheet string
     */
    static QString getCss(int theme_index);
    
    /**
     * @brief Check if audio format is supported
     * @param extension File extension
     * @param format Audio format/codec
     * @return true if supported
     */
    static bool isAudioSupported(const QString& extension, const QString &format);
    
    /**
     * @brief Check if video format is supported
     * @param extension File extension
     * @param format Video format/codec
     * @return true if supported
     */
    static bool isVideoSupported(const QString& extension, const QString &format);
    
    /**
     * @brief Check if subtitle format is supported
     * @param extension File extension
     * @param format Subtitle format
     * @return true if supported
     */
    static bool isSubtitleSupported(const QString& extension, const QString &format);
    
    /**
     * @brief Check if audio is incompatible with target codec
     * @param extension File extension
     * @param format Audio format/codec
     * @param targetAudioCodec Target codec for encoding
     * @return true if incompatible (needs re-encoding)
     */
    static bool isAudioIncompatible(const QString& extension, const QString &format, const QString& targetAudioCodec);
    
    /**
     * @brief Check if subtitle is incompatible with preset settings
     * @param extension File extension
     * @param format Subtitle format
     * @param usePresetSubtitleSettings Whether to use preset subtitle settings
     * @return true if incompatible
     */
    static bool isSubtitleIncompatible(const QString& extension, const QString &format, bool usePresetSubtitleSettings);
    
    /**
     * @brief Remove an element from a vector by row index
     * @tparam T Type of vector elements
     * @param v Vector to modify
     * @param row Index of element to remove
     */
    template<class T>
    static void eraseRow(QVector<T> &v, int row);
    
    /**
     * @brief Reorder vector elements according to an ordering specification
     * @tparam T Type of vector elements
     * @param v Vector to reorder
     * @param order Order specification vector
     */
    template<class T>
    static void reorder(QVector<T> &v, QVector<int> const &order);
    
    /**
     * @brief Non-blocking delay using event loop
     * @param msec Delay in milliseconds
     */
    static void nonBlockDelay(int msec);
    
    /**
     * @brief Get UI scaling factor based on screen DPI
     * @return Scaling factor (1.0 = normal, >1.0 = high DPI)
     */
    static double scaling();
    
    /**
     * @brief Read entire file contents into a byte array
     * @param path File path
     * @return File contents as byte array
     */
    static QByteArray readFile(const QString &path);

private:
    static DesktopEnv m_desktopEnv;  ///< Cached desktop environment
};

template<class T>
void Helper::eraseRow(QVector<T> &v, const int row)
{
    auto it = v.begin();
    std::advance(it, row);
    v.erase(it);
}

template<class T>
void Helper::reorder(QVector<T> &v, QVector<int> const &order)
{
    for (int s = 1, d; s < order.size(); ++s) {
        for (d = order[s]; d < s; d = order[d]);
        if (d == s)
            while (d = order[d], d != s)
                std::swap(v[s], v[d]);

    }
}

#endif // HELPER_H
