#ifndef FILEICONPROVIDER_H
#define FILEICONPROVIDER_H

#include <QFileIconProvider>


class FileIconProvider : public QFileIconProvider
{
public:
    explicit FileIconProvider();
    ~FileIconProvider() override;
    [[nodiscard]] QIcon icon(const QFileInfo &info) const override;
    [[nodiscard]] QIcon icon(const QFileIconProvider::IconType ic) const override;
};

#endif // FILEICONPROVIDER_H
