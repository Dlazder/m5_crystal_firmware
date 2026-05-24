void recreateCanvas() {
	canvas.deleteSprite();
	canvas.createSprite(DISP.width(), DISP.height() - getStatusBarHeight());
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(MEDIUM_TEXT);
}
