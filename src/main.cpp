#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QSurfaceFormat>

int
main (int argc, char *argv[])
{
  QApplication app (argc, argv);

  // Set OpenGL format requirements for Deferred Rendering (G-Buffer needs
  // precision)
  QSurfaceFormat format;
  format.setVersion (4, 5); // Target recent GL version for convenience
  format.setProfile (QSurfaceFormat::CoreProfile);
  format.setDepthBufferSize (24);
  format.setStencilBufferSize (8);
  format.setSamples (4); // MSAA (will likely be disabled for G-buffer pass,
                         // enabled for forward/composition)
  QSurfaceFormat::setDefaultFormat (format);

  // Set dark theme.
  QFile f (":qdarkstyle/dark/darkstyle.qss");

  if (!f.exists ())
    {
      qDebug () << "Unable to set stylesheet, file not found!\n";
    }
  else if (f.open (QFile::ReadOnly | QFile::Text))
    {
      QTextStream ts (&f);
      qApp->setStyleSheet (ts.readAll ());
    }

  MainWindow window;
  window.show ();

  return app.exec ();
}
