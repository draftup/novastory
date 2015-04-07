#include "translatorhelper.h"
#include <QDebug>
#include <QTextStream>

namespace novastory
{

	TranslatorHelper::TranslatorHelper()
	{

	}

	bool TranslatorHelper::save(const QString& output_file)
	{
		if (!output_file.isEmpty())
		{
			m_diskfile.setFileName(output_file);
		}

		if (!m_diskfile.open(QIODevice::ReadWrite | QIODevice::Text))
		{
			m_diskfile.close();
			return false;
		}

		QString current = m_diskfile.readAll();

		QMutexLocker locker(&append_mutex);

		QRegExp rx("([a-z]+)::tr\\(\"(.+)\"(?:,\\w\"(.+)\"\\))?");
		int pos = 0;
		while ((pos = rx.indexIn(current, pos)) != -1)
		{
			pos += rx.matchedLength();
			if (!m_translations.contains(rx.cap(2)))
			{
				m_translations[rx.cap(2)].existed = true;
				m_translations[rx.cap(2)].comment = rx.cap(3);
				m_translations[rx.cap(2)].context = rx.cap(1);
			}
			else
			{
				m_translations[rx.cap(2)].existed = true;
			}
		}

		m_diskfile.resize(0);
		QTextStream steam(&m_diskfile);

		QMapIterator<QString, TUnit> it(m_translations);
		while (it.hasNext())
		{
			it.next();
			steam << it.value().context << "::tr(\"" << it.key() << "\"" << (it.value().comment.isNull() ? "" : QString("\"%1\"").arg(it.value().comment)) << ");\n";
		}

		m_diskfile.close();
		
		return true;
	}

	void TranslatorHelper::appendTranslation(const QString& key, const QString& dis /*=QString()*/, const QString& context /*=QString()*/)
	{
		QMutexLocker locker(&append_mutex);
		if (!key.isEmpty())
			m_translations[key].existed = false;
		if (!dis.isNull())
			m_translations[key].comment = dis;
		if (!context.isNull())
			m_translations[key].context = context;
	}

}