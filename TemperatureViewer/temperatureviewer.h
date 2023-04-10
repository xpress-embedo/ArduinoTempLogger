
#ifndef TEMPERATUREVIEWER_H
#define TEMPERATUREVIEWER_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>


QT_BEGIN_NAMESPACE
namespace Ui { class TemperatureViewer; }
QT_END_NAMESPACE

class TemperatureViewer : public QMainWindow

{
  Q_OBJECT

public:
  TemperatureViewer(QWidget *parent = nullptr);
  ~TemperatureViewer();

private slots:
  void on_btn_ConnectDisconnect_clicked();

public slots:
  void read_data( void );

private:
  Ui::TemperatureViewer *ui;
  bool connect_status = false;
  QSerialPort m_serial;
  void refreshGraph( void );
};

#endif // TEMPERATUREVIEWER_H
