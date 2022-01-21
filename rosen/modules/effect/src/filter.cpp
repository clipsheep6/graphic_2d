#include "filter.h"

void Filter::Process(ProcessDate& data)
{
	DoProcess(data);
	if (this->GetFilterType() == filter_type::ALGOFILTER) {
		std::swap(data.srcTextureID_, data.outputTextureID_);
	}
	if (GetNextFilter()) {
		GetNextFilter()->Process(data);
	}

}

void Filter::AddNextFilter(Filter* next)
{
	if (nextNum_ < nextPtrMax_)
	{
		next_ = next;
		if (next) {
			next->AddPreviousFilter(this);
		}
	}
	if (nextNum_ == nextPtrMax_) return;
	nextNum_++;
}

void Filter::AddPreviousFilter(Filter* previous)
{
	if (preNum_ < prePtrMax_)
	{
		previous_ = previous;
	}
	if (preNum_ == prePtrMax_) return;
	preNum_++;
}

Filter* Filter::GetNextFilter(void)
{
	return next_;
}

Filter* Filter::GetPreviousFilter(void)
{
	return previous_;
}

int Filter::GetInputNumber()
{
	return preNum_;
}

int Filter::GetOutputNumber()
{
	return nextNum_;
}

int Filter::GetMaxInputNumber()
{
	return prePtrMax_;
}

int Filter::GetMaxOutputNumber()
{
	return nextPtrMax_;
}
