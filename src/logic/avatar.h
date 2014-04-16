#ifndef	AVATAR_H
#define AVATAR_H

#include "sql/sqlizable.h"
#include "jsonthrower.h"
#include "dataimage.h"

class Test_Avatar;

namespace novastory
{

class Avatar : protected Sqlizable, public JsonThrower
{
	friend class ::Test_Avatar;
	Q_OBJECT
	Q_PROPERTY(int userid READ userid WRITE setUserid)
	Q_PROPERTY(QByteArray avatar READ avatar WRITE setAvatar)
	Q_PROPERTY(QString contenttype READ contentType WRITE setContentType)
	Q_PROPERTY(unsigned int contentsize READ contentSize WRITE setContentSize)
public:
	Avatar();

	int userid() const;
	void setUserid(int userid);

	const QByteArray& avatar() const;
	void setAvatar(const QByteArray& avatar);
	void setAvatar(const DataImage& image);

	const QString& email() const;
	void setEmail(const QString& email);

	bool sync();
	bool update();
	bool remove();

	unsigned int contentSize() const {return m_conentSize;};
	void setContentSize(unsigned int contsize) { m_conentSize = contsize; };

	const QString& contentType() const { return m_contentType; };
	void setContentType(const QString& ct) { m_contentType = ct; };
private:
	int m_userid;
	QByteArray m_avatar;
	QString m_email;
	unsigned int m_conentSize;
	QString m_contentType;
};

}

#endif
