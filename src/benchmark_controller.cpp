#include "benchmark_controller.h"
#include "QDebug"

Benchmark_Controller::Benchmark_Controller(QObject *parent) :
  QObject(parent)
{
}

void Benchmark_Controller::executeSelectedBenchmark(QString benchmarkName)
{
  if(benchmarkName == "Blas3"){
    Benchmark_Blas3 *benchmark = new Benchmark_Blas3();
    connect(benchmark, SIGNAL(resultSignal(QString,double)), this, SLOT(resultSignalSlot(QString,double)) );
    connect(benchmark, SIGNAL(benchmarkComplete()), this, SLOT(benchmarkCompleteSlot()) );
        benchmark->execute(); //execution is too slow
    delete benchmark;

  }
  else if(benchmarkName == "Copy"){
    Benchmark_Copy *benchmark = new Benchmark_Copy();
    connect(benchmark, SIGNAL(resultSignal(QString,double)), this, SLOT(resultSignalSlot(QString,double)) );
    connect(benchmark, SIGNAL(benchmarkComplete()), this, SLOT(benchmarkCompleteSlot()) );
    benchmark->execute();
    delete benchmark;

  }
  else if(benchmarkName == "Scheduler"){
    Benchmark_Scheduler *benchmark = new Benchmark_Scheduler();
    connect(benchmark, SIGNAL(resultSignal(QString,double)), this, SLOT(resultSignalSlot(QString,double)) );
    connect(benchmark, SIGNAL(benchmarkComplete()), this, SLOT(benchmarkCompleteSlot()) );
    benchmark->execute();
    delete benchmark;

  }
  else if(benchmarkName == "Solver"){
    Benchmark_Solver *benchmark = new Benchmark_Solver();
    connect(benchmark, SIGNAL(resultSignal(QString,double)), this, SLOT(resultSignalSlot(QString,double)) );
    connect(benchmark, SIGNAL(benchmarkComplete()), this, SLOT(benchmarkCompleteSlot()) );
    benchmark->execute();
    delete benchmark;

  }
  else if(benchmarkName == "Sparse"){
    Benchmark_Sparse *benchmark = new Benchmark_Sparse();
    connect(benchmark, SIGNAL(resultSignal(QString,double)), this, SLOT(resultSignalSlot(QString,double)) );
    connect(benchmark, SIGNAL(benchmarkComplete()), this, SLOT(benchmarkCompleteSlot()) );
    benchmark->execute();
    delete benchmark;

  }
  else if(benchmarkName == "Vector"){
    Benchmark_Vector *benchmark = new Benchmark_Vector();
    connect(benchmark, SIGNAL(resultSignal(QString,double)), this, SLOT(resultSignalSlot(QString,double)) );
    connect(benchmark, SIGNAL(benchmarkComplete()), this, SLOT(benchmarkCompleteSlot()) );
    benchmark->execute();
    delete benchmark;

  }
  else if(benchmarkName == "Qr"){
    Benchmark_Qr *benchmark = new Benchmark_Qr();
    connect(benchmark, SIGNAL(resultSignal(QString,double)), this, SLOT(resultSignalSlot(QString,double)) );
    connect(benchmark, SIGNAL(benchmarkComplete()), this, SLOT(benchmarkCompleteSlot()) );
    benchmark->execute();
    delete benchmark;

  }
  else{
    qDebug()<<"cannot resolve benchmark name";

  }
}

void Benchmark_Controller::resultSignalSlot(QString benchmarkName, double bandwidthValue)
{
  qDebug()<<"resultSignalSlot of benchmarkController";
  emit resultSignal(benchmarkName, bandwidthValue);
}

void Benchmark_Controller::benchmarkCompleteSlot()
{
  emit benchmarkComplete();
}