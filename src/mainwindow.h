#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "meshdata.h"
#include "renderconfig.h"

#include <QMainWindow>
#include <QThread>

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
  void onModelLoaded (SceneData *data);
  void onModelLoadError (QString error);

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

  QThread *m_loaderThread;

private:
  void updateRenderConfig ();
};

#endif // MAINWINDOW_H
