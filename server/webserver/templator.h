#ifndef TEMPLATOR_H
#define TEMPLATOR_H

#include <QByteArray>
#include <QHash>
#if defined(NOVASTORY_BUILD) || defined(VSTEAMS_BUILD)
#include "logic/user.h"
#endif

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
		const QHash<QString, QString>& add_map = QHash<QString, QString>()
	);

#if defined(NOVASTORY_BUILD) || defined(VSTEAMS_BUILD)
	static QByteArray generate(
		const User& user,
		const QString& title = "Novastory",
		const QString& article = QString(),
		const QHash<QString, QString>& add_map = QHash<QString, QString>()
	);

	static QByteArray generateLogined(
		const User& user,
		const QString& title = "Novastory",
		const QString& article = QString(),
		const QHash<QString, QString>& add_map = QHash<QString, QString>()
	);
#endif
};
}

#endif //TEMPLATOR_H