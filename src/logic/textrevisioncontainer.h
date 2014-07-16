#ifndef	TEXTREVISIONCONTAINER_H
#define TEXTREVISIONCONTAINER_H

#include "textrevision.h"
#include <QMap>
#include "user.h"
#include "jsonthrower.h"

class Test_TextRevision;

namespace novastory
{

class TextRevisionContainer : public QMap<int, TextRevision>, public JsonThrower
{
	friend class ::Test_TextRevision;
public:
	TextRevisionContainer();

	bool sync();
	bool save();
	bool save(const QString& text);
	void clear();
	bool release(int targetRevision);
	bool release(const TextRevision& targetRevision);

	void setText(const QString& text);
	void setUser(const User& user);

	int userid() const;

	QString json();
private:
	User m_user;
	QString m_text;
	bool m_synchronized;
};

}

#endif
