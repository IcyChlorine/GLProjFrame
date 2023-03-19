#pragma once
#ifndef _UTIL_H
#define _UTIL_H

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <exception>
#include <stdexcept>
#include <Windows.h>
#include <ctime>
using namespace std;

class AbsObject {
protected:
	AbsObject* father{ nullptr };
public:
	AbsObject() {}
	AbsObject(void* father) { this->father = (AbsObject*)father; }
	AbsObject(AbsObject* father) { this->father = father; }
	void setFather(AbsObject* father_new) { father = father_new; }
	AbsObject* getFather() { return father; }

	virtual void outputDebugInfo(ostream& out) {}
};

class init_exception :public exception {
public:
	init_exception() {}
	init_exception(const char* message) :
		exception(message) {}
};
/*
	other useful & std-defined exception classes:
		runtime_error
		invalid_argument
*/

class LogWriter {
private:
	//把tm里的表示改得阳间一点
	//发现了新的骚操作...这里舍不得删 就留着吧
	//@Deprecated
	void _adjust_tm(struct tm& gmtm) {
		gmtm.tm_year += 1900;
			//转换时还得考虑进位问题...头大
		gmtm.tm_hour += 8;
		int g = gmtm.tm_hour / 24;
		gmtm.tm_hour %= 24;
			int leap = 0;
		if (!(gmtm.tm_year % 4)) {
			leap = 1;
			if (!(gmtm.tm_yday % 100)) {
				leap = 0;
				if (!(gmtm.tm_yday % 400)) {
					leap = 1;
				}
			}
		}
		int total_day_in_month[2][12] = {
			{ 31,28,31,30,31,30,31,31,30,31,30,31 },
			{ 31,29,31,30,31,30,31,31,30,31,30,31 },
		};
		int total_day_in_year[2] = { 365,366 };
			gmtm.tm_mday += g;
		g = gmtm.tm_mday > total_day_in_month[leap][gmtm.tm_mon];
		if (g) {
			gmtm.tm_mday = 1;
		}
		gmtm.tm_wday += g;
		gmtm.tm_yday += g;
	}
public:
		static void WriteErrLog(const exception& e, const string& exTypeName = "exception") {
		time_t t = time(nullptr) + 8 * 3600;//换算时区
		struct tm gmtm;
		gmtime_s(&gmtm, &t);
		char err_log_filename[40];
		sprintf_s(err_log_filename, 40u, "err_log %d-%d-%d %dh%dm%ds.log",
			gmtm.tm_year + 1900,
			gmtm.tm_mon + 1,
			gmtm.tm_mday,
			gmtm.tm_hour,
			gmtm.tm_min,
			gmtm.tm_sec);
			ofstream fout(err_log_filename);
		fout << "Program crashed at " << gmtm.tm_hour << ":" << gmtm.tm_min << ":" << gmtm.tm_sec << " due to the following exception:" << endl;
		fout << "\t" << exTypeName.c_str() << ": " << e.what() << endl;
		fout.close();
	}
};
void SimpleMessageBox(string text, string caption);
void HandleException(exception& e, const string& exTypeName = "exception");

#endif