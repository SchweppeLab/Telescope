#include "TelescopeManager.h"

using namespace std;

int main(int argc, char* argv[]) {

	string params = argv[1];
	TelescopeManager tm;
	if (!tm.SetParams(params)) {
		cout << "Failed to set params: " << params << endl;
		return 1;
	}
	cout << tm.Launch(true) << endl;
	return 0;

}
