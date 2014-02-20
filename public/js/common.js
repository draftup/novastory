function includeJS(js, callback)
{
	var file = document.createElement('script');
  	file.setAttribute("type","text/javascript");
  	file.setAttribute("src", js);
  	file.onreadystatechange = callback;
  	file.onload = callback;
	var parent = document.getElementsByTagName("head")[0];
	parent.appendChild(file);    
}
