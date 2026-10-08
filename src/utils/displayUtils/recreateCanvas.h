void recreateCanvas() {
	canvas.deleteSprite();
	canvas.setColorDepth(canvasColorDepth); // keep the buffer at half size (RGB332)
	canvas.createSprite(DISP.width(), DISP.height() - getStatusBarHeight());
	canvas.setTextColor(FGCOLOR, BGCOLOR);
	canvas.setTextSize(MEDIUM_TEXT);
}
