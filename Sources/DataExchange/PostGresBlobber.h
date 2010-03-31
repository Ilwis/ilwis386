class _export PostgresBlobber {
public:
	PostgresBlobber(const FileName& _fn);
	void setBuffer();
	const char* escapedRepresentation();
	static const int BUF_SIZE=1000;
private:
	FileName fn;
	char *escapedCharBuffer;

};