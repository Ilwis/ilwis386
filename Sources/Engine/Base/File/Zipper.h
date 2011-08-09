#pragma once

namespace ILWIS {
class _export Zipper {
public:
	Zipper();
	Zipper(const FileName& name);
	Zipper(const vector<FileName>& names, const String& bs = "");
	bool zip(const FileName& output);
	void unzip(const FileName& fn);
private:
	vector<FileName> files;
	String base;
};
}