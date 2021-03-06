#ifndef RESULTHISTORY_H
#define RESULTHISTORY_H

/* =========================================================================
   Copyright (c) 2014-2015, Institute for Microelectronics,
                            Institute for Analysis and Scientific Computing,
                            TU Wien.

                            -----------------
                ViennaCLBench - A Free Open-Source Benchmark
                            -----------------

   License:         MIT (X11), see file LICENSE in the base directory
============================================================================= */

#include <QWidget>
#include <QAbstractItemModel>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTableWidgetItem>
#if (QT_VERSION > QT_VERSION_CHECK(5, 0, 0))
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#else
#endif

namespace Ui {
  class ResultHistory;
}

/*! \class ResultHistory
 * \brief Handles loading and displaying the result history.
 */
class ResultHistory : public QWidget
{
  Q_OBJECT

public:
  explicit ResultHistory(QWidget *parent = 0);
  ~ResultHistory();

#if (QT_VERSION > QT_VERSION_CHECK(5, 0, 0))
  void readResultFile(const QJsonObject &jsonRoot);
#else
#endif

public slots:
  void loadHistory();
private:
  Ui::ResultHistory *ui;
};

#endif // RESULTHISTORY_H
