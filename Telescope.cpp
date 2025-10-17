#include "TelescopeManager.h"

using namespace std;

int main(int argc, char* argv[]) {

	string params = argv[1];
	TelescopeManager tm;
	if (!tm.SetParams(params)) {
		cout << "Failed to set params: " << params << endl;
		return 1;
	}
	int ret = tm.Launch(true);
	if (ret != 0) cout << "Telescope failed with error code: " << ret << endl;
	else cout << "Telescope completed successfully." << endl;
	return 0;

}
