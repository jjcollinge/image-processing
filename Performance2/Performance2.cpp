// Performance2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Performance2.h"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <thread>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Timer - used to established precise timings for code.
class TIMER
{
	LARGE_INTEGER t_;

	__int64 current_time_;

	public:
		TIMER()	// Default constructor. Initialises this timer with the current value of the hi-res CPU timer.
		{
			QueryPerformanceCounter(&t_);
			current_time_ = t_.QuadPart;
		}

		TIMER(const TIMER &ct)	// Copy constructor.
		{
			current_time_ = ct.current_time_;
		}

		TIMER& operator=(const TIMER &ct)	// Copy assignment.
		{
			current_time_ = ct.current_time_;
			return *this;
		}

		TIMER& operator=(const __int64 &n)	// Overloaded copy assignment.
		{
			current_time_ = n;
			return *this;
		}

		~TIMER() {}		// Destructor.

		static __int64 get_frequency()
		{
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency); 
			return frequency.QuadPart;
		}

		__int64 get_time() const
		{
			return current_time_;
		}

		void get_current_time()
		{
			QueryPerformanceCounter(&t_);
			current_time_ = t_.QuadPart;
		}

		inline bool operator==(const TIMER &ct) const
		{
			return current_time_ == ct.current_time_;
		}

		inline bool operator!=(const TIMER &ct) const
		{
			return current_time_ != ct.current_time_;
		}

		__int64 operator-(const TIMER &ct) const		// Subtract a TIMER from this one - return the result.
		{
			return current_time_ - ct.current_time_;
		}

		inline bool operator>(const TIMER &ct) const
		{
			return current_time_ > ct.current_time_;
		}

		inline bool operator<(const TIMER &ct) const
		{
			return current_time_ < ct.current_time_;
		}

		inline bool operator<=(const TIMER &ct) const
		{
			return current_time_ <= ct.current_time_;
		}

		inline bool operator>=(const TIMER &ct) const
		{
			return current_time_ >= ct.current_time_;
		}
};

// The one and only application object

CWinApp theApp;

using namespace std;


CImage* Copy(CImage *source)
{
	CImage *dest = new CImage;
	dest -> Create(source -> GetWidth(), source -> GetHeight(), source -> GetBPP());
	source -> Draw(dest -> GetDC(), 0, 0, dest -> GetWidth(), dest -> GetHeight(), 0, 0, source -> GetWidth(), source -> GetHeight());
	dest -> ReleaseDC();
	return dest;
}

CImage* Brighten(CImage *i)
{
	CImage *dest = Copy(i);

	int width = dest -> GetWidth();
	int height = dest -> GetHeight();

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			COLORREF pixel = dest -> GetPixel(x, y);

			BYTE r = GetRValue(pixel);
			BYTE g = GetGValue(pixel);
			BYTE b = GetBValue(pixel);

			if ((r + 10) > 255) r = 255; else r += 10;
			if ((g + 10) > 255) g = 255; else g += 10;
			if ((b + 10) > 255) b = 255; else b += 10;

			pixel = RGB(r, g, b);

			dest -> SetPixel(x, y, pixel);
		}
	}

	return dest;
}

using namespace std;
using namespace cv;

void processImage(string imagename) {
	Mat img = imread(imagename + ".JPG", CV_LOAD_IMAGE_COLOR);

	int height = img.rows / 2;
	int width = img.cols / 2;

	Size size(width / 2, height / 2);
	// resize
	resize(img, img, size);
	// greyscale
	cvtColor(img, img, CV_RGB2GRAY);
	// brighten
	img = img + cv::Scalar(10, 10, 10);
	// rotate
	transpose(img, img);
	flip(img, img, 1);
	// save
	imwrite(imagename + ".PNG", img);
}

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// initialize Microsoft Foundation Classes, and print an error if failure
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		_tprintf(_T("Fatal Error: MFC initialization failed\n"));
		nRetCode = 1;
	}
	else
	{
		// Application starts here...

		// Time the application's execution time.
		TIMER start;	// DO NOT CHANGE THIS LINE

		// Process the images...   // Put your code here...
		vector<thread> threads;

		for (int i(1); i <= 12; ++i) {
			threads.push_back(thread(processImage, "IMG_"+to_string(i)));
		}

		for (auto& thread : threads){
			thread.join();
		}

		// How long did it take?...   DO NOT CHANGE FROM HERE...
		
		TIMER end;

		TIMER elapsed;
		
		elapsed = end - start;

		__int64 ticks_per_second = start.get_frequency();

		// Display the resulting time...

		double elapsed_seconds = (double)elapsed.get_time() / (double)ticks_per_second;

		cout << "Elapsed time (seconds): " << elapsed_seconds;
		cout << endl;
		cout << "Press a key to continue" << endl;

		char c;
		cin >> c;
	}

	return nRetCode;
}
