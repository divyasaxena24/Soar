#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include "model.h"
#include "soar_interface.h"

using namespace std;

class velocity_model : public model {
public:
	velocity_model(int dims) : dims(dims) {}

	bool predict(const floatvec &x, floatvec &y) {
		if (x.size() != dims * 2 || y.size() != dims) {
			return false;
		}

		cout << "PREDICTION";
		for (int i = 0; i < dims; ++i) {
			y[i] = x[i] + x[i + dims];
			cout << " " << y[i];
		}
		cout << endl;
		return true;
	}
	
	std::string get_type() const {
		return "velocity";
	}
	
	int get_input_size() const {
		return dims * 2;  // current + velocity in each dimension
	}
	
	int get_output_size() const {
		return dims;
	}
	
private:
	int dims;
};

model *_make_velocity_model_(soar_interface *si, Symbol *root) {
	long dims = 1;
	wme_list children;
	wme_list::iterator i;
	si->get_child_wmes(root, children);
	for (i = children.begin(); i != children.end(); ++i) {
		string attr;
		if (si->get_val(si->get_wme_attr(*i), attr) && attr == "dims") {
			if (si->get_val(si->get_wme_val(*i), dims)) {
				break;
			}
		}
	}
	return new velocity_model(dims);
}
