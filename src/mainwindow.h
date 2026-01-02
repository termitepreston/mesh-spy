#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class GLViewWidget;
class QPushButton;
class QCheckBox;
class QLabel;

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow (QWidget *parent = nullptr);
  ~MainWindow ();

private slots:
  void onLoadModelClicked ();

private:
  void setupUi ();

  // Core Components
  GLViewWidget *m_glView;

  // UI Elements
  QPushButton *m_btnLoad;
  QCheckBox *m_chkBaseColor;
  QCheckBox *m_chkMetal;
  QCheckBox *m_chkRough;
  QCheckBox *m_chkNormal;
  QCheckBox *m_chkWireframe;
  QLabel *m_statusLabel;
};

#endif // MAINWINDOW_H
