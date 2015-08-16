#include "common.h"
#include <QDebug>

static quint8 gLogLvl = 0;

namespace NR {

void SetLogLvl(quint8 logLvl)
{
	gLogLvl = logLvl;
}

void Log(const QString &msg, quint8 logLvl)
{
	if (logLvl <= gLogLvl)
		qDebug()<<msg;
}

}

