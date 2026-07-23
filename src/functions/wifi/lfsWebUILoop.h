// pid PID::LFS_WEB_UI

#include <LittleFS.h>

static const char* LFS_WEB_UI_SSID = "M5 Crystal LFS";
static IPAddress LFS_WEB_UI_GATEWAY(172, 0, 0, 1);
static IPAddress LFS_WEB_UI_SUBNET(255, 255, 255, 0);

static String _lfsWebUIGenerateFileList() {
    if (!lfsBegin()) return "<li class=\"file-empty\">LittleFS mount failed</li>";

    String list = "";
    File root = LittleFS.open("/");
    if (root) {
        File f = root.openNextFile();
        bool hasFiles = false;
        while (f) {
            String name = String(f.name());
            if (!name.startsWith("/")) name = "/" + name;
            if (!f.isDirectory()) {
                hasFiles = true;
                list += "<li>";
                list += "<a class=\"file-link\" href=\"/download?path=" + name + "\">" + name + "</a>";
                list += "<span class=\"file-meta\">";
                list += "<span class=\"file-size\">" + String(f.size()) + " B</span>";
                list += "<a class=\"btn btn-danger\" href=\"/delete?path=" + name
                    + "\" onclick=\"return confirm('Delete " + name + "?')\">Delete</a>";
                list += "</span>";
                list += "</li>";
            }
            f = root.openNextFile();
        }
        if (!hasFiles) {
            list += "<li class=\"file-empty\">No files yet</li>";
        }
        root.close();
    }
    return list;
}

static void _lfsWebUIRegisterHandlers() {
    // ---- Serve CSS ----
    webServer.on("/lfs_web_ui.css", []() {
        webServer.send(200, "text/css", lfsWebUICSS());
    });

    // ---- Main page ----
    webServer.on("/", []() {
        String html = lfsWebUIHTML();
        html.replace("FILE_LIST_PLACEHOLDER", _lfsWebUIGenerateFileList());
        webServer.send(200, "text/html", html);
    });

    // ---- Download ----
    webServer.on("/download", []() {
        if (!lfsBegin()) {
            webServer.send(500, "text/plain", "LittleFS mount failed");
            return;
        }

        String path = webServer.arg("path");
        if (path.length() == 0 || !path.startsWith("/")) {
            webServer.send(400, "text/plain", "Invalid path");
            return;
        }

        if (!LittleFS.exists(path)) {
            webServer.send(404, "text/plain", "File not found: " + path);
            return;
        }

        File f = LittleFS.open(path, "r");
        if (!f) {
            webServer.send(500, "text/plain", "Failed to open: " + path);
            return;
        }

        webServer.streamFile(f, getContentType(path));
        f.close();
    });

    // ---- Delete ----
    webServer.on("/delete", []() {
        if (!lfsBegin()) {
            webServer.send(500, "text/plain", "LittleFS mount failed");
            return;
        }

        String path = webServer.arg("path");
        if (path.length() == 0 || !path.startsWith("/")) {
            webServer.send(400, "text/plain", "Invalid path");
            return;
        }

        if (LittleFS.remove(path)) {
            webServer.sendHeader("Location", "/", true);
            webServer.send(302, "text/plain", "");
        } else {
            webServer.send(500, "text/plain", "Failed to delete: " + path);
        }
    });

    // ---- Upload (POST + multipart) ----
    webServer.on("/upload", HTTP_POST,
        []() {
            webServer.sendHeader("Location", "/", true);
            webServer.send(302, "text/plain", "");
        },
        []() {
            HTTPUpload& upload = webServer.upload();
            static File uploadFile;

            if (upload.status == UPLOAD_FILE_START) {
                if (!lfsBegin()) return;

                String filename = upload.filename;
                if (!filename.startsWith("/")) filename = "/" + filename;

                if (uploadFile) uploadFile.close();

                uploadFile = LittleFS.open(filename, FILE_WRITE);
                if (!uploadFile) {
                    Serial.println("LFS WEB UI: failed to open " + filename + " for writing");
                }
            } else if (upload.status == UPLOAD_FILE_WRITE) {
                if (uploadFile) {
                    uploadFile.write(upload.buf, upload.currentSize);
                }
            } else if (upload.status == UPLOAD_FILE_END) {
                if (uploadFile) {
                    uploadFile.close();
                    Serial.println("LFS WEB UI: uploaded " + String(upload.totalSize) + " bytes");
                }
            } else if (upload.status == UPLOAD_FILE_ABORTED) {
                if (uploadFile) {
                    uploadFile.close();
                }
            }
        });
}

void lfsWebUILoop() {
    static bool webServerDone = false;

    if (isSetup()) {
        webServerDone = false;
    }

    if (!webServerDone) {
        WiFi.mode(WIFI_AP);
        WiFi.softAP(LFS_WEB_UI_SSID);
        WiFi.softAPConfig(LFS_WEB_UI_GATEWAY, LFS_WEB_UI_GATEWAY, LFS_WEB_UI_SUBNET);

        _lfsWebUIRegisterHandlers();

        webServer.begin();
        webServerDone = true;
        isWebInterfaceEnabled = true;

        IPAddress ip = WiFi.softAPIP();
        String lines[] = {
            "LFS WEB UI",
            "SSID: " + String(LFS_WEB_UI_SSID),
            "IP: " + ip.toString()
        };
        centeredPrintRows(lines, 3, SMALL_TEXT);
    }

    webServer.handleClient();

    if (checkExit()) {
        webServerDone = false;
    }
}
