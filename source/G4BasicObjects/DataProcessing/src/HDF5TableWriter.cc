/*
 * HDF5TableWriter.cc
 *
 * Generic HDF5 columnar writer base class.
 */

#include <HDF5TableWriter.hh>
#include <iostream>
#include <fstream>


HDF5TableWriter::HDF5TableWriter()
: File(nullptr)
, RunId(0)
{
}

HDF5TableWriter::~HDF5TableWriter()
{
	finalize();
}

void HDF5TableWriter::open(const std::string& filename, int runId)
{
	RunId = runId;

	if (!File)
	{
		Filename = filename;
		// Append to existing file, or create new
		std::ifstream test(filename.c_str());
		bool exists = test.good();
		test.close();

		if (exists)
			File = new H5::H5File(filename, H5F_ACC_RDWR);
		else
			File = new H5::H5File(filename, H5F_ACC_TRUNC);
	}

	clearAll();
}

void HDF5TableWriter::close()
{
	if (!File) return;

	// Check if run group already exists
	std::string runGroupName = "g4run_" + std::to_string(RunId);
	if (H5Lexists(File->getId(), runGroupName.c_str(), H5P_DEFAULT) > 0)
	{
		std::cerr << "HDF5TableWriter::close: group '" << runGroupName
		          << "' already exists in " << Filename << ", skipping write." << std::endl;
		clearAll();
		return;
	}

	writeDatasets();
	clearAll();

	// Close and flush the file so completed runs survive a crash
	finalize();
}

void HDF5TableWriter::finalize()
{
	if (File)
	{
		File->close();
		delete File;
		File = nullptr;
	}
}

// ---------- HDF5 writing helpers ----------

void HDF5TableWriter::writeIntDataset(H5::Group& group, const std::string& name, const std::vector<int>& data)
{
	if (data.empty()) return;
	hsize_t dims[1] = { data.size() };
	H5::DataSpace dataspace(1, dims);
	H5::DataSet dataset = group.createDataSet(name, H5::PredType::NATIVE_INT, dataspace);
	dataset.write(data.data(), H5::PredType::NATIVE_INT);
}

void HDF5TableWriter::writeDoubleDataset(H5::Group& group, const std::string& name, const std::vector<double>& data)
{
	if (data.empty()) return;
	hsize_t dims[1] = { data.size() };
	H5::DataSpace dataspace(1, dims);
	H5::DataSet dataset = group.createDataSet(name, H5::PredType::NATIVE_DOUBLE, dataspace);
	dataset.write(data.data(), H5::PredType::NATIVE_DOUBLE);
}

void HDF5TableWriter::writeStringDataset(H5::Group& group, const std::string& name, const std::vector<std::string>& data)
{
	if (data.empty()) return;
	hsize_t dims[1] = { data.size() };
	H5::DataSpace dataspace(1, dims);

	// Variable-length string type
	H5::StrType strType(H5::PredType::C_S1, H5T_VARIABLE);
	H5::DataSet dataset = group.createDataSet(name, strType, dataspace);

	// HDF5 C++ API needs an array of const char*
	std::vector<const char*> cStrings(data.size());
	for (size_t i = 0; i < data.size(); i++) {
		cStrings[i] = data[i].c_str();
	}
	dataset.write(cStrings.data(), strType);
}

void HDF5TableWriter::write2DDoubleDataset(H5::Group& group, const std::string& name, const std::vector<std::vector<double>>& data)
{
	if (data.empty()) return;
	hsize_t nrows = data.size();
	hsize_t ncols = 0;
	for (const auto& row : data) {
		if (row.size() > ncols) ncols = row.size();
	}
	if (ncols == 0) return;
	hsize_t dims[2] = { nrows, ncols };
	H5::DataSpace dataspace(2, dims);
	H5::DataSet dataset = group.createDataSet(name, H5::PredType::NATIVE_DOUBLE, dataspace);

	// Flatten row-major into a contiguous buffer, zero-padding shorter rows
	std::vector<double> flat(nrows * ncols, 0.0);
	for (hsize_t r = 0; r < nrows; r++) {
		for (hsize_t c = 0; c < data[r].size(); c++) {
			flat[r * ncols + c] = data[r][c];
		}
	}
	dataset.write(flat.data(), H5::PredType::NATIVE_DOUBLE);
}

H5::Group HDF5TableWriter::createOrderedGroup(H5::H5File& file, const std::string& name)
{
	hid_t gcpl = H5Pcreate(H5P_GROUP_CREATE);
	H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
	hid_t grp_id = H5Gcreate2(file.getId(), name.c_str(), H5P_DEFAULT, gcpl, H5P_DEFAULT);
	H5Pclose(gcpl);
	return H5::Group(grp_id);
}

H5::Group HDF5TableWriter::createOrderedGroup(H5::Group& parent, const std::string& name)
{
	hid_t gcpl = H5Pcreate(H5P_GROUP_CREATE);
	H5Pset_link_creation_order(gcpl, H5P_CRT_ORDER_TRACKED | H5P_CRT_ORDER_INDEXED);
	hid_t grp_id = H5Gcreate2(parent.getId(), name.c_str(), H5P_DEFAULT, gcpl, H5P_DEFAULT);
	H5Pclose(gcpl);
	return H5::Group(grp_id);
}
