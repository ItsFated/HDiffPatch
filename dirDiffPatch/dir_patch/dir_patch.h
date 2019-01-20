// dir_patch.h
// hdiffz dir patch
//
/*
 The MIT License (MIT)
 Copyright (c) 2018-2019 HouSisong
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef DirPatch_dir_patch_h
#define DirPatch_dir_patch_h
#include "../../libHDiffPatch/HPatch/patch_types.h"
#include "../../libHDiffPatch/HPatch/checksum_plugin.h"
#include "ref_stream.h"
#include "new_stream.h"
#include "res_handle_limit.h"
#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct ICopyDataListener{
        void* listenerImport;
        void    (*copyedData)(struct ICopyDataListener* listener,const unsigned char* data,
                              const unsigned char* dataEnd);
    } ICopyDataListener;
    
hpatch_BOOL TDirPatcher_copyFile(const char* oldFileName_utf8,const char* newFileName_utf8,
                                 ICopyDataListener* copyListener);
hpatch_BOOL TDirPatcher_readFile(const char* oldFileName_utf8,ICopyDataListener* copyListener);
    
    
typedef struct TDirDiffInfo{
    hpatch_BOOL                 isDirDiff;
    hpatch_BOOL                 newPathIsDir;
    hpatch_BOOL                 oldPathIsDir;
    hpatch_BOOL                 dirDataIsCompressed;
    //compressType saved in hdiffInfo
    hpatch_compressedDiffInfo   hdiffInfo;
    hpatch_StreamPos_t          externDataOffset;
    hpatch_StreamPos_t          externDataSize;
    hpatch_StreamPos_t          checksumOffset;
    size_t                      checksumByteSize;
    char                        checksumType[hpatch_kMaxPluginTypeLength+1]; //ascii cstring
} TDirDiffInfo;


hpatch_BOOL getDirDiffInfo(const hpatch_TStreamInput* diffFile,TDirDiffInfo* out_info);
hpatch_BOOL getDirDiffInfoByFile(const char* diffFileName,TDirDiffInfo* out_info);
hpatch_inline static hpatch_BOOL getIsDirDiffFile(const char* diffFileName,hpatch_BOOL* out_isDirDiffFile){
    TDirDiffInfo info;
    hpatch_BOOL result=getDirDiffInfoByFile(diffFileName,&info);
    if (result) *out_isDirDiffFile=info.isDirDiff;
    return result;
}
    
    typedef struct _TDirDiffHead {
        size_t              oldPathCount;
        size_t              newPathCount;
        size_t              oldPathSumSize;
        size_t              newPathSumSize;
        size_t              oldRefFileCount;
        size_t              newRefFileCount;
        size_t              sameFilePairCount;
        hpatch_StreamPos_t  sameFileSize;
        hpatch_StreamPos_t  typesEndPos;
        hpatch_StreamPos_t  compressSizeBeginPos;
        hpatch_StreamPos_t  headDataOffset;
        hpatch_StreamPos_t  headDataSize;
        hpatch_StreamPos_t  headDataCompressedSize;
        hpatch_StreamPos_t  hdiffDataOffset;
        hpatch_StreamPos_t  hdiffDataSize;
    } _TDirDiffHead;
    
    struct TFileStreamInput;
    struct TFileStreamOutput;
    
    typedef struct IDirPatchListener{
        void*       listenerImport;
        hpatch_BOOL   (*makeNewDir)(struct IDirPatchListener* listener,const char* newDir);
        hpatch_BOOL (*copySameFile)(struct IDirPatchListener* listener,const char* oldFileName,
                                    const char* newFileName,ICopyDataListener* copyListener);
        hpatch_BOOL  (*openNewFile)(struct IDirPatchListener* listener,struct TFileStreamOutput*  out_curNewFile,
                                    const char* newFileName,hpatch_StreamPos_t newFileSize);
        hpatch_BOOL (*closeNewFile)(struct IDirPatchListener* listener,struct TFileStreamOutput* curNewFile);
    } IDirPatchListener;
    
    typedef struct TPatchChecksumSet{
        hpatch_TChecksum*   checksumPlugin;
        hpatch_BOOL         isCheck_oldRefData;
        hpatch_BOOL         isCheck_newRefData;  
        hpatch_BOOL         isCheck_copyFileData;
        hpatch_BOOL         isCheck_dirDiffData;
    } TPatchChecksumSet;

typedef struct TDirPatcher{
    TDirDiffInfo                dirDiffInfo;
    _TDirDiffHead               dirDiffHead;
    const char* const *         oldUtf8PathList;
    const char* const *         newUtf8PathList;
    const size_t*               oldRefList;
    const size_t*               newRefList;
    const hpatch_StreamPos_t*   newRefSizeList;
    const TSameFileIndexPair*   dataSamePairList; //new map to old index
    hpatch_BOOL                 isDiffDataChecksumError;
    hpatch_BOOL                 isNewRefDataChecksumError;
    hpatch_BOOL                 isOldRefDataChecksumError;
    hpatch_BOOL                 isCopyDataChecksumError;
//private:
    INewStreamListener          _newDirStreamListener;
    TNewStream                  _newDirStream;
    struct TFileStreamOutput*   _curNewFile;
    char*                       _newRootDir;
    char*                       _newRootDir_end;
    char*                       _newRootDir_bufEnd;
    void*                       _pNewRefMem;
    
    TRefStream                  _oldRefStream;
    TResHandleLimit             _resLimit;
    IResHandle*                 _resList;
    struct TFileStreamInput*    _oldFileList;
    char*                       _oldRootDir;
    char*                       _oldRootDir_end;
    char*                       _oldRootDir_bufEnd;
    void*                       _pOldRefMem;
    
    ICopyDataListener           _sameFileCopyListener;
    
    TPatchChecksumSet           _checksumSet;
    hpatch_checksumHandle       _newRefChecksumHandle;
    hpatch_checksumHandle       _sameFileChecksumHandle;
    unsigned char*              _pChecksumMem;
    
    IDirPatchListener*          _listener;
    hpatch_TDecompress*         _decompressPlugin;
    const hpatch_TStreamInput*  _dirDiffData;
    void*                       _pDiffDataMem;
} TDirPatcher;

hpatch_inline
static void TDirPatcher_init(TDirPatcher* self)  { memset(self,0,sizeof(*self)); }
hpatch_BOOL TDirPatcher_open(TDirPatcher* self,const hpatch_TStreamInput* dirDiffData,
                             const TDirDiffInfo**  out_dirDiffInfo);
//if checksumSet->isCheck_dirDiffData return  checksum(dirDiffData);
hpatch_BOOL TDirPatcher_checksum(TDirPatcher* self,const TPatchChecksumSet* checksumSet);

hpatch_BOOL TDirPatcher_loadDirData(TDirPatcher* self,hpatch_TDecompress* decompressPlugin);
hpatch_BOOL TDirPatcher_openOldRefAsStream(TDirPatcher* self,const char* oldPath_utf8,
                                           size_t kMaxOpenFileNumber,
                                           const hpatch_TStreamInput** out_oldRefStream);
hpatch_BOOL TDirPatcher_openNewDirAsStream(TDirPatcher* self,const char* newPath_utf8,
                                           IDirPatchListener* listener,
                                           const hpatch_TStreamOutput** out_newDirStream);

hpatch_BOOL TDirPatcher_patch(TDirPatcher* self,const hpatch_TStreamOutput* out_newData,
                              const hpatch_TStreamInput* oldData,
                              unsigned char* temp_cache,unsigned char* temp_cache_end);

hpatch_BOOL TDirPatcher_closeOldRefStream(TDirPatcher* self);//for TDirPatcher_openOldRefAsStream
hpatch_BOOL TDirPatcher_closeNewDirStream(TDirPatcher* self);//for TDirPatcher_openNewDirAsStream
hpatch_BOOL TDirPatcher_close(TDirPatcher* self);


#ifdef __cplusplus
}
#endif
#endif //DirPatch_dir_patch_h
