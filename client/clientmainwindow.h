#ifndef CLIENTMAINWINDOW_H
#define CLIENTMAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

class ClientMainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit ClientMainWindow(QWidget *parent = 0);
	~ClientMainWindow();
signals:
	
public slots:
	void info(quint8 type, const QString &data);
private:
	QPushButton stopPB;
	QWidget *mMainWidget;
	QVBoxLayout *mMainLayout;
	QLabel *mInfoLabel;
};

#endif // CLIENTMAINWINDOW_H
