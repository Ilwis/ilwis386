#pragma once

namespace ILWIS {
	class _export ServiceConfiguration {
	public:
		ServiceConfiguration(const String& dir="",const String& name="");
		void add(const String& dir="",const String& name="");
		String get(const String& key) const;
		void readConfigFile(FileName fnConfig);

	private:
		map<String, String> config;
	

	};

}