#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

class ClientMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit ClientMainWindow(QWidget *parent = 0);
	
signals:
	
public slots:
private:
	QPushButton stopPB;
};

#endif // CLIENTMAINWINDOW_H