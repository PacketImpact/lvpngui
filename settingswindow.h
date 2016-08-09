#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QWidget>
#include <QSettings>

class VPNGUI;

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent, const VPNGUI &vpngui, QSettings &appSettings);
    ~SettingsWindow();

    void loadSettings();
    void saveSettings();

public slots:
    void saveAndClose();

private:
    Ui::SettingsWindow *ui;
    const VPNGUI &m_vpngui;
    QSettings &m_appSettings;
};

#endif // SETTINGSWINDOW_H
