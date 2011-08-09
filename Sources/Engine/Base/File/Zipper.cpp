#include "Headers\toolspch.h"
#include "Engine\Base\File\zlib.h"
#include "Engine\Base\File\zip.h"
#include "Engine\Base\File\unzip.h"
#include "Engine\Base\DataObjects\ilwisobj.h"
#include "Engine\Base\DataObjects\ObjectStructure.h"
#include "Engine\Base\File\Zipper.h"


using namespace ILWIS;

Zipper::Zipper() {
}

Zipper::Zipper(const FileName& name) {
	files.push_back(name);
}

Zipper::Zipper(const vector<FileName>& names, const String& bs) {
	for(int i = 0; i < names.size(); ++i)
		files.push_back(names[i]);
	base = bs;

}

#define WRITEBUFFERSIZE (16384)

bool Zipper::zip(const FileName& fnZip) {


	void* buf=NULL;
	int size_buf=0;
	size_buf = WRITEBUFFERSIZE;
	buf = (void*)malloc(size_buf);
	if (!buf)
	{
		throw ErrorObject("Could allocate memory for reading zip file");
	}

	String path = fnZip.sRelative();
	zipFile zf = zf = zipOpen(fnZip.sFullName().c_str(),0);
	for(int i=0; i < files.size(); ++i) {
		FileName fnobj = files[i];
		list<String> finalList;
		if ( IOTYPE(fnobj) != IlwisObject::iotANY) {
		IlwisObject object = IlwisObject::obj(fnobj);
			if ( object.fValid()) {
				ObjectStructure ostruct;
				object->GetObjectStructure(ostruct);
				ostruct.GetUsedFiles(finalList, false);
			} else {
				finalList.push_back(fnobj.sFullPath());
			}
		} else {
			finalList.push_back(fnobj.sFullPath());
		}

		FILE * fin;
		int size_read;

		zip_fileinfo zi;
		int opt_compress_level=Z_BEST_COMPRESSION;
		//Best compression to save bandwidth at a maximum.
		set<String> finalSet;
		for(list<String>::iterator cur=finalList.begin(); cur != finalList.end(); ++cur)
			finalSet.insert((*cur));


		for(set<String>::iterator cur=finalSet.begin(); cur != finalSet.end(); ++cur) {
			String curfile = (*cur);
			FileName fnt(curfile);
			if ( base == "")
				curfile = fnt.sFile + fnt.sExt;
			else
				curfile = curfile.substr(base.size(), curfile.size() - base.size());
			zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = 
				zi.tmz_date.tm_mday = zi.tmz_date.tm_min = zi.tmz_date.tm_year = 0;
			zi.dosDate = 0;
			zi.internal_fa = 0;
			zi.external_fa = 0;
			int err = zipOpenNewFileInZip(zf, curfile.c_str(), &zi,NULL,0,NULL,0,NULL /* comment*/,(opt_compress_level != 0) ? Z_DEFLATED : 0,opt_compress_level);

			if (err != ZIP_OK)
			{
				throw ErrorObject(String("error in opening %S in zipfile\n",curfile));

			}
			else
			{
				fin = fopen(fnt.sFullName().c_str(),_T("rb"));
				if (fin==NULL)
				{
					throw ErrorObject(String("error in opening %S for reading\n",curfile));
				}
			}
			do
			{
				err = ZIP_OK;
				size_read = fread(buf,1,size_buf,fin);
				//if (size_read < size_buf)
				//    if (feof(fin)==0)
				//    {
				//        //Seems like we could not read from the temp name.
				//        AfxFormatString1(strMessage, AFX_IDP_FAILED_IO_ERROR_READ, szTempName);
				//        AfxMessageBox(strMessage);
				//        strMessage.Empty();
				//        err = ZIP_ERRNO;
				//    }

				if (err==ZIP_OK && size_read>0)
				{
					err = zipWriteInFileInZip (zf,buf,size_read);
					// if (err<0)
					//{
					//    //We could not write the file in the ZIP-File for whatever reason.
					//    AfxFormatString1(strMessage, AFX_IDP_FAILED_IO_ERROR_WRITE,strZipFile);
					//    AfxMessageBox(strMessage);
					//    strMessage.Empty();
					//}

				}
			}
			while (err==ZIP_OK && size_read>0);



			fclose(fin);
		}
	}
	int errclose = zipClose(zf,NULL);
	free (buf);

	return true;
}

void Zipper::unzip(const FileName& fn) {
	::unzip(fn);
}
