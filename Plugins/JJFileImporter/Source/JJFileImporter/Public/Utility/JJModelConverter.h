#pragma once

struct FJJModel;
struct FMeshDescription;

class JJFILEIMPORTER_API JJModelConverter
{
public:
	static FMeshDescription ConvertJJModelToMeshDescription(const FJJModel& ModelToConvert);
	
};
