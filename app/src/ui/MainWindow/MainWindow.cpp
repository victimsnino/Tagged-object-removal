#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <rxqt.hpp>

#include <QFileDialog>
#include <QStandardPaths>
#include <QImageReader>
#include <QImageWriter>

static void InitializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode)
{
    dialog.setDirectory(QDir::currentPath());

    QStringList          mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen ?
                                                  QImageReader::supportedMimeTypes() :
                                                  QImageWriter::supportedMimeTypes();
    for (const QByteArray& mimeTypeName : supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);

    mimeTypeFilters.sort();
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");
    dialog.setAcceptMode(acceptMode);
    if (acceptMode == QFileDialog::AcceptSave)
        dialog.setDefaultSuffix("jpg");
}

static void UpdateImage(QLabel& image, const QPixmap& pixmap)
{
    if (pixmap.isNull())
        return;

    auto pixmapWidth  = pixmap.width();
    auto pixmapHeight = pixmap.height();
    if (pixmapWidth <= 0 || pixmapHeight <= 0)
        return;

    int w = image.width();
    int h = image.height();

    if (w <= 0 || h <= 0)
        return;

    if (w * pixmapHeight > h * pixmapWidth)
    {
        int m = (w - (pixmapWidth * h / pixmapHeight)) / 2;
        image.setContentsMargins(m, 0, m, 0);
    }
    else
    {
        int m = (h - (pixmapHeight * w / pixmapWidth)) / 2;
        image.setContentsMargins(0, m, 0, m);
    }

    image.setPixmap(pixmap.scaled(image.size(),
                                  Qt::IgnoreAspectRatio,
                                  Qt::SmoothTransformation));
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_ui(std::make_unique<Ui::MainWindow>())
{
    m_ui->setupUi(this);

    rxqt::from_signal(m_ui->load_image, &QPushButton::clicked)
            .map([&](const auto&)
            {
                QFileDialog dialog(this, tr("Open File"));

                InitializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

                if (dialog.exec() != QDialog::Accepted)
                    return tr("");
                return dialog.selectedFiles().constFirst();
            })
            .filter([](const QString& path) { return !path.isEmpty(); })
            .map([](const QString&    path)
            {
                QImageReader reader(path);
                reader.setAutoTransform(true);
                return reader.read();
            })
            .filter([](const QImage& img)
            {
                return !img.isNull();
            })
            .subscribe([&](const QImage& img)
            {
                m_pixmap_before = QPixmap::fromImage(img);

                updateImages();
            });

    rxqt::from_event(m_ui->Center, QEvent::Resize).subscribe([&](const auto&)
    {
        updateImages();
    });
}

MainWindow::~MainWindow()
{
    m_ui.reset();
}

rxcpp::observable<bool> MainWindow::GetOnClickObservable() const
{
    return rxcpp::observable<>::empty<bool>();
}

rxcpp::observer<std::string> MainWindow::GetOnTextObserver() const
{
    return rxcpp::make_observer_dynamic<std::string>([](const auto&) {});
}

void MainWindow::updateImages() const
{
    UpdateImage(*m_ui->image_before, m_pixmap_before);
    UpdateImage(*m_ui->image_after, m_pixmap_before);
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);

    const auto min_width = static_cast<int>(width() * 0.15);
    const auto max_width = static_cast<int>(width() * 0.3);

    for (auto* menu : {m_ui->LeftMenu})
    {
        menu->setMinimumWidth(min_width);
        menu->setMaximumWidth(max_width);
    }

    //updateImages();
}
