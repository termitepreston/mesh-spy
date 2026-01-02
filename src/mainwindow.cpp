#include "mainwindow.h"
#include "gltfloader.h" // Include the loader
#include "glviewwidget.h"

#include <QCheckBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>

MainWindow::MainWindow (QWidget *parent)
    : QMainWindow (parent), m_glView (new GLViewWidget (this))
{
  setupUi ();
  resize (1280, 720);
  setWindowTitle ("meshSpy - PBR Viewer");
}

MainWindow::~MainWindow () {}

void
MainWindow::setupUi ()
{
  QWidget *centralWidget = new QWidget (this);
  setCentralWidget (centralWidget);

  // Main horizontal layout (Side Panel | GL View)
  QHBoxLayout *mainLayout = new QHBoxLayout (centralWidget);
  mainLayout->setContentsMargins (0, 0, 0, 0);
  mainLayout->setSpacing (0);

  // --- Side Panel ---
  QWidget *sidePanel = new QWidget (this);
  sidePanel->setFixedWidth (250);
  sidePanel->setStyleSheet ("background-color: #2b2b2b; color: #ffffff;");

  QVBoxLayout *sideLayout = new QVBoxLayout (sidePanel);
  sideLayout->setContentsMargins (10, 10, 10, 10);
  sideLayout->setSpacing (15);

  // Load Section
  m_btnLoad = new QPushButton ("Load Model", this);
  m_btnLoad->setFixedHeight (40);
  connect (m_btnLoad, &QPushButton::clicked, this,
           &MainWindow::onLoadModelClicked);
  sideLayout->addWidget (m_btnLoad);

  // Materials Section
  QGroupBox *matGroup = new QGroupBox ("Materials", this);
  matGroup->setStyleSheet ("QGroupBox { border: 1px solid #444; margin-top: "
                           "10px; padding-top: 10px; font-weight: bold; }");
  QVBoxLayout *matLayout = new QVBoxLayout (matGroup);

  m_chkBaseColor = new QCheckBox ("Base Color", this);
  m_chkMetal = new QCheckBox ("Metallic", this);
  m_chkRough = new QCheckBox ("Roughness", this);
  m_chkNormal = new QCheckBox ("Normal", this);
  m_chkWireframe = new QCheckBox ("Wireframe", this);

  // Defaults per requirement
  m_chkBaseColor->setChecked (true);
  m_chkMetal->setChecked (true);
  m_chkRough->setChecked (true);
  m_chkNormal->setChecked (true);
  m_chkWireframe->setChecked (false);

  matLayout->addWidget (m_chkBaseColor);
  matLayout->addWidget (m_chkMetal);
  matLayout->addWidget (m_chkRough);
  matLayout->addWidget (m_chkNormal);
  matLayout->addWidget (m_chkWireframe);

  sideLayout->addWidget (matGroup);
  sideLayout->addStretch (); // Push everything up

  // --- GL Viewport ---
  // Added directly to main layout

  // Add splitter for resizability (optional, but good UX)
  QSplitter *splitter = new QSplitter (Qt::Horizontal, this);
  splitter->addWidget (sidePanel);
  splitter->addWidget (m_glView);
  splitter->setCollapsible (0, false); // Side panel always visible
  splitter->setStretchFactor (1, 1);   // GL View takes remaining space

  mainLayout->addWidget (splitter);

  // --- Status Bar ---
  m_statusLabel = new QLabel ("Ready", this);
  statusBar ()->addWidget (m_statusLabel);
}

void
MainWindow::onLoadModelClicked ()
{
  QString fileName = QFileDialog::getOpenFileName (
      this, "Open GLTF/GLB", "", "GLTF Files (*.gltf *.glb)");
  if (fileName.isEmpty ())
    return;

  // UI Feedback
  m_btnLoad->setEnabled (false);
  m_statusLabel->setText ("Loading " + fileName + "...");
  // Note: Spinner animation would require a QMovie or standard icon flip here,
  // keeping it simple text for now per Phase 3 scope.

  // Threading Setup
  m_loaderThread = new QThread;
  GLTFLoader *worker = new GLTFLoader;
  worker->moveToThread (m_loaderThread);

  connect (m_loaderThread, &QThread::started, worker,
           [worker, fileName] () { worker->process (fileName); });

  // Connect completion signals
  connect (worker, &GLTFLoader::finished, this, &MainWindow::onModelLoaded);
  connect (worker, &GLTFLoader::error, this, &MainWindow::onModelLoadError);

  // Cleanup
  connect (worker, &GLTFLoader::finished, m_loaderThread, &QThread::quit);
  connect (worker, &GLTFLoader::finished, worker, &QObject::deleteLater);
  connect (worker, &GLTFLoader::error, m_loaderThread, &QThread::quit);
  connect (worker, &GLTFLoader::error, worker, &QObject::deleteLater);
  connect (m_loaderThread, &QThread::finished, m_loaderThread,
           &QObject::deleteLater);

  m_loaderThread->start ();
}

void
MainWindow::onModelLoaded (SceneData *data)
{
  m_btnLoad->setEnabled (true);
  m_statusLabel->setText ("Loaded successfully.");

  // Pass to GLView (requires exposing the renderer or adding a method to
  // GLView)
  m_glView->loadModel (data); // Needs to be added to GLViewWidget

  // Note: SceneData* ownership is transferred to GLView/Renderer
}

void
MainWindow::onModelLoadError (QString error)
{
  m_btnLoad->setEnabled (true);
  m_statusLabel->setText ("Error loading model.");
  QMessageBox::critical (this, "Error", error);
}
