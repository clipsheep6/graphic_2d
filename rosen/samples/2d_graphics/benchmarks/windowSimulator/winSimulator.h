#ifndef WINSIMULATOR_H
#define WINSIMULATOR_H
#include "include/core/SkCanvas.h"
class WinSimulator {
public:
	WinSimulator() {}
	int setWindowInfo(int width, int height);
	int onPaint(SkCanvas *canvas);
	int onFlush();
	void setMode(int mode, int wins);
	void useMultiThread(int multiThread);
	float getPaintTime();
private:
	int mWindowWidth;
	int mWindowHeight;
	int mDisplayMode;
	int mSubWindows;
	int mMultiThread = 0;
	float mFrameTime_paint = 0;
	double mDrawTime = 0;
	int mFrameCount = 0;
};
#endif
