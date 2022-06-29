#include "builder.h"

ImageChain* Builder::CreatFromConfig(std::string path)
{
	std::ifstream configFile;
	// open files
	configFile.open(path.c_str());
	std::stringstream JFilterParamsStream;
	// read file's buffer contents into streams
	JFilterParamsStream << configFile.rdbuf();
	// close file handlers
	configFile.close();
	// convert stream into string
	std::string JFilterParamsString = JFilterParamsStream.str();
	cJSON* overallData = cJSON_Parse(JFilterParamsString.c_str());
	if (overallData) {
		filters_ = cJSON_GetObjectItem(overallData, "filters");
		if (filters_) {
			AnalyseFilters(filters_);
		}
		connect_ = cJSON_GetObjectItem(overallData, "connections");
		if (connect_) {
			ConnectPipeline(connect_);
		}
	}
	if (myInputs_.size() != 0) {
		return new ImageChain(myInputs_);
	}
	return nullptr;
}

void Builder::AnalyseFilters(cJSON* filters)
{
	int size = cJSON_GetArraySize(filters);
	for (int i = 0; i < size; i++)
	{
		cJSON* item = cJSON_GetArrayItem(filters, i);
		cJSON* type = cJSON_GetObjectItem(item, "type");
		cJSON* name = cJSON_GetObjectItem(item, "name");
		if (type && name) {
			nameType_[name->valuestring] = type->valuestring;
			Filter* tempFilter = CreatFilters(item);
			if (tempFilter)
			{
				nameFilter_[name->valuestring] = tempFilter;
			}
		}
	}
}

Filter* Builder::CreatBlurFilters(cJSON* filters, std::string filterName)
{
	float weights[3]{0};
	float offsets[3]{0};
	cJSON* weight = cJSON_GetObjectItemCaseSensitive(filters, "weight");
	if (weight)
	{
		for (int i = 0; i < RADIUS; i++) {
			cJSON* cjson_weight_item = cJSON_GetArrayItem(weight, i);
			if (cjson_weight_item)
			{
				weights[i] = cjson_weight_item->valuedouble;
			}
		}
	}

	cJSON* offset = cJSON_GetObjectItemCaseSensitive(filters, "offset");
	if (offset)
	{
		for (int i = 0; i < RADIUS; i++)
		{
			cJSON* cjson_offset_item = cJSON_GetArrayItem(offset, i);
			if (cjson_offset_item)
			{
				offsets[i] = cjson_offset_item->valuedouble;
			}
		}
	}
	
	Filter* result = nullptr;
	if (filterName == "GaussianBlur")
	{
		GaussianBlurFilter* temp = new GaussianBlurFilter();
		temp->SetValue("weights", weights, 3);
		temp->SetValue("offsets", offsets, 3);
		result = (Filter*)temp;
	}

	if (filterName == "HorizontalBlur")
	{
		HorizontalBlurFilter* temp = new HorizontalBlurFilter();
		temp->SetValue("weights", weights, 3);
		temp->SetValue("offsets", offsets, 3);
		result = (Filter*)temp;
	}
	if (filterName == "VerticalBlur")
	{
		VerticalBlurFilter* temp = new VerticalBlurFilter();
		temp->SetValue("weights", weights, 3);
		temp->SetValue("offsets", offsets, 3);
		result = (Filter*)temp;
	}
	return result;
}

Filter* Builder::CreatFilters(cJSON* filters)
{
	cJSON* type = cJSON_GetObjectItem(filters, "type");
	std::string filterName = type->valuestring;
	if (filterName == "")
	{
		return nullptr;
	}

	if (filterName == "GaussianBlur")
	{
		return CreatBlurFilters(filters, filterName);
	}

	if (filterName == "HorizontalBlur")
	{
		return CreatBlurFilters(filters, filterName);
	}
	if (filterName == "VerticalBlur")
	{
		return CreatBlurFilters(filters, filterName);
	}
	if (filterName == "Scale")
	{
		ScaleFilter* temp = new ScaleFilter();
		cJSON* scale = cJSON_GetObjectItem(filters, "scale");
		if (scale)
		{
			temp->SetScale(scale->valuedouble);
		}
		return (Filter*)temp;
	}
	if (filterName == "InputJPG")
	{
		cJSON* path = cJSON_GetObjectItem(filters, "path");
		if (path)
		{
			InputJPG* temp = new InputJPG(path->valuestring);
			return (Filter*)temp;
		}
		return nullptr;
	}	
	if (filterName == "InputPNG")
	{
		cJSON* path = cJSON_GetObjectItem(filters, "path");
		if (path)
		{
			InputPNG* temp = new InputPNG(path->valuestring);
			return (Filter*)temp;
		}
		return nullptr;
	}
	if (filterName == "OutFilterBMP")
	{
		OutFilterBMP* temp = new OutFilterBMP();
		cJSON* path = cJSON_GetObjectItem(filters, "path");
		if (path)
		{
			temp->SaveRenderResultToImage(path->valuestring);
		}
		return (Filter*)temp;
	}

	return nullptr;
}

void Builder::ConnectPipeline(cJSON* connect)
{
	int size = cJSON_GetArraySize(connect);
	for (int i = 0; i < size; i++)
	{
		cJSON* item = cJSON_GetArrayItem(connect, i);
		cJSON* from = cJSON_GetObjectItem(item, "from");
		cJSON* to = cJSON_GetObjectItem(item, "to");
		Filter* fFilter = nullptr;
		Filter* tFilter = nullptr;
		if (from && to) {
			std::string fTypeName = nameType_[from->valuestring];
			auto itFrom = nameFilter_.find(from->valuestring);
			if (itFrom != nameFilter_.end())
			{
				fFilter = itFrom->second;
				if (fFilter->GetFilterType() == filter_type::INPUT)
				{
					myInputs_.push_back((Input*)fFilter);
				}
			}
			auto itTo = nameFilter_.find(to->valuestring);
			if (itTo != nameFilter_.end())
			{
				tFilter = itTo->second;

			}
			if (fFilter && tFilter)
			{
				fFilter->AddNextFilter(tFilter);
			}
		}
	}
}

