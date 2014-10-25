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

function prettyDate(date)
{
	var months = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
	return date.getDate() + ' ' + months[date.getMonth() - 1] + ' ' + date.getHours() + ':' + date.getMinutes() + ':' + date.getSeconds();
}

$(document).ready(function ()
{
	Novastory = {}

	Novastory.animationLocker = false;

	Novastory.popupHelper = function (message)
	{
		var pw = $('#popup-message');
		pw.text(message);
		pw.attr('open', '');

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
				pw.removeAttr('open');
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
				$('#register-tab #register-tab').html("Congratulations with registration. <br />You must recive mail with authorization link.");
				Novastory.ok("Congratulations with registration");
			}
		}
		);

	}

	function doRecaptcha()
	{
		if ($("#register-tab #tocapcha").attr('class') != 'enabled')
		{
			return;
		}

		var email_adress = $('#register-tab #regmail').val();
		var password_original = $('#register-tab #regpass').val();
		var password_verify = $('#register-tab #confregpass').val();

		var trueRegistrationBody = $('#register-tab').clone();

		// init captcha
		Recaptcha.create(
			"6LfjsO4SAAAAADEhNeb51KeXPpVu_vtFQRC0w4iO",
			"register-tab",
		{
			theme : "red",
			callback : function ()
			{
				$('#register-tab').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						var challenge = Recaptcha.get_challenge();
						var response = Recaptcha.get_response();
						Recaptcha.destroy();
						$(this).unbind("keyup");
						$('#register-tab').replaceWith(trueRegistrationBody);
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
		var email = $('#restore-tab #restoremail').val();

		NovastoryApi.restorepassword(email, function (data)
		{
			if (data.error == null || data.error)
			{
				Novastory.error("Error on password restore");
			}
			else
			{
				Novastory.ok("Message sended to your email");
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

				$('#restore-tab #restorepassword').click(function ()
				{
					doRestorePassword();
				}
				);

				$('#restore-tab #restoremail').keyup(function (e)
				{
					if (e.keyCode == 13)
					{
						doRestorePassword();
					}
				}
				);

				function checkReady()
				{
					var email_adress = $('#register-tab #regmail').val();
					var password_original = $('#register-tab #regpass').val();
					var password_verify = $('#register-tab #confregpass').val();

					if (email_adress.length == 0 || password_original.length == 0 || password_verify.length == 0)
					{
						$("#register-tab #tocapcha").attr('class', 'disabled');
						return;
					}

					if (password_original != password_verify)
					{
						$("#register-tab #tocapcha").attr('class', 'disabled');
						return;
					}

					if (!$('#register-tab #checkterms').prop('checked'))
					{
						$("#register-tab #tocapcha").attr('class', 'disabled');
						return;
					}

					$("#register-tab #tocapcha").attr('class', 'enabled');
				}

				$('#register-tab #regmail').keyup(function (e)
				{
					checkReady();

					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#register-tab #regpass').keyup(function (e)
				{
					checkReady();

					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#register-tab #confregpass').keyup(function (e)
				{
					checkReady();

					if (e.keyCode == 13)
					{
						doRecaptcha();
					}
				}
				);

				$('#register-tab #tocapcha').click(function ()
				{
					doRecaptcha();
				}
				);

				$('#register-tab #checkterms').click(function ()
				{
					checkReady();
				}
				);
			}
			);
		}
		else
		{
			if (!$('#login-panel').is(":hidden"))
			{
				$("#login-panel").hide();
			}
			else
			{
				$("#login-panel").show();
			}
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

					lastClickedRevision = 0;
					function updateRevisionList()
					{
						function updateFolderContent()
						{
							if (typeof clickedRevisionInTree == 'undefined')
								clickedRevisionInTree = 0;
							NovastoryApi.revisionsList(clickedRevisionInTree, function (revisions)
							{
								var list = $('#editor-revisions');
								list.empty();
								var backupedText = {};
								for (var i = 0; i < revisions.length; i++)
								{
									var originalText = {};
									var bigestDate = 0;
									(function (i)
									{
										var dateModify = new Date(revisions[i].modifyDate);
										var element = $('<div id="revision' + revisions[i].revisionid + '"><div class="pointer"><div></div></div>'
												 + '<div class="revision-id" style="display: none;">' + revisions[i].revisionid + '</div>'
												 + '<div class="rev-name">' + ((revisions[i].mark.length > 0) ? revisions[i].mark : prettyDate(dateModify)) + '</div>'
												 + '<input size="" name="new-rev-name" value="' + ((revisions[i].mark.length > 0) ? revisions[i].mark : '') + '" autofocus>'
												 + '<svg viewBox="0 0 512 512" class="edit-rev-name" ><path id="pencil-10-icon" d="M172.782,438.836L172.782,438.836L50.417,461.42l24.686-120.264l0.001-0.001L172.782,438.836zM364.735,51.523l-43.829,43.829l97.682,97.68l43.829-43.829L364.735,51.523z M96.996,319.263l97.681,97.679l202.017-202.015l-97.68-97.682L96.996,319.263z"></path></svg>'
												 + '<svg viewBox="0 0 512 512" class="delete-rev"><path d="M424.562,78.022v40.032H87.438V78.022h94.938c15.464,0,28-12.546,28-28.022h91.25c0,15.477,12.536,28.022,28,28.022H424.562z M406.334,148.079V462H105.666V148.079H406.334z M197.333,210.462c0-8.291-6.716-15.012-15-15.012s-15,6.721-15,15.012v190.153c0,8.291,6.716,15.012,15,15.012s15-6.721,15-15.012V210.462zM271,210.462c0-8.291-6.716-15.012-15-15.012s-15,6.721-15,15.012v190.153c0,8.291,6.716,15.012,15,15.012s15-6.721,15-15.012V210.462z M344.667,210.462c0-8.291-6.716-15.012-15-15.012s-15,6.721-15,15.012v190.153c0,8.291,6.716,15.012,15,15.012s15-6.721,15-15.012V210.462z"></path></svg>'
												 + '<div class="rev-info">'
												 + '<div class="data-lenght">' + revisions[i].textLength + ' bytes</div>'
												 + '<div class="rev-date">' + prettyDate(dateModify) + '</div>'
												 + '</div>'
												 + '</div>');
										list.prepend(element);
										var revision = revisions[i].revisionid;

										if (revisions[i].modifyDate > bigestDate)
										{
											bigestDate = revisions[i].modifyDate;
											clickedRevlistDefault = revisions[i].revisionid;
										}

										// мы не хотим чтобы при нажатии на инпут обновляло элемент
										element.children('input[name=new-rev-name]').click(function (e)
										{
											e.stopPropagation();
										}
										);
										element.click(function ()
										{
											lastClickedRevision = revision;
											NovastoryApi.revision(revision, function (data)
											{
												lastClickedRevisionRelease = data.isRelease;
												var currentText = $('#editor').val();
												$('#editor-revisions > div.current').each(function ()
												{
													var revisionid = parseInt($(this).children('.revision-id').text());
													if (this != element.get(0) && originalText[revisionid] != currentText)
													{
														backupedText[revisionid] = currentText;
														// указываем что текст в бэкапе
														$(this).addClass('unsaved');
													}
												}
												);

												$('#editor-revisions > div').removeClass('current');
												element.addClass('current');
												/*
												$('#editor').on('input', function ()
											{
												currentTextElement.remove();
												currentTextElement = null;
												$(this).off('input');
												}
												);
												 */
												if (backupedText[revision] != null)
												{
													$('#editor').val(backupedText[revision]);
													delete backupedText[revision];
												}
												else
												{
													$('#editor').val(data.text);
													element.removeClass('unsaved');
												}

												originalText[data.revisionid] = data.text;
											}
											);
										}
										);

										element.children('.delete-rev').click(function (e)
										{
											e.stopPropagation();
											NovastoryApi.removeRevision(revision, function (data)
											{
												if (data.error != null && !data.error)
												{
													element.remove();
												}
												else
												{
													Novastory.error("Something wrong on revision delete");
												}
											}
											);
										}
										);

										function editMark(e)
										{
											e.stopPropagation();
											element.addClass('edited');
											var markButton = $(this);
											markButton.unbind('click');
											var markTextElement = element.children('input[name=new-rev-name]');
											function updateMark(e)
											{
												e.stopPropagation();
												var markText = markTextElement.val();
												NovastoryApi.updateRevisionMark(revision, markText, function (data)
												{
													if (data.error != null && !data.error)
													{
														element.removeClass('edited')
														element.children('.rev-name').text(markText);
														markButton.unbind('click');
														markButton.click(editMark);
													}
													else
													{
														Novastory.error("Something wrong on revision delete");
													}
												}
												);
											}
											markButton.click(updateMark);
											markTextElement.unbind('keyup');
											markTextElement.keyup(function (e)
											{
												if (e.keyCode == 13)
												{
													updateMark(e);
												}
											}
											);
										}
										element.children('.edit-rev-name').click(editMark);
									}
									)(i);
								}

								if (typeof clickedRevlistDefault != 'undefined' && clickedRevlistDefault > 0 && $('#revision' + clickedRevlistDefault).exists())
								{
									backupedText[clickedRevlistDefault] = $('#editor').val();
									var item = $('#revision' + clickedRevlistDefault);
									item.click();
									item.addClass('unsaved');
								}
							}
							);
						}
						updateFolderContent();

						//also update directories
						function updateFolders()
						{
							NovastoryApi.revisionDirectories(function (data)
							{
								var list = $('#revisions-directory');
								list.empty();

								var bigestDate = 0;
								function parseDirectory(dir)
								{
									var html = '';
								
									if (dir.length == 0)
										return html;
										
									for (var i = 0; i < dir.length; i++)
									{
										if (dir[i].childs.length > 0 || (dir[i].type != 'REVISION' && dir[i].type != 'TEXT'))
										{
											// This is directory
											html +=
												'<li draggable="true">'
												 + '<div class="revisionid" style="display: none;">' + dir[i].revisionid + '</div>'
												 + '<label for="folder' + dir[i].revisionid + '" class="cheked">'
												 + '<div>'
												 + '<svg  id="new-project-icon" viewBox="0 0 512 512">'
												 + '<path id="full-folder-icon" d="M430,122.265v77.303h-63.119l-75.04-57.707l-25.129,32.676l32.62,25.031h-33.843l-75.041-57.707l-44.379,57.707H81V89.66h100.35l22.979,22.834c6.298,6.258,14.814,9.771,23.693,9.771H430z M462,234.528H50L74,422.34h358.583L462,234.528z"></path>'
												 + '</svg>'
												 + '</div>'
												 + '<a>' + dir[i].mark + '</a>'
												 + '</label>'
												 + '<input checked type="checkbox" id="folder' + dir[i].revisionid + '" />'
												 + '<ol>'
												 + parseDirectory(dir[i].childs)
											     + '</ol></li>';
										}
										else
										{	
											// This is final leef
											html +=
												'<li draggable="true" class="file" id="file' + dir[i].revisionid + '">'
												 + '<div class="revisionid" style="display: none;">' + dir[i].revisionid + '</div>'
												 + '<div></div>'
												 + '<a>' + dir[i].mark + '</a>'
												 + '</li>';
												 
											if (dir[i].modifyDate > bigestDate)
											{
												bigestDate = dir[i].modifyDate;
												clickedRevtreeDefault = dir[i].revisionid;
											}
										}
									}
									return html;
								}

								list.append(parseDirectory(data));

								$('#revisions-directory li').click(function ()
								{
									$('#revisions-directory li').removeClass('current');
									$('#text-block-head-event').attr('checked', false);

									clickedRevisionInTreeElement = $(this);
									clickedRevisionInTreeElement.addClass('current');
									clickedRevisionInTree = parseInt(clickedRevisionInTreeElement.children('.revisionid').text());
									if (clickedRevisionInTree <= 0)
									{
										Novastory.error("Something wrong on revision click. Contact us.");
									}
									$('#text-block').show();
									NovastoryApi.revision(clickedRevisionInTree, function (revdata)
									{
										$('#text-block-name-edit').val(revdata.mark);
										$('#text-block-name').text(revdata.mark);
										$('#text-block-description-edit').val('');
										$('#text-block-description').text('');
									}
									);
									updateFolderContent();
								}
								);

								if (typeof clickedRevtreeDefault != 'undefined' && clickedRevtreeDefault > 0)
								{
									$('#file' + clickedRevtreeDefault).click();
								}

								$('#revisions-directory').on("dragover", "li", function (event)
								{
									event.stopPropagation();
									event.preventDefault();
									event.originalEvent.dataTransfer.effectAllowed = 'move';
								}
								);

								$('#revisions-directory').on("dragstart", "li", function (event)
								{
									event.originalEvent.dataTransfer.effectAllowed = 'move';
									event.originalEvent.dataTransfer.setData('text/plain', $(this).children('.revisionid').text());
								}
								);

								movingRightTreeLocker = false;
								$('#revisions-directory').on("drop", "li", function (event)
								{
									event.stopPropagation();
									event.preventDefault();
									var revisionid = parseInt(event.originalEvent.dataTransfer.getData("text/plain"));
									var target = parseInt($(this).children('.revisionid').text());
									if (revisionid <= 0 || target <= 0)
									{
										Novastory.error("No revision to move founded");
										return;
									}

									if (!movingRightTreeLocker)
									{
										movingRightTreeLocker = true;
										NovastoryApi.moveRevision(revisionid, target, function (data)
										{
											if (data.error != null && !data.error)
											{
												Novastory.ok("Moved");
												clickedRevtreeDefault = revisionid;
												updateRevisionList();
											}
											else
											{
												movingRightTreeLocker = false;
												Novastory.error("Something wrong on revision move");
											}
										}
										);
									}
								}
								);
							}
							);
						}
						updateFolders();
					}
					updateRevisionList();

					// revision control
					$("#editor, #revision-mark").keydown(function (e)
					{
						if (e.ctrlKey && e.which === 83)
						{
							e.preventDefault();
							NovastoryApi.updateRevision($("#editor").val(), $("#revision-mark").val(), lastClickedRevision, function (data)
							{
								if (data.error != null && !data.error)
								{
									Novastory.ok("Text updated in last revision");
									updateRevisionList();
								}
								else if (data.error != null && data.error && data.errorType == 3)
								{
									Novastory.warning("Same text was saved slightly before");
								}
								else
								{
									Novastory.error("Something wrong on text save");
								}
							}
							);
						}
					}
					);

					$("#editor, #revision-mark").keydown(function (e)
					{
						if (e.ctrlKey && e.which === 82)
						{
							e.preventDefault();

							var parentRev = 0;
							if (typeof clickedRevisionInTree != 'undefined' && clickedRevisionInTree > 0)
								parentRev = clickedRevisionInTree;

							NovastoryApi.insertRevision($("#editor").val(), $("#revision-mark").val(), parentRev, function (data)
							{
								if (data.error != null && !data.error)
								{
									Novastory.ok("Text inserted in revision list");
									updateRevisionList();
								}
								else if (data.error != null && data.error && data.errorType == 3)
								{
									Novastory.warning("Same text was saved slightly before");
								}
								else
								{
									Novastory.error("Something wrong on text save");
								}
							}
							);
						}
					}
					);

					$('#editor-books-controls').load('/rp-projects.html #projects-panel', null, function ()
					{
						$('#delete-butt').click(function ()
						{
							if (clickedRevisionInTree <= 0)
							{
								Novastory.error("Something wrong on clickedRevisionInTree");
								return;
							}
							NovastoryApi.removeRevision(clickedRevisionInTree, function (data)
							{
								if (data.error != null && !data.error)
								{
									Novastory.ok("Removed");
									updateRevisionList();
									$('#text-block').hide();
								}
								else
								{
									Novastory.error("Something wrong on revision delete");
								}
							}
							);
						}
						);

						$('#publish-butt').click(function ()
						{
							if (typeof lastClickedRevision == 'undefined' || typeof lastClickedRevisionRelease == 'undefined')
							{
								Novastory.error("Please check some revision on bottom");
								return;
							}

							function helper(data)
							{
								if (data.error != null && !data.error)
								{
									updateRevisionList();
									Novastory.ok("Released");
								}
								else
								{
									Novastory.error("Something wrong on release revision");
								}
							}
							if (!lastClickedRevisionRelease)
								NovastoryApi.release(lastClickedRevision, helper);
							else
								NovastoryApi.unrelease(lastClickedRevision, helper);
						}
						);

						$('#new-text-butt').click(function ()
						{
							NovastoryApi.insertRevision($("#editor").val(), "New Text", 0, function (data)
							{
								if (data.error != null && !data.error)
								{
									Novastory.ok("New text created");
									updateRevisionList();
									clickedRevtreeDefault = data.revision.parentid;
								}
								else if (data.error != null && data.error && data.errorType == 3)
								{
									Novastory.warning("Same text was saved slightly before");
								}
								else
								{
									Novastory.error("Something wrong on text save");
								}
							}
							);
						}
						);

						$("#new-project-butt").click(function ()
						{
							NovastoryApi.newProject("New Project", function (data)
							{
								if (data.error != null && !data.error)
								{
									Novastory.ok("Project created");
									updateRevisionList();
									clickedRevtreeDefault = data.revision.revisionid;
								}
								else
								{
									Novastory.error("Something wrong on text save");
								}
							}
							);
						}
						);

						$('#text-block-head-event').change(function ()
						{
							if (!this.checked)
							{
								if (typeof clickedRevisionInTree == 'undefined' || typeof clickedRevisionInTreeElement == 'undefined')
								{
									Novastory.error("Please check some revision on bottom");
									return;
								}

								var newName = $('#text-block-name-edit').val();
								NovastoryApi.updateRevisionMark(clickedRevisionInTree, newName, function (data)
								{
									if (data.error != null && !data.error)
									{
										$('#text-block-name').text(newName);
										clickedRevisionInTreeElement.children('a').text(newName);
									}
									else
									{
										Novastory.error("Something wrong on revision update mark");
									}
								}
								);
							}
						}
						);
					}
					);
				}
				);

			} // подгрузка редактора
			);
		}
		else
		{
			switchEditor();
		}
	} // загрузка редактора

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
		if ($('#helpme-dialog').is(":not([open])"))
		{
			$('#helpme-dialog').attr('open', '');
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
				$(this).removeAttr('open');
			}
			);
		}
	}
	);
}
);
