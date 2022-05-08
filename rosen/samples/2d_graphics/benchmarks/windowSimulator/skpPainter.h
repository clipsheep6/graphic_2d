#ifndef SKPPAINTER_H
#define SKPPAINTER_H
#include <thread>
#include <string>
#include <iostream>
#include "include/core/SkCanvas.h"
#include "include/core/SkPicture.h"

class SkCanvas;
class SkPictureRecorder;
class SkNWayCanvas;
class WinSimulator;
class skpPainter {
public:
	skpPainter(std::string file);
	int setPath(std::string file);
	int loadSkp(std::string file);
	int onPaint(SkCanvas* canvas, int letf, int top, int width, int height);
	int onFlush();
	void wait4Paint();
	void enableThread(int enable);
	float getPaintTime();
private:
	int doPaint(SkCanvas* canvas, int letf, int top, int width, int height);
private:
	std::string mPath;
	sk_sp<SkPicture> mPic;
	int mUseThread;
	std::unique_ptr<std::thread> mThread = nullptr;
	float mPaintTime;
};
#endif