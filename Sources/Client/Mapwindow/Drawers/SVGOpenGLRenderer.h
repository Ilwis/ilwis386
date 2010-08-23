class SVGOpenGLRenderer {
public:
	SVGOpenGLRenderer();
	void addDocument(FileName& fn);
private:
	map<String, GLuint> drawings;


};