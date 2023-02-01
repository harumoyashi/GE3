#pragma once
#include "NMaterial.h"
#include <vector>

class NMtllib
{
public:
	std::vector<NMaterial> materials;

	static NMaterial Load(const std::string& directoryPath, const std::string& filename);
};