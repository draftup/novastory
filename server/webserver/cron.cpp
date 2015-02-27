#include "cron.h"
#include "webserver/globals.h"
#include <QDebug>
#include <QEventLoop>
#include <QMetaObject>
#include <QCoreApplication>

namespace novastory
{

Cron::Cron() : QThread()
{
	setObjectName("Cron Thread");
	start();
	moveToThread(this);
	qDebug() << "Cron thread started";
}

Cron::~Cron()
{
	quit();
	wait();
	qDebug() << "Cron thread destroyed";
}


void Cron::run()
{
	exec();
}

void Cron::newTask(void(*func)(void), int interval /* = 1000 */)
{
	qDebug() << "New cron task added to query with interaval" << interval;
	QTimer* timer = new QTimer();
	timer->moveToThread(&Instance());
	timer->setInterval(interval);
	VERIFY(connect(timer, &QTimer::timeout, func));
	Instance().m_tasks.append(QSharedPointer<QTimer>(timer));
	QMetaObject::invokeMethod(&Instance(), "addTask", Qt::QueuedConnection, Q_ARG(void*, (void*)timer));
}

void Cron::addTask(void* timer)
{
	((QTimer*)timer)->start();
	qDebug() << "New cron task started";
}

}
