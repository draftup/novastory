#ifndef TRANSLATOR_HELPER_H
#define TRANSLATOR_HELPER_H

#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QMap>
#include <QMutex>

namespace novastory
{

	class TranslatorHelper
	{
	public:
		TranslatorHelper();
		bool save(const QString& output_file);
		void appendTranslation(const QString& key, const QString& dis = QString(), const QString& context = QString());
		static TranslatorHelper& Instance()
		{
			static TranslatorHelper theSingleInstance;
			return theSingleInstance;
		}
	private:
		struct TUnit
		{
			bool existed;
			QString context;
			QString comment;
		};
		QFile m_diskfile;
		QMap<QString, TUnit> m_translations;
		QMutex append_mutex;
	};

}

#endif