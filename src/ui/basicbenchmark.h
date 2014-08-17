#ifndef BASICBENCHMARK_H
#define BASICBENCHMARK_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QString>
#include <QVector>
#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QPixmap>
#include <QPainter>
#include <QTabWidget>
#include <QDebug>
#include <QThread>
#include <QSpacerItem>
#include <QPlainTextEdit>
#include <QSplitter>
#include <QMap>
#include <QMultiMap>
#include <QPushButton>
#include <QProgressBar>
#include <QComboBox>

#include <cstdlib>
#include <ctime>

#include "qcustomplot.h"
#include "../benchmark_controller.h"
#include "benchmarklistwidget.h"
#include "collapsewidget.h"
#include "matrixmarket_widget.h"

#include "viennacl/ocl/device.hpp"
#include "viennacl/ocl/platform.hpp"

namespace Ui {
  class BasicBenchmark;
}

class BasicBenchmark : public QWidget
{
  Q_OBJECT

public:
  explicit BasicBenchmark(QWidget *parent = 0);
  ~BasicBenchmark();

  void initBasic();
  void plotBarResult(QString benchmarkName, double key, double value, QCustomPlot *customPlot);
  void plotLineResult(QString benchmarkName, double key, double value, QCustomPlot *customPlot, int testId);
  void plotFinalResult(QString benchmarkName, double value, QCustomPlot *customPlot);
  void resetAllPlots();

  QPushButton *startBenchmarkButton;
  QPushButton *stopBenchmarkButton;
  QPushButton  *singlePrecisionButton;
  QProgressBar *progressBar;
  BenchmarkListWidget *benchmarkListWidget;
  QComboBox *contextComboBox;

public slots:
  void hideStopButton();
  void showBenchmarkStartButton();
  void updateSinglePrecisionButtons();
  void updateDoublePrecisionButtons();
  void setActiveBenchmarkPlot(int benchmarkIdNumber);
  void updateFinalResultPlot(QString benchmarkName, double finalResult);
  void graphClicked(QCPAbstractPlottable *plottable);
  void selectionChanged();
  void updateBenchProgress();
  void updateBenchmarkListWidget(QListWidgetItem *item);
  void resetPlotData(QCustomPlot *benchmarkGraph);
  void updateBenchmarkUnitMeasure(QString unitMeasureName, int axis);
  void parseBenchmarkResult(QString benchmarkName, double key, double resultValue, int graphType, int testId);
private:
  int activeBenchmark;
  int currentBenchProgress;
  int maximumBenchProgress;

  QTabWidget *basic_DetailedPlotTab;
  QVector<QCustomPlot*> basic_DetailedPlotsVector;
  QCustomPlot *blas3_DetailedPlot;
  QCustomPlot *copy_DetailedPlot;
  //  QCustomPlot *qr_DetailedPlot;
  //  QCustomPlot *solver_DetailedPlot;
  QCustomPlot *sparse_DetailedPlot;
  QCustomPlot *vector_DetailedPlot;
  Ui::BasicBenchmark *ui;
};

#endif // BASICBENCHMARK_H