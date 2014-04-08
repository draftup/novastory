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
