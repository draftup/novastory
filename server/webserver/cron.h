#ifndef CRON_H
#define CRON_H

#include <QThread>
#include <QTimer>
#include <QSharedPointer>
#include <QMutex>
#include <QMap>
#include <QHash>
#include <QString>
#include <QDateTime>

class Test_Cron;

namespace novastory
{

class Cron : public QThread
{
	Q_OBJECT
	friend class ::Test_Cron;
public:
	Cron();
	~Cron();

	static Cron& Instance()
	{
		static Cron theSingleInstance;
		return theSingleInstance;
	}

	void resumeTasks();

	static int startTask(const QString& name, const QString& args = QString(), int interval = 1000, bool singlesht = false);
	static int startTask(const QString& name, const QString& args = QString(), const QDateTime& time = QDateTime());
	static int startTask(const QString& name, void(*func)(int, const QString&), const QString& args = QString(), const QDateTime& time = QDateTime());
	static int startTask(const QString& name, void(*func)(int, const QString&), const QString& args = QString(), int interval = 1000, bool singlesht = false);
	static void stopTask(int id);

	static void addFunc(const QString& name, void(*func)(int, const QString&));
protected:
	void run() override;
private:
	static void newTask(void(*func)(int, const QString&), int id, const QString& args = QString(), int interval = 1000, bool singlesht = false);

	QMap<int, QSharedPointer<QTimer>> m_tasks;
	QHash<QString, void(*)(int, const QString&)> m_tasks_func;
	QMutex m_taks_mutex;
	QMutex m_func_mutex;
private slots:
	void addTask(void* timer);
};

}

#endif
