#include "clientmainwindow.h"

ClientMainWindow::ClientMainWindow(QWidget *parent) :
	QMainWindow(parent),
	stopPB(tr("Отключить удаленное подключение"), this),
	mMainLayout(0),
	mInfoLabel(0),
	mMainWidget(0)
{
	mMainWidget = new QWidget(this);
	mMainLayout = new QVBoxLayout();
	mInfoLabel = new QLabel(this);
	mMainLayout->addWidget(mInfoLabel);
	mMainLayout->addWidget(&stopPB);
	mMainWidget->setLayout(mMainLayout);
	connect(&stopPB, SIGNAL(clicked()), this, SLOT(close()));
	setCentralWidget(mMainWidget);
	setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
	//setFixedSize(sizeHint());
}

ClientMainWindow::~ClientMainWindow()
{
	delete mMainLayout;
}

void ClientMainWindow::info(quint8 type, const QString &data)
{
	mInfoLabel->setText(QString("<h2>%1</h2>").arg(data));
}

