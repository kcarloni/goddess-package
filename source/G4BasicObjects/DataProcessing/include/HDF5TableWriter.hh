/*
 * HDF5TableWriter.hh
 *
 * Abstract base class for writing columnar data to HDF5 files.
 * Each "table" is an HDF5 group whose "columns" are 1-D datasets
 * of equal length.  Subclasses define the specific tables and
 * accumulation logic by overriding writeDatasets() and clearAll().
 */

#ifndef HDF5TABLEWRITER_HH_
#define HDF5TABLEWRITER_HH_

#include <string>
#include <vector>
#include <H5Cpp.h>


class HDF5TableWriter
{
public:
	HDF5TableWriter();
	virtual ~HDF5TableWriter();

	void open(const std::string& filename);
	void close();

protected:
	// Subclasses override these to define their schema
	virtual void writeDatasets() = 0;
	virtual void clearAll() = 0;

	// Reusable I/O helpers
	std::string Filename;
	void writeIntDataset(H5::Group& group, const std::string& name, const std::vector<int>& data);
	void writeDoubleDataset(H5::Group& group, const std::string& name, const std::vector<double>& data);
	void writeStringDataset(H5::Group& group, const std::string& name, const std::vector<std::string>& data);
	void write2DDoubleDataset(H5::Group& group, const std::string& name, const std::vector<std::vector<double>>& data);
	H5::Group createOrderedGroup(H5::H5File& file, const std::string& name);
};

#endif
