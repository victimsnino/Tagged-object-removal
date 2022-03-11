#include "MainWindow.h"
#include "./ui_MainWindow.h"

#include <rxqt.hpp>

#include <QFileDialog>
#include <QCheckBox>
#include <QImageReader>
#include <QImageWriter>
#include <QMessageBox>
#include <QScrollArea>

static void InitializeImageFileDialog(QFileDialog& dialog, QFileDialog::AcceptMode acceptMode)
{
    //dialog.setDirectory(QDir::currentPath());

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
    m_ui->save_result->setEnabled(false);
    m_ui->process_image->setEnabled(false);
    m_ui->save_result->setToolTip("Save processed image to disk. Button activated after processed image is ready");
    m_ui->process_image->setToolTip("Process loaded image with selected above tags. Button activated after loading of image");

    m_on_image_observable = rxqt::from_signal(m_ui->load_image, &QPushButton::clicked)
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
                               return reader.read().scaled(1280, 720, Qt::KeepAspectRatio).copy();
                           })
                           .filter([](const QImage& img)
                           {
                               return !img.isNull();
                           })
                           .publish().ref_count();

    m_on_image_observable.subscribe([&](const QImage& img)
    {
        m_ui->process_image->setEnabled(true);
        m_ui->save_result->setEnabled(false);

        m_pixmap_before = QPixmap::fromImage(img);
        m_pixmap_after  = m_pixmap_before;

        updateImages();
    });

    m_on_process_tags = rxqt::from_signal(m_ui->process_image, &QPushButton::clicked)
                        .tap([&](const auto&)
                        {
                            m_ui->image_after->setText("LOADING...");
                        })
                        .map([&](const auto&)
                        {
                            std::vector<std::string> enabled_tags{};
                            for (const auto& c : m_ui->tags_scroll_content->findChildren<QCheckBox*>(QString(),
                                                                                                     Qt::FindDirectChildrenOnly))
                            {
                                if (c->isChecked())
                                    enabled_tags.push_back(c->text().toStdString());
                            }
                            return enabled_tags;
                        });

    rxqt::from_event(m_ui->Center, QEvent::Resize).subscribe([&](const auto&)
    {
        updateImages();
    });

    rxqt::from_signal(m_ui->save_result, &QPushButton::clicked)
            .map([&](const auto&)
            {
                QFileDialog dialog(this, tr("Save As File"));

                InitializeImageFileDialog(dialog, QFileDialog::AcceptSave);

                if (dialog.exec() != QDialog::Accepted)
                    return tr("");
                return dialog.selectedFiles().constFirst();
            })
            .filter([](const QString& path) { return !path.isEmpty(); })
            .tap([&](const QString&   path)
            {
                QImageWriter writer{path};
                writer.write(m_pixmap_after.toImage());
            })
            .subscribe([&](const QString& path)
            {
                QMessageBox::information(this,
                                         QGuiApplication::applicationDisplayName(),
                                         tr("Image successfully saved to %1")
                                         .arg(QDir::toNativeSeparators(path)));
            });
}

MainWindow::~MainWindow()
{
    m_ui.reset();
}

rxcpp::observable<QImage> MainWindow::GetOnImageObservable() const
{
    return m_on_image_observable;
}

rxcpp::observable<std::vector<std::string>> MainWindow::GetOnProcessTagsObservable() const
{
    return m_on_process_tags;
}

rxcpp::observer<QImage> MainWindow::GetOnProcessedImageObserver()
{
    return rxcpp::make_observer_dynamic<QImage>([&](const QImage& img)
    {
        m_ui->save_result->setEnabled(true);

        m_pixmap_after = QPixmap::fromImage(img);

        updateImages();
    });
}

rxcpp::observer<std::vector<std::string>> MainWindow::GetOnTagsListObserver()
{
    return rxcpp::make_observer_dynamic<std::vector<std::string>>([&](const std::vector<std::string>& tags)
    {
        QVBoxLayout* vbox = new QVBoxLayout();

        for (const auto& tag : tags)
            vbox->addWidget(new QCheckBox(QString::fromStdString(tag)));

        vbox->addStretch(1);
        m_ui->tags_scroll_content->setLayout(vbox);
    });
}

void MainWindow::updateImages() const
{
    UpdateImage(*m_ui->image_before, m_pixmap_before);
    UpdateImage(*m_ui->image_after, m_pixmap_after);
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
