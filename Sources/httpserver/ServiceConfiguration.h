#pragma once

namespace ILWIS {
	class _export ServiceConfiguration {
	public:
		ServiceConfiguration(const String& dir="",const String& name="");
		FileName add(const String& dir="",const String& name="");
		void add(const FileName& fn);
		String get(const String& key) const;
		void set(const String& key, const String& value);
		void readConfigFile(FileName fnConfig);
		void flush(const FileName& fnC=FileName());

	private:
		map<String, String> config;
		FileName fnConfig;
		bool isChanged;
	

	};

}