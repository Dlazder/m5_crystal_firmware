#pragma once

// Takes a BMP screenshot of the full display and saves it to SD card.
// Returns the file path on success, or error message on failure.
void screenshotCommand() {
#if !HAS_SD
    Serial.println("Error: no SD card support");
    return;
#else
    if (!sdBegin()) {
        Serial.println("Error: SD init failed");
        return;
    }

    // Find a free filename: /screenshot_0001.bmp ... /screenshot_9999.bmp
    char filePath[32];
    for (int i = 1; i <= 9999; i++) {
        snprintf(filePath, sizeof(filePath), "/screenshot_%04d.bmp", i);
        if (!SD.exists(filePath)) break;
    }

    int w = DISP.width();
    int h = DISP.height();

    // BMP file layout: 14-byte file header + 40-byte DIB header + pixel data (BGR, row-padded to 4 bytes)
    int rowSize = (w * 3 + 3) & ~3;
    int pixelDataSize = rowSize * h;
    int fileSize = 54 + pixelDataSize;

    File f = SD.open(filePath, FILE_WRITE);
    if (!f) return;

    // File header (14 bytes)
    uint8_t header[54] = {};
    header[0] = 'B'; header[1] = 'M';
    header[2] = fileSize & 0xFF;
    header[3] = (fileSize >> 8) & 0xFF;
    header[4] = (fileSize >> 16) & 0xFF;
    header[5] = (fileSize >> 24) & 0xFF;
    // reserved: bytes 6-9 = 0
    header[10] = 54; // pixel data offset
    // DIB header (40 bytes starting at offset 14)
    header[14] = 40; // header size
    header[18] = w & 0xFF;
    header[19] = (w >> 8) & 0xFF;
    header[20] = (w >> 16) & 0xFF;
    header[21] = (w >> 24) & 0xFF;
    // BMP rows are bottom-up: store negative height to indicate top-down
    int negH = -h;
    header[22] = negH & 0xFF;
    header[23] = (negH >> 8) & 0xFF;
    header[24] = (negH >> 16) & 0xFF;
    header[25] = (negH >> 24) & 0xFF;
    header[26] = 1;  // color planes
    header[28] = 24; // bits per pixel
    // compression = 0 (none), remaining fields = 0
    f.write(header, 54);

    // Read and write pixel data row by row
    uint8_t rowBuf[rowSize];
    memset(rowBuf, 0, rowSize);
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            uint16_t px = DISP.readPixel(x, y); // RGB565
            uint8_t r = ((px >> 11) & 0x1F) << 3;
            uint8_t g = ((px >> 5)  & 0x3F) << 2;
            uint8_t b = ( px        & 0x1F) << 3;
            rowBuf[x * 3 + 0] = b;
            rowBuf[x * 3 + 1] = g;
            rowBuf[x * 3 + 2] = r;
        }
        f.write(rowBuf, rowSize);
    }

    f.close();
    Serial.println("Screenshot saved: " + String(filePath));
#endif
}
