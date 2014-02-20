// Author: DEgITx <degitx@gmail.com>

// Some globals
jQuery.fn.exists = function ()
{
	return jQuery(this).length > 0;
}

includeJS("js/jquery.sha1.js");

$(document).ready(function ()
{

	function doLogin()
	{
		var login = $('#login-email-field').val();
		var pass = $('#login-password-field').val();
		$.post(
            '/api/login', 
            {
                email: login,
                password: $.sha1(pass)
            }, 
            function(data)
            {
				alert(data.logined);
            }, 
            'json'
            );
	}

	$('#login-controls').click(function ()
	{
		if (!$('#login-panel').exists())
		{
			$('#page-content').after("\
											<div id=\"login-panel\">\
												<div class=\"content\">\
													<input type=\"text\" id=\"login-email-field\" placeholder=\"Логин\" />\
													<input type=\"text\" id=\"login-password-field\" placeholder=\"Пароль\" />\
													<div class=\"registration\">Регистрация</div>\
													<div class=\"password-forgotten\">Я забыл(а) пароль</div>\
												</div>\
												<div class=\"hidder\"></div>\
											</div>\
											");

			$('#login-email-field').keyup(function (e)
			{
				if (e.keyCode == 13)
				{
					doLogin();
				}
			}
			);

			$('#login-password-field').keyup(function (e)
			{
				if (e.keyCode == 13)
				{
					doLogin();
				}
			}
			);

			$('#login-panel .registration').click(function ()  {
				$("#login-panel").hide();
				
			}

			);
		}
		else
		{
			if ($('#login-panel').is(":hidden"))
			{
				$("#login-panel").show();
			}
			else
			{
				$("#login-panel").hide();
			}
		}
	}
	);

}
);
