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
	TextRevision insert();
	TextRevision insert(const QString& text);
	TextRevision insert(char* text);
	TextRevision update(int revision = 0);
	TextRevision update(const TextRevision& revision, const QString& text = QString());
	TextRevision update(const QString& text);
	TextRevision update(char* text);
	bool updateMark(int revision);
	bool updateMark(const TextRevision& targetRevision);
	bool removeRevision(const TextRevision& targetRevision);
	bool removeRevision(int revision);
	void clear();
	bool release(int targetRevision);
	bool release(const TextRevision& targetRevision);
	bool unrelease(int targetRevision);
	bool unrelease(const TextRevision& targetRevision);

	TextRevision revision(int rev);

	void setText(const QString& text);
	void setMark(const QString& text);
	void setUser(const User& user);

	int userid() const;

	QString json(bool withoutText = false);
private:
	User m_user;
	QString m_text;
	QString m_mark;
	bool m_synchronized;
};

}

#endif
