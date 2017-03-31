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

void Cron::newTask(void(*func)(int, const QString&, bool last_call), int id, const QString& args /* = QString()*/, int interval /* = 1000 */, bool singlesht /*= false*/, const QDateTime& starttime /* = QDateTime()*/, const QDateTime& endtime /* = QDateTime() */)
{
	QMutexLocker locker(&Instance().m_taks_mutex);
	qDebug() << "New cron task" << id << "added to query";
	QTimer* timer = new QTimer();
	timer->moveToThread(&Instance());
	if (!singlesht)
	{
		timer->setInterval(interval);
		qDebug() << "Cron task started with interval: " << interval;
	}
	else
	{
		qint64 diff_time = starttime.toMSecsSinceEpoch() - QDateTime::currentDateTime().toMSecsSinceEpoch();
		timer->setInterval(diff_time > 0 ? diff_time : 0);
		qDebug() << "Cron task will started at" << starttime << "once";
	}
	timer->setSingleShot(singlesht);
	VERIFY(connect(timer, &QTimer::timeout, std::bind([timer, func, endtime](int taskid, const QString & args)
	{
		bool last_call = timer->isSingleShot() || (!endtime.isNull() && QDateTime::currentDateTime() > endtime);
		func(taskid, args, last_call);

		if (last_call)
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

int Cron::startTask(const QString& name, void(*func)(int, const QString&, bool last_call), const QString& args /* = QString()*/, int interval /*= 1000*/, bool singlesht /*= false*/, const QDateTime& endtime /* = QDateTime()*/)
{
	QMutexLocker locker(&Instance().m_func_mutex);
	SqlQuery saveTask;
	saveTask.prepare("INSERT INTO cron(`task`, `args`, `starttime`, `endtime`, `oncetime`, `interval`) VALUES(?, ?, ?, ?, ?, ?)");
	saveTask.bindValue(0, name);
	saveTask.bindValue(1, args);
	QDateTime starttime = QDateTime::fromMSecsSinceEpoch(QDateTime::currentDateTime().toMSecsSinceEpoch() + interval);
	saveTask.bindValue(2, starttime);
	saveTask.bindValue(3, endtime);
	saveTask.bindValue(4, singlesht);
	saveTask.bindValue(5, interval);
	VERIFY(saveTask.exec());
	Instance().m_tasks_func.insert(name, func);
	int id = saveTask.lastInsertId().toInt();
	SqlDatabase::close();
	newTask(func, id, args, interval, singlesht, starttime, endtime);
	return id;
}

int Cron::startTask(const QString& name, void(*func)(int, const QString&, bool last_call), const QString& args /* = QString()*/, const QDateTime& time)
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

void Cron::stopTask(const QString& name)
{
	SqlQuery q;
	q.prepare("SELECT `taskid` FROM cron WHERE task = ?");
	q.bindValue(0, name);
	q.exec();
	if (q.next())
	{
		stopTask(q.value("taskid").toInt());
	}
}

void Cron::stopTask(const QString& name, const QString& args)
{
	SqlQuery q;
	q.prepare("SELECT `taskid` FROM cron WHERE task = ? AND args = ?");
	q.bindValue(0, name);
	q.bindValue(1, args);
	q.exec();
	if (q.next())
	{
		stopTask(q.value("taskid").toInt());
	}
}

void Cron::resumeTasks()
{
	QMutexLocker locker(&Instance().m_func_mutex);
	SqlQuery tasks("SELECT * FROM cron");
	qDebug() << "Resuming unfinished cron tasks";
	while (tasks.next())
	{
		QString task = tasks.value("task").toString();
		if (Instance().m_tasks_func.contains(task))
		{
			if (!tasks.value("endtime").toDateTime().isNull() && QDateTime::currentDateTime() > tasks.value("endtime").toDateTime())
			{
				SqlQuery("DELETE FROM cron WHERE taskid = " + QString::number(tasks.value("taskid").toInt()));
			}
			else
			{
				newTask(Instance().m_tasks_func[task], tasks.value("taskid").toInt(), tasks.value("args").toString(), tasks.value("interval").toInt(), tasks.value("oncetime").toBool(), tasks.value("starttime").toDateTime(), tasks.value("endtime").toDateTime());
			}
		}
	}
	SqlDatabase::close();
}

void Cron::addFunc(const QString& name, void(*func)(int, const QString&, bool last_call))
{
	QMutexLocker locker(&Instance().m_func_mutex);
	Instance().m_tasks_func.insert(name, func);
}

void Cron::setTaskTime(int id, const QDateTime& time /*= QDateTime()*/)
{
	QMutexLocker locker(&Instance().m_taks_mutex);
	if (Instance().m_tasks.contains(id) && Instance().m_tasks[id]->isSingleShot())
	{
		int interval = time.toMSecsSinceEpoch() - QDateTime::currentDateTime().toMSecsSinceEpoch();
		QMetaObject::invokeMethod(&Instance(), "changeTime", Qt::QueuedConnection, Q_ARG(void*, (void*)Instance().m_tasks[id].data()), Q_ARG(int, interval));
		SqlQuery updateTask;
		updateTask.prepare("UPDATE `cron` SET `interval` = :interval, `starttime` = :starttime WHERE `taskid` = :id");
		updateTask.bindValue(":id", id);
		updateTask.bindValue(":interval", interval);
		updateTask.bindValue(":starttime", time);
		VERIFY(updateTask.exec());
	}
}

void Cron::changeTime(void* timer, int interval)
{
	((QTimer*)timer)->setInterval(interval);
	qDebug() << "Task time changed";
}

}
