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
	TextRevision save(bool isRelease = false);
	TextRevision save(const QString& text, bool isRelease = false);
	TextRevision save(char* text, bool isRelease = false);
	void clear();
	bool release(int targetRevision);
	bool release(const TextRevision& targetRevision);
	bool unrelease(int targetRevision);
	bool unrelease(const TextRevision& targetRevision);

	TextRevision revision(int rev);

	void setText(const QString& text);
	void setUser(const User& user);

	int userid() const;

	QString json(bool withoutText = false);
private:
	User m_user;
	QString m_text;
	bool m_synchronized;
};

}

#endif
