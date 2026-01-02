#ifndef GLTFLOADER_H
#define GLTFLOADER_H

#include "meshdata.h"
#include <QObject>
#include <QString>

class GLTFLoader : public QObject
{
  Q_OBJECT
public:
  explicit GLTFLoader (QObject *parent = nullptr) : QObject (parent) {}

public slots:
  void process (QString filepath);

signals:
  void
  finished (SceneData *data); // Passing raw pointer to be managed by receiver
  void error (QString msg);
};

#endif // GLTFLOADER_H
