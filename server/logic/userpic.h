#ifndef	USERPIC_H
#define USERPIC_H

#include "logic/webdata.h"

namespace novastory
{
/// Userpicture class. Covered by Avatars test
class UserPic : public WebData
{
public:
	UserPic();

	const WebDataContainer& userPic() const;
	void setUserPic(const WebDataContainer& image);
};

}

#endif
