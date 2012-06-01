/**
 * \file pqFbxStarter.h
 * 2012-06-01 LB Initial implementation
 */

#ifndef PQFBXSTARTER_H
#define PQFBXSTARTER_H

#include <QObject>

/// @brief Inits and exits the Fbx SDK on startup or shutdown of ParaView
class pqFbxStarter : public QObject
{
  Q_OBJECT
  typedef QObject Superclass;

public:
  pqFbxStarter(QObject* parent = 0);
  ~pqFbxStarter();
  
  void onShutdown(); 
  void onStartup();

private:
  Q_DISABLE_COPY(pqFbxStarter);
};

#endif // PQOSGSTARTER_H