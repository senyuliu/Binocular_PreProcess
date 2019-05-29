#include "myviz.h"

//global controls
extern bool close_flag;
extern int  save_control;
extern float cam_gain;
extern float cam_shulter;

// BEGIN_TUTORIAL
// Constructor for MyViz.  This does most of the work of the class.
MyViz::MyViz( QWidget* parent )
  : QWidget( parent )
{
//Q_OBJECT
  //normal parameters
  setWindowFlags(Qt::CustomizeWindowHint|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint|Qt::WindowMaximizeButtonHint|Qt::WindowStaysOnTopHint);
  //width  = 300;
  //height = 100;
  this->sizeHint();

  int rows = 0; 
  int cols = 0;

  start    = false; 

  //[00]EDIT1 FOR PATHIN 
  path_in_edit      = new QLineEdit( this);
  path_in_edit->setReadOnly(true);
  path_in_edit->setFocusPolicy(Qt::NoFocus); 
  //[01]BUTTON1 FOR PATHIN 
  push_input_button = new QPushButton("PIN...",this);
  
  //[10]EDIT2 FOR PATHOUT 
  path_out_edit      = new QLineEdit( this);
  path_out_edit->setReadOnly(true);
  path_out_edit->setFocusPolicy(Qt::NoFocus);

  //[11]BUTTON2 FOR PATHOUT  
  push_output_button = new QPushButton("POUT...",this);

  //[20]DISPLAY FOR PROCEDURE 
  progressBar = new QProgressBar(this); 
  
  //[21]BUTTON3 FOR PROCEDURE
  push_start_button = new QPushButton("Start...",this);

  //[O]QGridLayout
  controls_layout = new QGridLayout();
  
  controls_layout->setMargin(10);
  controls_layout->setSpacing(3);

  controls_layout->addWidget( path_in_edit, rows, 0 );
  controls_layout->addWidget( push_input_button, rows, 1 );

  controls_layout->addWidget( path_out_edit, ++rows, 0);
  controls_layout->addWidget( push_output_button, rows, 1 );

  controls_layout->addWidget( progressBar, ++rows , 0);
  controls_layout->addWidget( push_start_button, rows, 1 );

  

  //[p]QMainLayout
  main_layout = new QVBoxLayout;

  main_layout->addLayout( controls_layout );

  setLayout( main_layout );


  connect( push_input_button,SIGNAL( clicked()), this, SLOT(onButtonInClicked(void)));
  connect( push_output_button,SIGNAL( clicked()), this, SLOT(onButtonOutClicked(void)));
  connect( push_start_button,SIGNAL( clicked()), this, SLOT(onButtonStClicked(void)));

}

// Destructor.
MyViz::~MyViz()
{

}



QSize MyViz::sizeHint() const
 {
     return QSize(500, 150 );
 }
void MyViz::onButtonInClicked(void)
{
    std::cout<<"onButtonInClicked : onButtonInClicked come in "<< std::endl; 

    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Path In ...");

    fileDialog->setDirectory(".");
    fileDialog->setFileMode(QFileDialog::AnyFile);

    QString file_name = QFileDialog::getExistingDirectory(NULL,"caption",".");

    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    path_in_edit->setPlaceholderText(file_name);

    std::cout<<"onButtonInClicked: file_name: "<< file_name.toStdString() <<std::endl ; 
   
    Path4In  =  file_name.toStdString() ; 


    std::cout<<"onButtonInClicked: onButtonInClicked out "<< std::endl;
     
}
void MyViz::onButtonOutClicked(void)
{
    std::cout<<"onButtonInClicked : onButtonInClicked come in "<< std::endl; 

    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle("Path In ...");

    fileDialog->setDirectory(".");
    fileDialog->setFileMode(QFileDialog::AnyFile);

    QString file_name = QFileDialog::getExistingDirectory(NULL,"caption",".");

    fileDialog->setAcceptMode(QFileDialog::AcceptOpen);

    path_out_edit->setPlaceholderText(file_name);

    std::cout<<"onButtonInClicked: file_name: "<< file_name.toStdString() <<std::endl ; 

    Path4Out = file_name.toStdString(); 


    std::cout<<"onButtonInClicked: onButtonInClicked out "<< std::endl;
}
void MyViz::showprogress(int value_process)
{
    progressBar->setRange(0,100); //设置进度条最小值和最大值(取值范围)

    progressBar->setMinimum(0); //设置进度条最小值

    progressBar->setMaximum(100); //设置进度条最大值

    progressBar->setValue(value_process);  //设置当前的运行值
}

void MyViz::onButtonStClicked(void)
{

    std::cout<<"onButtonStClicked : onButtonStClicked come in "<< std::endl;

    progressBar->setRange(0,100); //设置进度条最小值和最大值(取值范围)

    progressBar->setMinimum(0); //设置进度条最小值

    progressBar->setMaximum(100); //设置进度条最大值

    //for (int i = 0 ; i < 105; i = i + 5)
    //{
    //    progressBar->setValue(i);  //设置当前的运行值
    //}

    
    progressBar->reset(); //让进度条重新回到开始

    progressBar->setOrientation(Qt::Horizontal);  //水平方向

    start  = true; 

    //progressBar->setAlignment(Qt::AlignVCenter);  // 对齐方式 

    //progressBar->setTextVisible(false); //隐藏进度条文本

    //progressBar->setFixedSize(258,5);   //进度条固定大小

    //progressBar->setInvertedAppearance(true); //true:反方向  false:正方向

    //progressBar->setVisible(false);  //false:隐藏进度条  true:显示进度条

    
    std::cout<<"onButtonStClicked : onButtonStClicked come out "<< std::endl;

}
