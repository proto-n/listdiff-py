#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>

using MatrixXi = Eigen::Matrix<long, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using MatrixXiCM = Eigen::Matrix<long, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;

long get_elem(Eigen::Ref<const MatrixXi> &m) {
    return m(2, 1);
}
long get_elem2(Eigen::Ref<const MatrixXiCM> &m) {
    return m(2, 1);
}

namespace py = pybind11;


PYBIND11_MODULE(listdiff, m) {
	m.def(
		"get_elem",
		[](Eigen::Ref<const MatrixXi> &m) -> long {
			return get_elem(m);
		},
		py::arg().noconvert()
	);
	m.def(
		"get_elem2",
		[](Eigen::Ref<const MatrixXiCM> &m) -> long {
			return get_elem2(m);
		},
		py::arg().noconvert()
	);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}