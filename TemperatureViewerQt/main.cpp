#include "temperatureviewer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  TemperatureViewer w;

  w.show();
  return a.exec();
}
