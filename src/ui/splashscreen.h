#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>
#include <QEvent>

class SplashScreen : public QSplashScreen
{
  Q_OBJECT
public:
  explicit SplashScreen(QWidget *parent = 0);

  bool eventFilter(QObject *target, QEvent *event);
signals:

public slots:

};

#endif // SPLASHSCREEN_H
