#ifndef	TEXTREVISION_H
#define TEXTREVISION_H

#include "sql/sqlizable.h"
#include "jsonthrower.h"
#include "user.h"
#include <QDateTime>

class Test_TextRevision;

namespace novastory
{

class TextRevisionContainer;

class TextRevision : protected Sqlizable, public JsonThrower
{
	friend class ::Test_TextRevision;
	friend class TextRevisionContainer;
	Q_OBJECT
	Q_PROPERTY(int revisionid READ revisionId WRITE setRevisionID)
	Q_PROPERTY(int userid READ userid WRITE setUserID)
	Q_PROPERTY(bool release READ isRelease WRITE setRelease RESET resetRelease)
	Q_PROPERTY(QString type READ type WRITE setType RESET resetType)
	Q_PROPERTY(QDateTime createdate READ createDate WRITE setCreateDate RESET resetCreateDate)
	Q_PROPERTY(QDateTime modifydate READ modifyDate WRITE setModifyDate RESET resetModifyDate)
	Q_PROPERTY(QString mark READ mark WRITE setMark RESET resetMark)
	Q_PROPERTY(QString text READ text WRITE setText RESET resetText)
	Q_PROPERTY(int parent_id READ parent WRITE setParent RESET resetParent)
public:
	TextRevision();

	int userid() const;
	void setUserID(int userid);
	void setUser(const User& user);

	const QString& text() const;
	void setText(const QString& text);
	void resetText();

	const QString& mark() const;
	void setMark(const QString& mark);
	void resetMark();

	bool isRelease() const;
	void setRelease(bool rel);
	void resetRelease();

	const QDateTime& createDate() const;
	void setCreateDate(const QDateTime& date);
	void resetCreateDate();

	const QDateTime& modifyDate() const;
	void setModifyDate(const QDateTime& date);
	void resetModifyDate();

	const int& revisionId() const;
	void setRevisionID(int revision);

	const int& parent() const;
	void setParent(int id);
	void resetParent();

	const QString& type() const;
	void setType(QString id);
	void resetType();

	bool operator==(const TextRevision& rv) const;

	bool isValid() const;

	QJsonObject json(bool withoutText = false) const;

	void appendChild(const TextRevision& rev);
	void clearChilds();
private:
	User m_user;
	QString m_text;
	QString m_mark;
	int m_revisionId;
	bool m_release;
	QString m_type;
	QDateTime m_create_date;
	QDateTime m_modify_date;
	int m_parentId;
	QList<TextRevision> m_childs;
};

}

#endif
