#ifndef	TEXTREVISIONCONTAINER_H
#define TEXTREVISIONCONTAINER_H

#include "textrevision.h"
#include <QMap>
#include "user.h"
#include "jsonthrower.h"
#include "sql/nestedset.h"

class Test_TextRevision;

namespace novastory
{

class TextRevisionContainer : public QMap<int, TextRevision>, public JsonThrower, public NestedSet
{
	friend class ::Test_TextRevision;
public:
	TextRevisionContainer();

	bool sync(int parentId = 0);
	TextRevision insert(int parentId = 0);
	TextRevision insert(const QString& text, int parentId = 0);
	TextRevision insert(char* text, int parentId = 0);
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
	QString treeFolders();
private:
	User m_user;
	QString m_text;
	QString m_mark;
	bool m_synchronized;
};

}

#endif
