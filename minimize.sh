minimized_html=$(tr -d '\r\n\t' < './src/web/index.html' | sed 's/"/\\"/g')
printf "String mainHTML() {\n\tString html = \"%s\";\n\treturn html;\n}\n\n" "$minimized_html" > './src/utils/webServerUtils.h'

minimized_css=$(tr -d '\r\n\t' < './src/web/style.css' | sed 's/"/\\"/g')
printf "String mainCSS() {\n\tString css = \"%s\";\n\treturn css;\n}\n\n" "$minimized_css" >> './src/utils/webServerUtils.h'

minimized_js=$(tr -d '\r\n\t' < './src/web/index.js' | sed 's/"/\\"/g')
printf "String mainJS() {\n\tString js = \"%s\";\n\treturn js;\n}\n\n" "$minimized_js" >> './src/utils/webServerUtils.h'

# Evil portal — login page
minimized_ep=$(tr -d '\r\n\t' < './src/web/evil_portal/index.html' | sed 's/"/\\"/g')
printf "String evilPortalHTML() {\n\tString html = \"%s\";\n\treturn html;\n}\n\n" "$minimized_ep" >> './src/utils/webServerUtils.h'

# Evil portal — success page
minimized_eps=$(tr -d '\r\n\t' < './src/web/evil_portal/success.html' | sed 's/"/\\"/g')
printf "String evilPortalSuccessHTML() {\n\tString html = \"%s\";\n\treturn html;\n}\n\n" "$minimized_eps" >> './src/utils/webServerUtils.h'

# LFS Web UI
minimized_lfs_ui=$(tr -d '\r\n\t' < './src/web/lfs_web_ui/lfs_web_ui.html' | sed 's/"/\\"/g')
printf "String lfsWebUIHTML() {\n\tString html = \"%s\";\n\treturn html;\n}\n\n" "$minimized_lfs_ui" >> './src/utils/webServerUtils.h'

# LFS Web UI
minimized_lfs_css=$(tr -d '\r\n\t' < './src/web/lfs_web_ui/lfs_web_ui.css' | sed 's/"/\\"/g')
printf "String lfsWebUICSS() {\n\tString css = \"%s\";\n\treturn css;\n}\n\n" "$minimized_lfs_css" >> './src/utils/webServerUtils.h'

echo done
