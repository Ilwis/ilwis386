class _export Downloader {
public:
	Downloader(const URL& _url);

	FileName download(const String& executionDir);
	FILE *getFile() { return file; }
	bool fUpdateTrq(long nr);
private:
	FILE *file;

    Tranquilizer trq;
	int count;

	URL url;

};