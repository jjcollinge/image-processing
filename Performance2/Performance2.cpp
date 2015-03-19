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
using namespace cv;

void __fastcall processImage(string imagename) {
	/*
		Load image from file
		---------------------------
		Desc:
	*/
	Mat img = imread(imagename + ".JPG", CV_LOAD_IMAGE_COLOR);
	
	/*
		Resize the image to half of the original
		---------------------------
		Desc:
	*/
	Size size(img.cols/2, img.rows/2);
	resize(img, img, size); //INTER_LINEAR by default

	/*
		Convert the colour of the image to greyscale
		---------------------------
		Desc:
	*/
	cvtColor(img, img, CV_RGB2GRAY);
	
	/*
		Brighten the image
		---------------------------
		Desc:
	*/
	img = img + cv::Scalar(10, 10, 10);
	
	/*
		Rotate the image 90 degrees clockwise
		---------------------------
		Desc:
	*/
	transpose(img, img);
	flip(img, img, 1);
	
	/*
		Write the image to file
		---------------------------
		Desc:
	*/
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

		threads.push_back(thread(processImage, "IMG_1"));
		threads.push_back(thread(processImage, "IMG_2"));
		threads.push_back(thread(processImage, "IMG_3"));
		threads.push_back(thread(processImage, "IMG_4"));
		threads.push_back(thread(processImage, "IMG_5"));
		threads.push_back(thread(processImage, "IMG_6"));
		threads.push_back(thread(processImage, "IMG_7"));
		threads.push_back(thread(processImage, "IMG_8"));
		threads.push_back(thread(processImage, "IMG_9"));
		threads.push_back(thread(processImage, "IMG_10"));
		threads.push_back(thread(processImage, "IMG_11"));
		threads.push_back(thread(processImage, "IMG_12"));

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
