#ifndef _SHADER_LOADER_H_
#define _SHADER_LOADER_H_

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

class ShaderLoader
{
public:
	static std::string load_shader(const std::string& path)
	{
		std::fstream file;
		file.open(path, std::ios_base::in);

		std::stringstream ss;
		
		while(!file.eof())
		{
			std::string buffer;
			getline(file, buffer);
			ss<<buffer<<std::endl;
		}

		file.close();

		return ss.str();
	}	
};

#endif