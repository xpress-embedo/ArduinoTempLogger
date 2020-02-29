#include <QApplication>
#include <QMainWindow>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QTime>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDebug>

QT_CHARTS_USE_NAMESPACE

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  // Open File
  QFile file("D:\\Projects\\Embedded\\ArduinoTempLogger\\01-21-18.txt");

  if( !file.open(QIODevice::ReadOnly|QIODevice::Text) )
  {
    qDebug() << "File don't exist";
    return  1;
  }
  QTextStream stream(&file);
  QLineSeries *adc_series = new QLineSeries();
  QLineSeries *temp_series = new QLineSeries();
  QDateTime datetime = QDateTime::currentDateTime();
  while( !stream.atEnd() )
  {
    QString line = stream.readLine();
    QStringList values = line.split(",");
    QTime time;
    time = QTime::fromString(values[0], "hh:mm:ss");
    datetime.setTime(time);
    adc_series->append( datetime.toMSecsSinceEpoch(), values[1].toUInt() );
    temp_series->append( datetime.toMSecsSinceEpoch(), values[2].toDouble() );
    // qDebug() << time.toString("hh:mm:ss") << "-->" << datetime.toMSecsSinceEpoch();
  }
  file.close();

  QChart *chart = new QChart();
  chart->legend()->hide();
  chart->addSeries(adc_series);
  chart->addSeries(temp_series);
  // chart->createDefaultAxes();
  chart->setTitle("Temperature Plot");

  // Since we use QLineSeries, calling createDefaultAxes will create QValueAxis both as X and Y axis.
  // To use QDateTimeAxis we need to set it manually to the chart.
  // First, the instance of QDateTimeAxis is created, then the number of ticks to be shown is set.
  //
  QDateTimeAxis *axisX = new QDateTimeAxis;
  axisX->setTickCount(10);
  axisX->setFormat("hh:mm:ss");
  axisX->setTitleText("Time Axis");
  chart->addAxis(axisX, Qt::AlignBottom);
  adc_series->attachAxis(axisX);
  temp_series->attachAxis(axisX);

  QValueAxis *axisY = new QValueAxis;
  axisY->setLabelFormat("%i");
  axisY->setTitleText("Temperature and ADC Value");
  axisY->setRange(0, 100);
  chart->addAxis(axisY, Qt::AlignLeft);
  adc_series->attachAxis(axisY);
  temp_series->attachAxis(axisY);

  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);

  QMainWindow window;
  window.setCentralWidget(chartView);
  window.resize(820, 600);
  window.show();

  return a.exec();
}
