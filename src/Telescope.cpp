#include "TelescopeManager.h"

using namespace std;

int main(int argc, char* argv[]) {

	cout << "Telescope version " << T_VERSION << endl;
	if (argc != 2) {
		cout << "USAGE: Telescope <params file>" << endl;
		return 0;
	}
	string params = argv[1];
	TelescopeManager tm;
	if (!tm.SetParams(params)) {
		cout << "Failed to set params: " << params << endl;
		return 1;
	}
	int ret = tm.Launch(true);
	if (ret != 0) {
		cout << "Telescope failed with error code: " << ret << endl;
		return ret;
	}	else cout << "Telescope completed successfully." << endl;
	return 0;

}
