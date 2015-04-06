#include "translatorhelper.h"
#include <QDebug>
#include <QTextCodec>

namespace novastory
{

	TranslatorHelper::TranslatorHelper(const QString& ts_file)
	{
		open(ts_file);
	}

	TranslatorHelper::TranslatorHelper()
	{

	}


	void TranslatorHelper::open(const QString& ts_file)
	{
		m_diskfile.setFileName(ts_file);

		if (!m_diskfile.open(QIODevice::ReadOnly))
		{
			createTSBase();
			return;
		}

		if (!m_tsfile.setContent(&m_diskfile))
		{
			createTSBase();
		}

		m_diskfile.close();
	}


	bool TranslatorHelper::save(const QString& output_file)
	{
		if (!output_file.isEmpty())
		{
			m_diskfile.setFileName(output_file);
		}

		if (!m_diskfile.open(QIODevice::WriteOnly))
		{
			m_diskfile.close();
			return false;
		}

		QDomElement docElem = m_tsfile.documentElement();
		QDomElement context = docElem.firstChildElement("context");
		while (!context.isNull())
		{
			QDomElement message = context.firstChildElement("message");
			while (!message.isNull())
			{
				QString source = message.firstChildElement("source").firstChild().toText().data();

				if (m_translations.contains(source))
				{
					m_translations[source].existed = true;
				}

				message = message.nextSiblingElement("message");
			}
			context = context.nextSiblingElement("context");
		}

		QHashIterator<QString, TUnit> tr_it(m_translations);
		QDomElement context_element;
		while (tr_it.hasNext())
		{
			tr_it.next();

			if (!tr_it.value().existed)
			{
				// Creating new message
				QDomElement source = m_tsfile.createElement("source");
				source.appendChild(m_tsfile.createTextNode(tr_it.key()));
				QDomElement translation = m_tsfile.createElement("translation");
				translation.setAttribute("type", "unfinished");
				QDomElement message = m_tsfile.createElement("message");
				message.appendChild(source);
				message.appendChild(translation);
				if (!tr_it.value().comment.isEmpty())
				{
					QDomElement comment = m_tsfile.createElement("comment");
					comment.appendChild(m_tsfile.createTextNode(tr_it.value().comment));
					message.appendChild(comment);
				}

				QString contextName = tr_it.value().context.isNull() ? "QObject" : tr_it.value().context;
				qDebug() << contextName;
				qDebug() << context_element.firstChildElement("name").firstChild().toText().data();
				if (context_element.firstChildElement("name").firstChild().toText().data() != contextName)
				{
					if (!context_element.isNull())
						docElem.appendChild(context_element);
					QDomElement name = m_tsfile.createElement("name");
					name.appendChild(m_tsfile.createTextNode(contextName));
					context_element = m_tsfile.createElement("context");
					context_element.appendChild(name);
				}

				context_element.appendChild(message);


			}
		}
		if (!context_element.isNull())
			docElem.appendChild(context_element);

		QTextStream textstream(&m_diskfile);
		textstream.setCodec(QTextCodec::codecForName("UTF-8"));
		m_tsfile.save(textstream, 4, QDomNode::EncodingFromTextStream);
		m_diskfile.close();

		return true;
	}

	QString TranslatorHelper::tr(const QString& key, const QString& dis /*=QString()*/, const QString& context /*=QString()*/)
	{
		if (!key.isEmpty())
			m_translations[key].existed = false;
		if (!dis.isNull())
			m_translations[key].comment = dis;
		if (!context.isNull())
			m_translations[key].context = context;
		return key;
	}

	void TranslatorHelper::createTSBase()
	{
		QDomElement ts = m_tsfile.createElement("TS");
		ts.setAttribute("version", "2.1");
		m_tsfile.appendChild(ts);
		QDomProcessingInstruction instruct = m_tsfile.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"utf-8\"");
		m_tsfile.insertBefore(instruct, m_tsfile.firstChild());
	}

}