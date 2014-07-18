// Author: DEgITx <degitx@gmail.com>

// Some globals
jQuery.fn.exists = function ()
{
	return jQuery(this).length > 0;
}

includeJS("/js/jquery.sha1.js");
includeJS("/js/jquery.cookie.js");
var USERID;
var STOKEN;

$(document).ready(function ()
{
	Novastory = {}

	Novastory.animationLocker = false;

	Novastory.popupHelper = function (message)
	{
		var pw = $('#popup-message');
		pw.text(message);
		pw.show();

		if (!this.animationLocker)
		{
			this.animationLocker = true;
			$("#popup-message").animate(
			{
				top : "70",
				opacity : 0.6
			}, 400).delay(1700).animate(
			{
				top : "50%",
				opacity : 0
			}, 500, function ()
			{
				pw.css('top', '0px');
				pw.hide();
				Novastory.animationLocker = false;
			}
			);
		}
	}

	Novastory.error = function (errormessage)
	{
		$('#popup-message').attr('class', 'error');
		this.popupHelper(errormessage);
	}

	Novastory.ok = function (okmessage)
	{
		$('#popup-message').attr('class', 'ok');
		this.popupHelper(okmessage);
	}

	Novastory.warning = function (wmessage)
	{
		$('#popup-message').attr('class', 'warning');
		this.popupHelper(wmessage);
	}

	Novastory.attachDropArea = function (selector_id, callback, files_match, limit_size)
	{
		files_match = files_match || 'image.*';
		limit_size = limit_size || 1 * 1024 * 1024;

		function processFiles(files)
		{
			for (var i = 0, file; file = files[i]; i++)
			{
				// Only process image files.
				if (!file.type.match(files_match))
				{
					Novastory.error("Please drop image");
					continue;
				}

				if (file.size > limit_size)
				{
					Novastory.error("File must be less than " + (limit_size / (1024 * 1024)) + "MB");
					continue;
				}

				var reader = new FileReader();
				reader.onload = function (e)
				{
					if (callback)
						callback(e.target.result);
				};
				reader.readAsDataURL(file);
			}
		}

		function handleFileSelect(event)
		{
			event.stopPropagation();
			event.preventDefault();
			processFiles(event.dataTransfer.files); // FileList object.
		}

		function handleDragOver(evt)
		{
			evt.stopPropagation();
			evt.preventDefault();
			evt.dataTransfer.dropEffect = 'copy'; // Explicitly show this is a copy.
		}

		var dropZone = document.getElementById(selector_id);
		dropZone.addEventListener('dragover', handleDragOver, false);
		dropZone.addEventListener('drop', handleFileSelect, false);

		var dropZoneJQ = $(dropZone);
		var dialog = $('<input type="file" id="' + selector_id + '-dialog" style="display: none;" />');
		dropZoneJQ.after(dialog);

		dropZoneJQ.click(function ()
		{
			dialog.trigger('click');
		}
		);

		dialog.change(function ()
		{
			processFiles($(this)[0].files);
		}
		);
	}

	function login(event)
	{
		var login = $('#loginmail').val();
		var pass = $('#loginpass').val();

		var data = NovastoryApi.loginSync(login, pass);
		if (data.logined != null && data.logined)
		{
			USERID = data.userid;
			STOKEN = data.token;
			$.cookie("userid", data.userid,
			{
				path : '/',
				expires : 7
			}
			);
			$.cookie("stoken", data.token,
			{
				path : '/',
				expires : 7
			}
			);
			// Does not reload page because work will do this
			//window.location.href = window.location.href;
		}
		else
		{
			Novastory.error("Login failed. Check your login and password");
			if (event)
				event.preventDefault();
		}
	}

	function logout()
	{
		$.removeCookie("userid");
		$.removeCookie("stoken");

		// reload page after logout
		window.location.href = window.location.href;
	}

	function proceedRegistration(email_adress, password_original, password_verify, challenge, response)
	{
		NovastoryApi.register(email_adress, password_original, challenge, response, function (data)
		{

			if (data.error == null || data.error)
			{
				Novastory.error("Registration error");
			}
			else
			{
				$('#registration-panel #registration-body').text("Congratulations with registration. You must recive mail with authorization link.");
				Novastory.ok("Congratulations with registration");
			}
		}
		);

	}

	function doRecaptcha()
	{
		if ($("#registration-panel #tocapcha").attr('class') != 'enabled')
		{
			return;
		}

		var email_adress = $('#registration-panel #regmail').val();
		var password_original = $('#registration-panel #regpass').val();
		var password_verify = $('#registration-panel #confregpass').val();

		var trueRegistrationBody = $('#registration-panel #registration-body').clone();

		// init captcha
		Recaptcha.create(
			"6LfjsO4SAAAAADEhNeb51KeXPpVu_vtFQRC0w4iO",
			"registration-body",
		{
			theme : "red",
			callback : function ()
			{
				$('#registration-body').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						var challenge = Recaptcha.get_challenge();
						var response = Recaptcha.get_response();
						Recaptcha.destroy();
						$(this).unbind("keyup");
						$('#registration-panel #registration-body').replaceWith(trueRegistrationBody);
						proceedRegistration(email_adress, password_original, password_verify, challenge, response);
					}
				}
				);

				Recaptcha.focus_response_field();
			}
		}
		);
	}

	function doRestorePassword()
	{
		var email = $('#restore-panel #restoremail').val();

		NovastoryApi.restorepassword(email, function (data)
		{
			if (data.error == null || data.error)
			{
				Novastory.error("Error on password restore");
			}
			else
			{
				Novastory.ok("Message sended to your email");
				$("#login-panel").show();
				$("#restore-panel").hide();
			}
		}
		);
	}

	$('#login').click(function ()
	{
		if (!$('#login-panel').exists())
		{
			$('#login-space').load('/modal-login.html #login-panel', null, function ()
			{
				var loginForm = document.getElementById("login-form");
				loginForm.onsubmit = function (event)
				{
					login(event);
				};
				$('#login-form').attr('action', window.location.href);

				$('#loginmail').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						$('#login-form').submit();
					}
				}
				);

				$('#loginpass').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						$('#login-form').submit();
					}
				}
				);

				$('#loginbutt').click(function ()
				{
					$('#login-form').submit();
				}
				);

				$('#login-panel #newacc').click(function ()
				{
					$("#login-panel").hide();
					$('#registration-panel').show();
				}
				);

				$('#login-panel #restorepass').click(function ()
				{
					$("#login-panel").hide();
					$('#restore-panel').show();
				}
				);
			}
			);
		}
		else
		{
			if (!$('#login-panel').is(":hidden") || !$('#registration-panel').is(":hidden") || !$('#restore-panel').is(":hidden"))
			{
				$("#login-panel").hide();
				$("#registration-panel").hide();
				$("#restore-panel").hide();
			}
			else
			{
				$("#login-panel").show();
			}
		}

		if (!$('#restore-panel').exists())
		{
			$('#restore-space').load('/modal-restore.html #restore-panel', null, function ()
			{
				$('#restore-panel').hide();

				$('#restore-panel #restorepassword').click(function ()
				{
					doRestorePassword();
				}
				);

				$('#restore-panel #backtologin').click(function ()
				{
					$('#restore-panel').hide();
					$("#login-panel").show();
				}
				);

				$('#restore-panel #restoremail').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doRestorePassword();
					}
				}
				);
			}
			);
		}

		if (!$('#registration-panel').exists())
		{
			$('#register-space').load('/modal-register.html #registration-panel', null, function ()
			{
				$('#registration-panel').hide();

				$('#registration-panel #myacc').click(function ()
				{
					$('#registration-panel').hide();
					$('#login-panel').show();
				}
				);

				function checkReady()
				{
					var email_adress = $('#registration-panel #regmail').val();
					var password_original = $('#registration-panel #regpass').val();
					var password_verify = $('#registration-panel #confregpass').val();

					if (email_adress.length == 0 || password_original.length == 0 || password_verify.length == 0)
					{
						$("#registration-panel #tocapcha").attr('class', 'disabled');
						return;
					}

					if (password_original != password_verify)
					{
						$("#registration-panel #tocapcha").attr('class', 'disabled');
						return;
					}

					if (!$('#registration-panel #checkterms').prop('checked'))
					{
						$("#registration-panel #tocapcha").attr('class', 'disabled');
						return;
					}

					$("#registration-panel #tocapcha").attr('class', 'enabled');
				}

				$('#registration-panel #regmail').keyup(function (e)
				{
					checkReady();

					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#registration-panel #regpass').keyup(function (e)
				{
					checkReady();

					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#registration-panel #confregpass').keyup(function (e)
				{
					checkReady();

					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#registration-panel #tocapcha').click(function ()
				{
					doRecaptcha();
				}
				);

				$('#registration-panel #checkterms').click(function ()
				{
					checkReady();
				}
				);
			}
			);
		}
	}
	);

	$('#outico').click(function ()
	{
		logout();
	}
	);

	$("#panelava img").error(function ()
	{
		$(this).hide();
	}
	);

	$("#panelava").click(function ()
	{
		window.location.href = '/' + PROFILEID;
	}
	);

	// Profile settings
	$('#setico').click(function ()
	{
		if (!$('#modal-sett').exists())
		{
			$('#settings-space').load('/modal-settings.html #modal-sett', null, function ()
			{
				// Hide from user until profile info is loaded
				$("#modal-sett").hide();

				// Loading my profile
				NovastoryApi.myProfile(function (data)
				{
					if (data.error != null && data.error)
					{
						Novastory.error("Error durring profile loading");
					}
					else
					{
						$("#myfirstname").val(data.user.firstname);
						$("#mylastname").val(data.user.lastname);
						$("#mynickname").val(data.user.nickname);
						$("#myprofileid").val(data.user.profileid);
						$("#modal-sett").show();
					}
				}
				);

				Novastory.attachDropArea('avablock', function (image)
				{
					$("#avapreview").show();
					$("#avapreview").attr('src', image);
					NovastoryApi.updateAvatar(image, function (data)
					{
						if (data.error != null && data.error)
						{
							Novastory.error("Error on file upload");
						}
						else
						{
							Novastory.ok("Your avatar updated. You can continues to update profile.");
							$("#panelava img").show();
							$("#panelava img").attr('src', image);
						}
					}
					);
				}
				);

				$("#avapreview").attr('src', "/avatar/" + USERID);

				$("#avapreview").error(function ()
				{
					$(this).hide();
				}
				);

				Novastory.attachDropArea('artblock', function (image)
				{
					$("#artpreview").show();
					$("#artpreview").attr('src', image);
					NovastoryApi.updateUserPic(image, function (data)
					{
						if (data.error != null && data.error)
						{
							Novastory.error("Error on file upload");
						}
						else
						{
							Novastory.ok("Your profile pic updated. You can continues to update profile.");
						}
					}
					);
				}
				);

				$("#artpreview").attr('src', "/userpic/" + USERID);

				$("#artpreview").error(function ()
				{
					$(this).hide();
				}
				);

				$("#savesett").click(function ()
				{
					var profileid = $("#myprofileid").val();
					if (!profileid.match(/[A-Za-z0-9_]+/) || profileid.length > 64)
					{
						Novastory.error("You profile indificator contains bad symbols. Use latin.");
						return;
					}

					NovastoryApi.updateProfile($("#myfirstname").val(), $("#mylastname").val(), $("#mynickname").val(), profileid, function (data)
					{
						if (data.error != null && data.error)
						{
							Novastory.error("Error on profile update. Contact administation.");
						}
						else
						{
							$("#panelava").unbind('click');
							$("#panelava").click(function ()
							{
								window.location.href = '/' + profileid;
							}
							);
							Novastory.ok("Your profile updated");
						}
					}
					);
					$("#modal-sett").hide();
				}
				);

				$("#cancsett").click(function ()
				{
					$("#modal-sett").hide();
				}
				);
			}
			);
		}
		else // panel already exists (hide show)
		{
			if ($("#modal-sett").is(":hidden"))
				$("#modal-sett").show();
			else
				$("#modal-sett").hide();
		}
	}
	);

	// hide chat panel
	$('#chatpanel').hide();

	$("#chatpanel").css('right', '-280px');
	$('#messico').click(function ()
	{
		var panel = $("#chatpanel");
		if (panel.is(":hidden"))
		{
			panel.show();
			panel.animate(
			{
				right : "0"
			}, 300, function ()
			{
				// Animation complete.
			}
			);
		}
		else
		{
			panel.animate(
			{
				right : "-280"
			}, 300, function ()
			{
				panel.hide();
			}
			);
		}
	}
	);

	if ($('#follbutt').exists())
	{
		var profileid = parseInt($('#profileidspace').text());

		if (profileid <= 0)
		{
			Novastory.error("Error durring profile loading");
		}

		var halfButton = $('#follbutt');
		var fullButton = halfButton.parent();

		fullButton.hide();

		if (USERID == profileid)
		{
			$("#myprocount").show();
			// свой профиль
			$('#wallref').addClass('myown');
		}
		else if (USERID > 0)
		{
			$("#myprocount").hide();
			NovastoryApi.isSubscribed(profileid, function (data)
			{
				fullButton.show();

				var isSubscribed = data.issubscribed;

				if (isSubscribed)
				{
					fullButton.attr('id', 'followed');
					halfButton.text('Unfollow');
				}

				halfButton.click(function ()
				{
					if (!isSubscribed)
					{
						NovastoryApi.subscribe(profileid, function (data)
						{
							if (data.error != null && data.error)
							{
								Novastory.error("Error on subscribe");
							}
							else
							{
								isSubscribed = true;
								fullButton.attr('id', 'followed');
								halfButton.text('Unfollow');
							}
						}
						);
					}
					else
					{
						NovastoryApi.unsubscribe(profileid, function (data)
						{
							if (data.error != null && data.error)
							{
								Novastory.error("Error on unsubscribe");
							}
							else
							{
								isSubscribed = false;
								fullButton.attr('id', '');
								halfButton.text('Follow');
							}
						}
						);
					}
				}
				);
			}
			);
		}
	}

	//backup timer
	var saveTimerEditor = 0;
	var saveTimerEditorInterval = 15000;
	// Редактор
	function openEditor()
	{
		// Main backup function
		function backupEditor()
		{
			if ($("#editor").exists())
			{
				NovastoryApi.editorUpdate($("#editor").val());
			}
		}

		function backupEditorSync()
		{
			if ($("#editor").exists())
			{
				NovastoryApi.editorUpdateSync($("#editor").val());
			}
		}

		function hideEditor()
		{
			$('#editor-page').animate(
			{
				opacity : 0
			}, 400, function ()
			{
				$(this).hide();
			}
			);

			$('article').show();
			$('footer').show();
			$('article').animate(
			{
				opacity : 1
			}, 400, function ()
			{}

			);

			$('footer').animate(
			{
				opacity : 1
			}, 400, function ()
			{}

			);

			clearInterval(saveTimerEditor);
			saveTimerEditor = 0;
		}

		function showEditor()
		{
			var editor_panel = $('#editor-page');
			editor_panel.css('opacity', '0');
			editor_panel.show();
			editor_panel.animate(
			{
				opacity : 1
			}, 400, function ()
			{}

			);

			$('article').animate(
			{
				opacity : 0
			}, 400, function ()
			{
				$(this).hide();
			}
			);

			$('footer').animate(
			{
				opacity : 0
			}, 400, function ()
			{
				$(this).hide();
			}
			);

			$('#editor').focus();

			if (!saveTimerEditor)
				saveTimerEditor = setInterval(backupEditor, saveTimerEditorInterval);
		}

		function switchEditor()
		{
			if (!$('#editor-page').is(":hidden"))
			{
				hideEditor()
			}
			else
			{
				showEditor();
			}
		}

		function isOpenedEditor()
		{
			return !$('#editor-page').is(":hidden");
		}

		if (!$('#editor-page').exists())
		{
			// скрываем кнопку от дальнейших нажатий и перезагрузок
			$('#editico').hide();

			$('#editor-space').load('/editor.html #editor-page', null, function ()
			{
				// сразу же скрываем панель от наглых глаз
				$('#editor-page').hide();

				NovastoryApi.editorText(function (data)
				{
					$('#editico').show();

					if (data.error != null && !data.error)
					{
						var editor_panel = $('#editor-page');
						editor_panel.css('opacity', '0');
						editor_panel.show();
						editor_panel.animate(
						{
							opacity : 1
						}, 400, function ()
						{}

						);

						$('article').animate(
						{
							opacity : 0
						}, 400, function ()
						{
							$(this).hide();
						}
						);

						$('footer').animate(
						{
							opacity : 0
						}, 400, function ()
						{
							$(this).hide();
						}
						);

						$('#editor').val(data.text);
					}

					// Back up tab close
					window.onbeforeunload = backupEditorSync;

					// Back up every 15 or what sec
					saveTimerEditor = setInterval(backupEditor, saveTimerEditorInterval);

					$(window).focus(function ()
					{
						if (!saveTimerEditor && isOpenedEditor())
							saveTimerEditor = setInterval(backupEditor, saveTimerEditorInterval);
					}
					);

					$(window).blur(function ()
					{
						clearInterval(saveTimerEditor);
						saveTimerEditor = 0;
					}
					);

					// revision control
					$("#editor").keydown(function (e)
					{
						if (e.ctrlKey && e.which === 83)
						{
							e.preventDefault();
							NovastoryApi.revisionSave($("#editor").val(), function(data){
								if (data.error != null && !data.error)
								{
									Novastory.ok("Text saved in revision history");
								}
								else if(data.error != null && data.error && data.errorType == 3)
								{
									Novastory.warning("Same text was saved slightly before");
								}
								else
								{
									Novastory.error("Something wrong on text save");
								}
							});
						}
					}
					);
				}
				);
			}
			);
		}
		else
		{
			switchEditor();
		}
	}

	$('#editico').click(openEditor);

	// открытие редактора по комбинации Ctrl+E
	$(document).keydown(function (e)
	{
		if (e.ctrlKey && e.which === 69)
		{
			openEditor();
			e.preventDefault();
		}
	}
	);

	// открытие профиля на Ctrl+P
	$(document).keydown(function (e)
	{
		if (e.ctrlKey && e.which === 80)
		{
			window.location.href = '/' + PROFILEID;
			e.preventDefault();
		}
	}
	);

	$('#helpico').click(function ()
	{
		if ($('#helpme-dialog').is(":hidden"))
		{
			$('#helpme-dialog').show();
			$('#helpme-dialog').animate(
			{
				top : 40
			}, 400, function ()
			{}

			);
		}
		else
		{
			$('#helpme-dialog').animate(
			{
				top : -200
			}, 400, function ()
			{
				$(this).hide();
			}
			);
		}
	}
	);
}
);
