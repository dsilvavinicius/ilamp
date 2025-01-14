#include "ilamp.h"
#include "imp_project.h"
#include "ilamp_utils.h"

#include <vector>
#include <fstream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#define DllExport extern "C" __declspec (dllexport)



static std::unique_ptr<ILamp<float>> ilamp;
static std::vector<float> output_verts;
static std::unique_ptr<std::ofstream> ilamp_log;


DllExport void ILamp_BuildNdFile(const char* ilamp_project_filename)
{
	//
	// Creating nd file
	//
	imp_project ilp_prj(ilamp_project_filename);
	build_nd_file(ilp_prj.inputFiles, ilp_prj.filenameNd);
}

DllExport bool ILamp_RunLamp(const char* input_filename_nd, const char* output_filename_2d)
{
	//
	// Running lamp in order to generate 2d file from nd file
	//
	std::string lamp_script = std::getenv("ILAMP_LAMP_SCRIPT");

	if (!fs::exists(lamp_script))
	{
		std::cerr << "Lamp python script not found" << std::endl;
		return false;
	}

	std::stringstream lamp_cmd;
	lamp_cmd << "python " << lamp_script << ' ' << input_filename_nd << ' ' << output_filename_2d << " > lamp.log";
	std::system(lamp_cmd.str().c_str());
#if _DEBUG
	std::cout << std::ifstream("lamp.log").rdbuf();
#endif

	return true;
}


DllExport bool ILamp_LoadInputFiles(const char* filename_2d, const char* filename_Nd)
{
	ilamp.reset(new ILamp<float>());
	ilamp_log.reset(new std::ofstream("ilamp.log"));
	
	(*ilamp_log) << "Info : Ilamp dll initilized" << std::endl;

	if (!(ilamp->load_data_2d(filename_2d) && ilamp->load_data_Nd(filename_Nd)))
		return false;

	if (ilamp->verts_Nd.size() != ilamp->verts_2d.size())
	{
		(*ilamp_log) << "Error: <ILamp_LoadInputFiles> Vertex arrays do not have the same size. Abort" << std::endl;
		return false;
	}

	return true;
}



DllExport void ILamp_SetKdTree(uint16_t _kdtree_count, uint16_t _num_neighbours, uint16_t _knn_search_checks)
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_SetKdTree> ilamp not initilized" << std::endl;
		return;
	}
	
	ilamp->set_kdtree(_kdtree_count, _num_neighbours, _knn_search_checks);
}

DllExport void ILamp_Build()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_Build> ilamp not initilized" << std::endl;
		return;
	}

	try
	{
		ilamp->build();
	}
	catch (const std::exception& ex)
	{
		(*ilamp_log) << ex.what() << std::endl;
	}
}


DllExport bool ILamp_Execute(float x, float y)
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_RunILamp> ilamp not initilized" << std::endl;
		return false;
	}

	try
	{
		ilamp->execute(x, y);
		return true;
	}
	catch (const std::exception& ex)
	{
		(*ilamp_log) << "Error: <ILamp_RunILamp> " << x << ' ' << y << std::endl
			<< ex.what() << std::endl;
		return false;
	}
}

DllExport size_t ILamp_QRows()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_QRows> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->q.rows();
}

DllExport size_t ILamp_QCols()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_QCols> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->q.cols();
}


DllExport void ILamp_CopyQ(void* p_array_float_N)
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_CopyQ> ilamp not initilized" << std::endl;
		return;
	}

	float* p_array_float = (float*)p_array_float_N;

	// safeguard - pointer must be not null
	if (!p_array_float)
		return;

	size_t r = ilamp->q.rows();
	size_t c = ilamp->q.cols();
	size_t coords_count = ilamp->q.rows() * ilamp->q.cols();

	std::memcpy(p_array_float, ilamp->q.data(), coords_count * sizeof(float));
}

DllExport float ILamp_MinX()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_MinX> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->min_x;
}

DllExport float ILamp_MaxX()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_MaxX> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->max_x;
}

DllExport float ILamp_MinY()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_MinY> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->min_y;
}

DllExport float ILamp_MaxY()
{
	if (!ilamp)
	{
		(*ilamp_log) << "Error: <ILamp_MaxY> ilamp not initilized" << std::endl;
		return 0;
	}
	return ilamp->max_y;
}

