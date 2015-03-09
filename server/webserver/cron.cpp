#include "cron.h"
#include "webserver/globals.h"
#include <QDebug>
#include <QEventLoop>
#include <QMetaObject>
#include <QMutexLocker>
#include <sql/sqlquery.h>
#include <functional>
#include <QDateTime>
#include "sql/sqldatabase.h"

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

void Cron::newTask(void(*func)(int, const QString&), int id, const QString& args /* = QString()*/, int interval /* = 1000 */, bool singlesht /*= false*/, qint64 until_time /* = -1 */)
{
	QMutexLocker locker(&Instance().m_taks_mutex);
	qDebug() << "New cron task" << id << "added to query with interaval" << interval << "started" << (singlesht ? "once" : "multiple times");
	QTimer* timer = new QTimer();
	timer->moveToThread(&Instance());
	timer->setInterval(interval);
	timer->setSingleShot(singlesht);
	VERIFY(connect(timer, &QTimer::timeout, std::bind([timer, func, until_time](int taskid, const QString & args)
	{
		func(taskid, args);

		if (timer->isSingleShot())
		{
			stopTask(taskid);
		}

		if (until_time > 0 && QDateTime::currentDateTime().toMSecsSinceEpoch() > until_time)
		{
			stopTask(taskid);
		}
	}, id, args)));
	Instance().m_tasks.insert(id, QSharedPointer<QTimer>(timer));
	QMetaObject::invokeMethod(&Instance(), "addTask", Qt::QueuedConnection, Q_ARG(void*, (void*)timer));
}

void Cron::addTask(void* timer)
{
	((QTimer*)timer)->start();
	qDebug() << "New cron task started";
}

int Cron::startTask(const QString& name, void(*func)(int, const QString&), const QString& args /* = QString()*/, int interval /*= 1000*/, bool singlesht /*= false*/, const QDateTime& endtime /* = QDateTime()*/)
{
	QMutexLocker locker(&Instance().m_func_mutex);
	SqlQuery saveTask;
	saveTask.prepare("INSERT INTO cron(`task`, `args`, `starttime`, `oncetime`, `endtime`) VALUES(?, ?, ?, ?, ?)");
	saveTask.bindValue(0, name);
	saveTask.bindValue(1, args);
	saveTask.bindValue(2, interval);
	saveTask.bindValue(3, singlesht);
	saveTask.bindValue(4, (endtime.isNull() ? -1 : endtime.toMSecsSinceEpoch()));
	VERIFY(saveTask.exec());
	Instance().m_tasks_func.insert(name, func);
	int id = saveTask.lastInsertId().toInt();
	SqlDatabase::close();
	newTask(func, id, args, interval, singlesht, (endtime.isNull() ? -1 : endtime.toMSecsSinceEpoch()));
	return id;
}

int Cron::startTask(const QString& name, void(*func)(int, const QString&), const QString& args /* = QString()*/, const QDateTime& time)
{
	if (time < QDateTime::currentDateTime())
	{
		return -1;
	}

	int time_diff = time.toMSecsSinceEpoch() - QDateTime::currentDateTime().toMSecsSinceEpoch();
	Q_ASSERT(time_diff > 0);

	return startTask(name, func, args, time_diff, true);
}

int Cron::startTask(const QString& name, const QString& args /*= QString()*/, int interval /*= 1000*/, bool singlesht /*= false*/, const QDateTime& endtime /* = QDateTime()*/)
{
	if (Instance().m_tasks_func.contains(name))
	{
		return startTask(name, Instance().m_tasks_func[name], args, interval, singlesht, endtime);
	}

	return -1;
}

int Cron::startTask(const QString& name, const QString& args /*= QString()*/, const QDateTime& time /*= QDateTime()*/)
{
	if (time < QDateTime::currentDateTime())
	{
		return -1;
	}

	int time_diff = time.toMSecsSinceEpoch() - QDateTime::currentDateTime().toMSecsSinceEpoch();
	Q_ASSERT(time_diff > 0);

	if (Instance().m_tasks_func.contains(name))
	{
		return startTask(name, Instance().m_tasks_func[name], args, time_diff, true);
	}

	return -1;
}

void Cron::stopTask(int id)
{
	QMutexLocker locker(&Instance().m_taks_mutex);
	Instance().m_tasks.remove(id);
	SqlQuery("DELETE FROM cron WHERE taskid = " + QString::number(id));
	SqlDatabase::close();
}

void Cron::resumeTasks()
{
	QMutexLocker locker(&Instance().m_func_mutex);
	SqlQuery tasks("SELECT * FROM cron");
	qDebug() << "Resuming unfinished cron tasks";
	while (tasks.next())
	{
		QString task = tasks.value("task").toString();
		if (m_tasks_func.contains(task))
		{
			newTask(m_tasks_func[task], tasks.value("taskid").toInt(), tasks.value("args").toString(), tasks.value("starttime").toInt(), tasks.value("oncetime").toBool(), tasks.value("endtime").toLongLong());
		}
	}
	SqlDatabase::close();
}

void Cron::addFunc(const QString& name, void(*func)(int, const QString&))
{
	QMutexLocker locker(&Instance().m_func_mutex);
	Instance().m_tasks_func.insert(name, func);
}

}
