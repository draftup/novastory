#ifndef TEMPLATOR_H
#define TEMPLATOR_H

#include <QByteArray>
#include "logic/user.h"

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
		const QString& article = QString()
	);

	static QByteArray generateLogined(
		const User& user,
		const QString& title = "Novastory",
		const QString& article = QString()
	);
};
}

#endif //TEMPLATOR_H