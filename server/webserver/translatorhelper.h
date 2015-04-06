#ifndef TRANSLATOR_HELPER_H
#define TRANSLATOR_HELPER_H

#include <QDomDocument>
#include <QFile>
#include <QString>
#include <QHash>

namespace novastory
{

	class TranslatorHelper
	{
	public:
		TranslatorHelper(const QString& ts_file);
		TranslatorHelper();
		void open(const QString& ts_file);
		bool save(const QString& output_file = "");
		QString tr(const QString& key, const QString& dis = QString(), const QString& context = QString());
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
		void createTSBase();
		QDomDocument m_tsfile;
		QFile m_diskfile;
		QHash<QString, TUnit> m_translations;
	};

}

#endif