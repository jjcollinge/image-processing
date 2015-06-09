// Performance2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Performance2.h"
//#include "opencv2/highgui/highgui.hpp"
#include <opencv2/opencv.hpp>
//#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <thread>
#include <vector>
#include <ppl.h>
#include <mutex>

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

/*
	Resize the image to 50% of the original size.
	Use bilinear interp to resample for a better result.
*/
inline void resize(Mat& img) {
	Size size(img.cols / 2, img.rows / 2);	// Define the desired size
	resize(img, img, size);					// Perform the resize (Note: INTER_LINEAR (bilinear interp) by default)
}

/*
	Brighten the image by +10 across all channels.
*/
inline void brighten(Mat& img) {
	img += 10; // Add 10 to single channel greyscale image
	//img = img cv::Scalar(10, 10, 10); // Add a scalar value of 10 to each colour (RGB) channel
}

/*
	Rotate the image by 90 degrees clockwise.
*/
inline void rotate(Mat& img) {
	transpose(img, img); // Perform a transpose matrix transformation
	flip(img, img, 1); // Reverse the image pixels horizontally
}

/*
	Convert the image to greyscale
*/
inline void greyscale(Mat& img) {
	cvtColor(img, img, COLOR_RGB2GRAY);
}

/*
	Function to wrap desired image processing procedures.
	This function will be executed in its own thread.
	Performs the tasks in a particular order to optimise
	performance.
		1. Load the image
		(2 or 3). Resize the image to make subsequent transformations faster
		(2 or 3). Convert to greyscale to reduce to a single channel
		4. Brighten the image
		5. Rotate the image
		6. Save the image
*/
void __fastcall processImage(const string& path) {
	
	// Load the image
	Mat image = imread(path + ".JPG", IMREAD_GRAYSCALE); // use CV_LOAD_IMAGE_COLOR for full colour profile

	// Perform image transformations...
	resize(image);
	//greyscale(img); // Not needed as image RGB not loaded initially
	brighten(image);
	rotate(image);

	// Save the image
	imwrite(path + ".PNG", image);	// Write image to file
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

		/*
			NOTE:
			===============================================

			This program uses the OpenCV 3.0 beta library.
			Source: http://opencv.org/

			Please run in Release x64 configuration to use
			the correct libs. The 'GPU Only' configuration
			appears to perform fastest on my pc and the uni
			pcs. Please select the most appropriate for your
			machine
			
			Run once prior to testing to allow symbols 
			to be loaded from DLLs.

			Application may print 'Failed to load OpenCL'
			This is handled by the library and will
			continue to work as expected

		*/

		/* Hold a reference to each thread in a vector to
		ensure they are joined before termination */
		vector<thread> threads;
		const int NUM_IMAGES = 12;
		const int FIRST_IMAGE_NUM = 1;

		// Sequentially loop through the image numbers
		for (int i(FIRST_IMAGE_NUM); i <= NUM_IMAGES; ++i) {
			// Compose image filename (without extension)
			const string path = "IMG_" + to_string(i);
			/* Create and store a new thread to process this
			   Image independently of the master thread */
			threads.push_back(thread(processImage, path));
		};

		// Join the threads to master thread
		for (auto& thread : threads) {
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
