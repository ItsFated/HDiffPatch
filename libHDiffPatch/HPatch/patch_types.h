//  patch_types.h
//
/*
 The MIT License (MIT)
 Copyright (c) 2012-2017 HouSisong
 
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

#ifndef HPatch_patch_types_h
#define HPatch_patch_types_h
#ifdef __cplusplus
extern "C" {
#endif
    
    //hpatch_StreamPos_t for support large file
#ifdef _MSC_VER
    typedef unsigned __int64        hpatch_StreamPos_t;
#else
    typedef unsigned long long      hpatch_StreamPos_t;
#endif
    
    typedef void* hpatch_TStreamInputHandle;
    typedef void* hpatch_TStreamOutputHandle;
    
    typedef struct hpatch_TStreamInput{
        hpatch_TStreamInputHandle streamHandle;
        hpatch_StreamPos_t        streamSize;
        //read() must return (out_data_end-out_data), otherwise error
        long                      (*read) (hpatch_TStreamInputHandle streamHandle,
                                           const hpatch_StreamPos_t readFromPos,
                                           unsigned char* out_data,unsigned char* out_data_end);
    } hpatch_TStreamInput;
    
    typedef struct hpatch_TStreamOutput{
        hpatch_TStreamOutputHandle streamHandle;
        hpatch_StreamPos_t         streamSize;
        //write() must return (out_data_end-out_data), otherwise error
        //   first writeToPos==0; the next writeToPos+=(data_end-data)
        long                       (*write)(hpatch_TStreamOutputHandle streamHandle,
                                            const hpatch_StreamPos_t writeToPos,
                                            const unsigned char* data,const unsigned char* data_end);
    } hpatch_TStreamOutput;
    

    #define hpatch_kMaxCompressTypeLength 64
    
    typedef void*  hpatch_decompressHandle;
    typedef struct hpatch_TDecompress{
        //error return 0.
        hpatch_decompressHandle  (*open)(struct hpatch_TDecompress* decompressPlugin,
                                         const char* compressType,
                                         hpatch_StreamPos_t dataSize,
                                         const struct hpatch_TStreamInput* codeStream,
                                         hpatch_StreamPos_t code_begin,
                                         hpatch_StreamPos_t code_end);//codeSize==code_end-code_begin
        void                     (*close)(struct hpatch_TDecompress* decompressPlugin,
                                          hpatch_decompressHandle decompressHandle);
        //decompress_part() must return (out_part_data_end-out_part_data), otherwise error
        long           (*decompress_part)(const struct hpatch_TDecompress* decompressPlugin,
                                          hpatch_decompressHandle decompressHandle,
                                          unsigned char* out_part_data,unsigned char* out_part_data_end);
    } hpatch_TDecompress;

    
    
    void memory_as_inputStream(hpatch_TStreamInput* out_stream,
                               const unsigned char* mem,const unsigned char* mem_end);
    void memory_as_outputStream(hpatch_TStreamOutput* out_stream,
                                unsigned char* mem,unsigned char* mem_end);
    

#ifdef __cplusplus
}
#endif
#endif
