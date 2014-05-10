#include "userpic.h"

novastory::UserPic::UserPic()
{
	setObjectName("userspics");
}

const novastory::WebDataContainer& novastory::UserPic::userPic() const
{
	return data();
}

void novastory::UserPic::setUserPic(const novastory::WebDataContainer& image)
{
	setData(image);
}
