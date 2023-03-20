#include "util.h"

void SimpleMessageBox(string text, string caption) {
	MessageBox(nullptr, text.c_str(), caption.c_str(), MB_OK);
}

void HandleExceptionOffline(exception& e, const string& exTypeName) {
	SimpleMessageBox(e.what(), "Exception!");
	LogWriter().WriteErrLog(e, exTypeName);
	exit(0);
}

void HandleExceptionOnline(exception& e, const string& exTypeName) {
	system("pause");
	exit(0);
}