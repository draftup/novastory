NovastoryApi = {};

NovastoryApi.login = function (email, rawpassword, callback)
{
	$.post(
		'/api/login',
	{
		email : email,
		password : $.sha1(rawpassword)
	},
		callback,
		'json');
};

NovastoryApi.register = function (email, rawpassword, challenge, response, callback)
{
	$.post(
		'/api/register',
	{
		email : email,
		password : $.sha1(rawpassword),
		challenge : challenge,
		response : response
	},
		callback,
		'json');
};

NovastoryApi.restorepassword = function (email, callback)
{
	$.post(
		'/api/forgotpassword',
	{
		email : email
	},
		callback,
		'json');
}

NovastoryApi.updateAvatar = function (avatar, callback)
{
	$.post(
		'/api/updateavatar',
	{
		avatar : avatar
	},
		callback,
		'json');
}

NovastoryApi.updateUserPic = function (userpic, callback)
{
	$.post(
		'/api/updateuserpic',
	{
		userpic : userpic
	},
		callback,
		'json');
}

NovastoryApi.updateProfile = function (firstname, lastname, nickname, profileid, callback)
{
	$.post(
		'/api/updateprofile',
	{
		firstname : firstname,
		lastname : lastname,
		nickname : nickname,
		profileid : profileid
	},
		callback,
		'json');
}

NovastoryApi.myProfile = function (callback)
{
	$.post(
		'/api/myprofile',
	{},
		callback,
		'json');
}
