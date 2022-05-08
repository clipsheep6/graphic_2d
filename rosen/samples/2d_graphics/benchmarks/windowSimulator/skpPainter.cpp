#include <chrono>
#include "skpPainter.h"
#include "include/core/SkPictureRecorder.h"
#include "include/core/SkPicture.h"
#include "include/core/SkStream.h"

using namespace std::chrono;
skpPainter::skpPainter(std::string file)
	:mPath(file)
{
	mUseThread = 0;
}

int skpPainter::setPath(std::string file) {
	mPath = file;
	return 0;
}

int skpPainter::loadSkp(std::string file) {
	SkFILEStream input(mPath.c_str());
	if (!input.isValid()) {
		SkDebugf("Bad file path = %s", mPath.c_str());
		return -1;
	}
	mPic = SkPicture::MakeFromStream(&input);
	if (!mPic) {
		SkDebugf("Bad skp: load skp fail!");
		return -1;
	}
	return 0;
}

int skpPainter::onPaint(SkCanvas* canvas, int letf, int top, int width, int height) {
	if (mUseThread == 1) {
		if (mThread == nullptr) {
        		mThread = std::make_unique<std::thread>(&skpPainter::doPaint, this, canvas, letf, top, width, height);
    		}
	} else {
		doPaint(canvas, letf, top, width, height);
	}
	return 0;
}

int skpPainter::onFlush() {
	return 0;
}

void skpPainter::wait4Paint() {
	if (mUseThread == 1) {
		if (mThread != nullptr && mThread->joinable()) {
        		mThread->join();
   		}
		mThread = nullptr;
	}
}

int skpPainter::doPaint(SkCanvas * canvas, int letf, int top, int width, int height) {
	auto start = system_clock::now();

	SkRect skpWin = mPic->cullRect();
	int picWidth = skpWin.fRight - skpWin.fLeft;
	//int picHeight = skpWin.fButtom - skpWin.fTop;
	float scaleRatioX = 0;
	float scaleRatioY = 0;
	scaleRatioX = (float)width / picWidth;
	scaleRatioY = scaleRatioX;

	canvas->save();
	canvas->translate(letf, top);
	canvas->scale(SkFloatToScalar(scaleRatioX), SkFloatToScalar(scaleRatioY));
	canvas->drawPicture(mPic);
	canvas->restore();

	auto end = system_clock::now();
	auto duration = duration_cast<nanoseconds>(end - start);
	mPaintTime = duration.count() / 1000000.0;
	std::cout << mPath.c_str()<<":current draw time = "<<mPaintTime <<"ms"<<std::endl;

	return 0;
	
}

void skpPainter::enableThread(int enable) {
	mUseThread = enable;
}

float skpPainter::getPaintTime() {
	return mPaintTime;
}
