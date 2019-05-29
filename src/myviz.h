#ifndef MYVIZ_H
#define MYVIZ_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QSlider> 
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QLineEdit>
#include <unistd.h>
#include <QPixmap>  
#include <QMessageBox> 
#include <QEventLoop>
#include <QTimer>
#include <string>
#include <iostream> 
#include <stdio.h> 
#include <QProgressDialog> 
#include <QProgressBar>
#include <QFileDialog>

using namespace std;


class MyViz: public QWidget
{
Q_OBJECT

public:
  MyViz( QWidget* parent = 0 );
  virtual ~MyViz();

public Q_SLOTS:
  void onButtonInClicked(void);
  void onButtonOutClicked(void);
  void onButtonStClicked(void);


public: 
 
  QPushButton* push_out_button;

  QLineEdit* path_in_edit;
  QLineEdit* path_out_edit;

  QPushButton* push_input_button; 
  QPushButton* push_output_button; 

  QPushButton* push_start_button; 

  QProgressBar *progressBar;

  QGridLayout* controls_layout;
  QVBoxLayout* main_layout;

  string       Path4In ; 
  string       Path4Out; 

  QEventLoop eventloop;

public:
  int width;
  int height;
  bool start;
public:
  //window size
  QSize sizeHint() const;
  void showprogress(int value_process);
 /* void setcellchange(int cell_change);
  //grid state
  void setgpstate(bool);
  void setGPRMCstate(bool);
  void setPPSstate(bool);
  //void setSpeedstate(void);
  //show times
  void setTimestate(void);
  //show speed
  void showSpeed(int speed);
  //state bar
  void showState(int stat);
  //show imu cnt
  void showImucnt(int imucnt);
  //show gnss num
  void show_GNSS_num(int gnss_num);*/
  
};
// END_TUTORIAL
#endif // MYVIZ_H
