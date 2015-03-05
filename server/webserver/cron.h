#ifndef CRON_H
#define CRON_H

#include <QThread>
#include <QTimer>
#include <QSharedPointer>
#include <QMutex>

namespace novastory
{

class Cron : public QThread
{
	Q_OBJECT
public:
	Cron();
	~Cron();

	static Cron& Instance()
	{
		static Cron theSingleInstance;
		return theSingleInstance;
	}

	static void newTask(void(*func)(void), int interval = 1000, bool singlesht = false);
	static void newTask(void(*func)(void), const QDateTime& time);
protected:
	void run() override;
private:
	QList<QSharedPointer<QTimer>> m_tasks;
	QMutex m_mutex;
private slots:
	void addTask(void* timer);
};

}

#endif
