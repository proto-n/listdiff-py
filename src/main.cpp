#include <pybind11/pybind11.h>
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include <Eigen/Dense>
#include <unordered_map>
#include <map>
#include <set>
#include <vector>

using MatrixXi = Eigen::Matrix<long, Eigen::Dynamic, Eigen::Dynamic, Eigen::ColMajor>;
using VectorXi = Eigen::Matrix<long, Eigen::Dynamic, 1>;
using MatrixXiRM = Eigen::Matrix<long, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using namespace std;

const Eigen::Ref<const MatrixXi> default_mat = MatrixXi(0,3);
const Eigen::Ref<const MatrixXi> default_vec = VectorXi(0);

MatrixXiRM listdiff(
	const Eigen::Ref<const MatrixXi> &list_to_filter,
	const Eigen::Ref<const MatrixXi> &ground_truth,
	const Eigen::Ref<const MatrixXi> &complement = default_mat,
	const Eigen::Ref<const VectorXi> &extra_pids = default_vec,
	int cut_to_size = 0
	) {
	assert(list_to_filter.cols() == 3);
	assert(ground_truth.cols() == 3);
	assert(complement.cols() == 3);

	unordered_map<long, unordered_set<long>> ground_truth_map;
	for(uint i=0; i<ground_truth.rows(); i++){
		long pid = ground_truth(i,0);
		long song_id = ground_truth(i,1);
		ground_truth_map[pid].insert(song_id);
	}

	unordered_map<long, map<long, long>> filtered_lists;
	unordered_map<long, unordered_set<long>> filtered_lists_sets;
	for(uint i=0; i<list_to_filter.rows(); i++){
		long pid = list_to_filter(i,0);
		long song_id = list_to_filter(i,1);
		long pos = list_to_filter(i,2);
		auto gtset_it = ground_truth_map.find(pid);
		if(gtset_it == ground_truth_map.end() || gtset_it->second.find(song_id) == gtset_it->second.end()){
			filtered_lists[pid][pos] = song_id;
			filtered_lists_sets[pid].insert(song_id);
		}
	}
	for(uint i=0; i<extra_pids.rows(); i++){
		long pid = extra_pids(i);
		if(filtered_lists.find(pid) == filtered_lists.end()){
			filtered_lists[pid] = map<long, long>();
			filtered_lists_sets[pid] = unordered_set<long>();
		}
	}

	unordered_map<long, vector<long>> complement_vectors;
	for(uint i=0; i<complement.rows(); i++){
		long pid = complement(i,0);
		long song_id = complement(i,1);
		long pos = complement(i,2);
		vector<long> &comp_vec = complement_vectors[pid];
		if((long)comp_vec.size() < pos){
			comp_vec.resize(pos);
		}
		comp_vec[pos-1] = song_id;
	}
	vector<long> output_collector;
	for(auto pid_it = filtered_lists.begin(); pid_it != filtered_lists.end(); ++pid_it){
		long next_pos = 1;
		for(auto song_it = pid_it->second.begin(); song_it != pid_it->second.end(); ++song_it){
			output_collector.push_back(pid_it->first);
			output_collector.push_back(song_it->second);
			output_collector.push_back(next_pos);
			next_pos++;
			if(cut_to_size != 0 && next_pos > cut_to_size){
				break;
			}
		}

		if(complement.rows() != 0 && (cut_to_size == 0 || next_pos <= cut_to_size)){
			auto comp_ref = complement_vectors.find(pid_it->first);
			if(comp_ref == complement_vectors.end()){
				comp_ref = complement_vectors.find(-1);
			}
			if(comp_ref == complement_vectors.end()){
				continue;
			}
			auto gtset = ground_truth_map.find(pid_it->first);
			auto flset = filtered_lists_sets.find(pid_it->first);
			for(auto comp_it = comp_ref->second.begin(); comp_it != comp_ref->second.end(); ++comp_it){
				bool in_gt = gtset != ground_truth_map.end() && gtset->second.find(*comp_it) != gtset->second.end();
				bool in_list = flset != ground_truth_map.end() && flset->second.find(*comp_it) != flset->second.end();

				if(in_gt || in_list) {
					continue;
				}	
				output_collector.push_back(pid_it->first);
				output_collector.push_back(*comp_it);
				output_collector.push_back(next_pos);
				next_pos++;
				if(cut_to_size != 0 && next_pos > cut_to_size){
					break;
				}
			}
		}
	}

    return Eigen::Map<MatrixXiRM>(output_collector.data(), output_collector.size() / 3, 3);
}

namespace py = pybind11;

PYBIND11_MODULE(listdiff, m) {
	m.def(
		"listdiff",
		[&default_mat, &default_vec](
			const Eigen::Ref<const MatrixXi> &list_to_filter,
			const Eigen::Ref<const MatrixXi> &ground_truth,
			const Eigen::Ref<const MatrixXi> &complement = default_mat,
			const Eigen::Ref<const VectorXi> &extra_pids = default_vec,
			int cut_to_size = 0
		) -> MatrixXiRM {
			return listdiff(list_to_filter, ground_truth, complement, extra_pids, cut_to_size);
		},
		py::arg().noconvert(),
		py::arg().noconvert(),
		py::arg("complement").noconvert() = default_mat,
		py::arg("extra_pids").noconvert() = default_vec,
		py::arg("cut_to_size") = 0
	);

#ifdef VERSION_INFO
    m.attr("__version__") = VERSION_INFO;
#else
    m.attr("__version__") = "dev";
#endif
}