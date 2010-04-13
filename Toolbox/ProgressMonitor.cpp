#include <iostream>
#include <iomanip>
#include <signal.h>
#include "ProgressMonitor.h"
#include "IOHelper.h"

using namespace std;

bool ProgressMonitor::bAbort = false;

void ProgressMonitor::CatchSignal(int sig)
{
	bAbort = true;
}

ProgressMonitor::ProgressMonitor(const unsigned long nPos)
{
	signal(SIGINT, CatchSignal);
	this->nPos = nPos;
	bShow = isatty(1);
	Reset();
}

ProgressMonitor::~ProgressMonitor()
{
	if (bShow)
		cout << "\r";
	cout << "Status: " << *this << endl;
}

bool ProgressMonitor::Update()
{
	return Update(++(this->cPos));
}

bool ProgressMonitor::Update(const unsigned long cPos)
{
	this->cPos = cPos;
	if (bShow && (this->cPos % 1000 == 0))
	{
		cout << "\rStatus: ";
		cout << *this;
		cout.flush();
	}
	return !bAbort;
}

void ProgressMonitor::IncMax(const unsigned long nPos)
{
	this->nPos += nPos;
	Reset();
}

void ProgressMonitor::Reset()
{
	cPos = 0;
	gettimeofday(&tStartTime, 0);
}

struct SecTime
{
	SecTime(const long _sec) : sec(_sec) {}
	long sec;
};

ostream &operator<<(ostream &os, const SecTime &st)
{
	return os
		<< setw(2) << setfill('0') << st.sec / 3600 << ":"
		<< setw(2) << setfill('0') << (st.sec / 60) % 60 << ":"
		<< setw(2) << setfill('0') << st.sec % 60;
}

ostream &operator<<(ostream &os, const ProgressMonitor &pm)
{
	OStreamGuard guard(os);
	struct timeval now;
	gettimeofday(&now, 0);
	const double realSpeed = pm.cPos / (double)(now.tv_sec - pm.tStartTime.tv_sec + 1.0e-6 * (now.tv_usec - pm.tStartTime.tv_usec));
	const double calcSpeed = (realSpeed < 1) ? 1 : realSpeed;
	const unsigned long sec = (unsigned long)((pm.nPos - pm.cPos) / calcSpeed);
	os << pm.cPos << "/" << pm.nPos << " - "
		<< SecTime(sec) << "/" << SecTime((int)(pm.nPos / calcSpeed)) << " - "
		<< (int)realSpeed << " / s" << " - ";
	if (pm.cPos < (unsigned int)(0.99 * pm.nPos))
		os << setw(3) << setfill(' ') << setprecision(2) << 100.0 * pm.cPos / pm.nPos << "%    ";
	else
		os << "100%    ";
	return os;
}