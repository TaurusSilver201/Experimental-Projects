#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QThread>
#include <QMutex>
#include <QVector>
#include <QStringList>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QFileDialog>
#include <QNetworkProxy>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

namespace {
QVector<QString> parseArguments(int argc, char* argv[]) {
    QVector<QString> arguments;
    for (int i = 1; i < argc; ++i) {
        arguments.push_back(QString::fromUtf8(argv[i]));
    }
    return arguments;
}

bool allSuccessful = true;

void logError(const QString& message) {
    std::cerr << message.toStdString() << std::endl;
    std::ofstream logFile("error.log", std::ios::app);
    logFile << message.toStdString() << std::endl;
    logFile.close();
    allSuccessful = false;
}
}  // namespace

class WebsiteOpener : public QThread {
    Q_OBJECT

public:
    WebsiteOpener(const QVector<QString>& urls, QMutex& mutex, QProgressBar* progressBar, QNetworkProxy* proxy, QObject* parent = nullptr)
        : QThread(parent), m_urls(urls), m_mutex(mutex), m_progressBar(progressBar), m_proxy(proxy) {}

protected:
    void run() override {
        int urlCount = m_urls.size();
        int openedCount = 0;
        for (const QString& url : m_urls) {
            configureProxy();
            bool success = QDesktopServices::openUrl(QUrl(url));
            m_mutex.lock();
            if (!success) {
                logError("Failed to open: " + url);
            } else {
                openedCount++;
            }
            m_progressBar->setValue(static_cast<int>((openedCount * 100.0) / urlCount));
            m_mutex.unlock();
        }
    }

private:
    void configureProxy() {
        QNetworkProxyFactory::setUseSystemConfiguration(true);
        if (m_proxy) {
            QNetworkProxyFactory::setApplicationProxy(*m_proxy);
        }
    }

    QVector<QString> m_urls;
    QMutex& m_mutex;
    QProgressBar* m_progressBar;
    QNetworkProxy* m_proxy;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr)
        : QMainWindow(parent), m_urlInput(new QLineEdit(this)), m_progressBar(new QProgressBar(this)), m_proxyInput(new QLineEdit(this)) {
        setupUI();
    }

private:
    void setupUI() {
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(centralWidget);

        layout->addWidget(new QLabel("Enter URLs (separated by spaces):"));
        layout->addWidget(m_urlInput);

        QPushButton* openButton = new QPushButton("Open URLs", centralWidget);
        connect(openButton, &QPushButton::clicked, this, &MainWindow::openURLs);
        layout->addWidget(openButton);

        QPushButton* importButton = new QPushButton("Import URLs from File", centralWidget);
        connect(importButton, &QPushButton::clicked, this, &MainWindow::importURLs);
        layout->addWidget(importButton);

        layout->addWidget(new QLabel("Proxy (optional):"));
        layout->addWidget(m_proxyInput);

        layout->addWidget(m_progressBar);

        setCentralWidget(centralWidget);
        setWindowTitle("Website Opener");
    }

private slots:
    void openURLs() {
        QStringList urlStrings = m_urlInput->text().split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        QVector<QString> urls = urlStrings.toVector();

        if (!urls.isEmpty()) {
            QNetworkProxy proxy = parseProxy();

            resetProgress();

            QMutex mutex;
            QVector<WebsiteOpener*> threads = createThreads(urls, mutex, proxy);

            waitForThreads(threads);
            displayResult();
        }
    }

    void importURLs() {
        QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");
        if (!fileName.isEmpty()) {
            std::ifstream file(fileName.toStdString());
            if (file.is_open()) {
                std::string line;
                QStringList urls;
                while (std::getline(file, line)) {
                    urls.append(QString::fromStdString(line));
                }
                file.close();
                m_urlInput->setText(urls.join(" "));
            } else {
                logError("Failed to open file: " + fileName);
            }
        }
    }

private:
    QNetworkProxy parseProxy() const {
        QNetworkProxy proxy;
        if (!m_proxyInput->text().isEmpty()) {
            proxy = QNetworkProxy::fromUser(m_proxyInput->text());
        }
        return proxy;
    }

    void resetProgress() {
        m_progressBar->setValue(0);
        m_progressBar->setMaximum(100);
    }

    QVector<WebsiteOpener*> createThreads(const QVector<QString>& urls, QMutex& mutex, const QNetworkProxy& proxy) {
        QVector<WebsiteOpener*> threads;
        const int maxThreads = QThread::idealThreadCount();
        int currentThread = 0;

        for (const QString& url : urls) {
            if (currentThread == 0 || threads[currentThread]->m_urls.size() >= maxThreads) {
                WebsiteOpener* opener = new WebsiteOpener({url}, mutex, m_progressBar, proxy.isNull() ? nullptr : &proxy);
                threads.push_back(opener);
                opener->start();
            } else {
                threads[currentThread]->m_urls.push_back(url);
            }
            currentThread = (currentThread + 1) % maxThreads;
        }

        return threads;
    }

    void waitForThreads(const QVector<WebsiteOpener*>& threads) {
        for (WebsiteOpener* thread : threads) {
            thread->wait();
        }
    }

    void displayResult() {
        if (allSuccessful) {
            std::cout << "All websites opened successfully." << std::endl;
        } else {
            std::cerr << "One or more websites failed to open. Check error.log for details." << std::endl;
        }
    }

    QLineEdit* m_urlInput;
    QProgressBar* m_progressBar;
    QLineEdit* m_proxyInput;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    QVector<QString> arguments = parseArguments(argc, argv);
    QVector<QString> defaultSites = {
        "https://store.steampowered.com/",
        "https://www.epicgames.com/store/",
        "https://www.origin.com/",
        "https://www.ubisoft.com/en-us/game-launcher.html",
        "https://www.blizzard.com/apps",
        "https://www.gog.com/",
        "https://www.humblebundle.com/"
    };

    bool useProxy = false;
    QString proxyString;
    QNetworkProxy proxy;

    for (int i = 1; i < argc; ++i) {
        QString arg = QString::fromUtf8(argv[i]);
        if (arg == "--proxy") {
            useProxy = true;
            if (i + 1 < argc) {
                proxyString = QString::fromUtf8(argv[++i]);
                proxy = QNetworkProxy::fromUser(proxyString);
            } else {
                logError("Error: --proxy option requires a proxy address.");
                return 1;
            }
        } else if (arg == "--urls") {
            if (i + 1 < argc) {
                QString urlsFile = QString::fromUtf8(argv[++i]);
                std::ifstream file(urlsFile.toStdString());
                if (file.is_open()) {
                    std::string line;
                    while (std::getline(file, line)) {
                        defaultSites.push_back(QString::fromStdString(line));
                    }
                    file.close();
                } else {
                    logError("Failed to open URL file: " + urlsFile);
                    return 1;
                }
            } else {
                logError("Error: --urls option requires a file path.");
                return 1;
            }
        }
    }

    if (!arguments.isEmpty()) {
        MainWindow window;
        window.show();
        return app.exec();
    } else {
        QMutex mutex;
        QProgressBar progressBar;
        progressBar.setMaximum(100);

        QVector<WebsiteOpener*> threads = createThreads(defaultSites, mutex, useProxy ? proxy : QNetworkProxy());
        progressBar.show();
        waitForThreads(threads);

        if (allSuccessful) {
            std::cout << "All websites opened successfully." << std::endl;
            return 0;
        } else {
            std::cerr << "One or more websites failed to open. Check error.log for details." << std::endl;
            return 1;
        }
    }
}
