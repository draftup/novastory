#ifndef TEMPLATOR_H
#define TEMPLATOR_H

#include <QByteArray>
#include <QHash>
#if defined(NOVASTORY_BUILD) || defined(VSTEAMS_BUILD)
#include "logic/user.h"
#endif
#include <QMutex>

namespace novastory
{
/**
 * @class	Templator
 *
 * @brief	Template generator for all pages
 *
 * @author	Alexey Kasyanchuk
 * @date	04.04.2014
 */

class Templator
{
public:
	/**
	 * @brief	Generate standart page content
	 *
	 * @author	Alexey Kasyanchuk
	 * @date	04.04.2014
	 *
	 * @param	title  	(Optional) the title of page
	 * @param	article	(Optional) the article of page
	 *
	 * @return	HTML
	 */
	static QByteArray generate(
		const QString& title = "Novastory",
		const QString& article = QString(),
		const QHash<QString, QString>& add_map = QHash<QString, QString>(),
		const QString& description = QString(),
		const QString& keywords = QString()
	);

	static void translate(QString& html)
	{
		QRegExp rx("\\{\\{(.+)\\}\\}");
		rx.setMinimal(true);
		while (rx.indexIn(html) != -1)
		{
			html.replace(rx.pos(), rx.matchedLength(), novastory::tr(rx.cap(1)));
		}
	}

#if defined(NOVASTORY_BUILD) || defined(VSTEAMS_BUILD)
	static QByteArray generate(
		const User& user,
		const QString& title = "Novastory",
		const QString& article = QString(),
		const QHash<QString, QString>& add_map = QHash<QString, QString>(),
		const QString& description = QString(),
		const QString& keywords = QString()
	);

	static QByteArray generateLogined(
		const User& user,
		const QString& title = "Novastory",
		const QString& article = QString(),
		const QHash<QString, QString>& add_map = QHash<QString, QString>(),
		const QString& description = QString(),
		const QString& keywords = QString()
	);
#endif
private:
	static QMutex loadTemplateMutex;
};
}

#endif //TEMPLATOR_H