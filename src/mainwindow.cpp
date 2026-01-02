#include "mainwindow.h"
#include "glviewwidget.h"

#include <QCheckBox>
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
  // Placeholder for Priority 3
  QMessageBox::information (
      this, "Info", "GLTF Loading will be implemented in the next step.");
}
