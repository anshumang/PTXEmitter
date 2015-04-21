#include <cassert>
#include <iostream>
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <dlfcn.h>
#include "ptx_from_fatbin.hpp"

void ptx_from_fatbin(const void *cubin_ptr)
{
    assert(cubin_ptr != 0);
    if(*(int*)cubin_ptr == __cudaFatMAGIC) 
    {
      __cudaFatCudaBinary *binary = (__cudaFatCudaBinary *)cubin_ptr;
      assert(binary->ident != 0);
      std::cout << binary->ident << std::endl;
      assert(binary->ptx != 0);
      assert(binary->ptx->ptx != 0);
      int i=0;
      while(binary->ptx[i].ptx != 0){
        assert(binary->ptx[i].gpuProfileName != 0);
        std::cout << binary->ptx[i].gpuProfileName << std::endl;
        assert(binary->ptx[i].ptx != 0);
        std::cout << binary->ptx[i].ptx << std::endl;
        i++;
      }
    }
    else if(*(unsigned*)cubin_ptr == __cudaFatMAGIC2)
    {
	__cudaFatCudaBinary2* binary = (__cudaFatCudaBinary2*) cubin_ptr;

        printf("%x ", binary->magic);
        printf("%x ", binary->version);
        printf("%p ", binary->fatbinData);
        printf("%p ", binary->f);
        printf("\n");

	__cudaFatCudaBinary2Header* header = (__cudaFatCudaBinary2Header*) binary->fatbinData;
	char* base = (char*)(header + 1);
	long long unsigned int offset = 0;
	__cudaFatCudaBinary2EntryRec* entry = (__cudaFatCudaBinary2EntryRec*)(base);

        printf("%x ", header->magic);
        printf("%x ", header->version);
        printf("%llx ", header->length);
        printf("\n");


	while (!(entry->type & FATBIN_2_PTX) && offset < header->length) 
        {
	  entry = (__cudaFatCudaBinary2EntryRec*)(base + offset);
	  printf("%x ", entry->type);
	  printf("%x ", entry->binary);
	  printf("%llx ", entry->binarySize);
	  printf("%x ", entry->unknown2);
	  printf("%x ", entry->kindOffset);
	  printf("%x ", entry->unknown3);
	  printf("%x ", entry->unknown4);
	  printf("%x ", entry->name);
	  printf("%x ", entry->nameSize);
	  printf("%llx ", entry->flags);
	  printf("%llx ", entry->unknown7);
	  printf("%llx ", entry->uncompressedBinarySize);
	  printf("\n");

	  printf("%s\n", (char *)((char*)entry + entry->name));
	  printf("%lld %lld bytes\n", entry->binary, entry->binarySize);

	  offset += entry->binary + entry->binarySize;
	}
        printf("Found PTX\n");
        assert(entry->type & FATBIN_2_PTX);
	printf("%x ", entry->type);
	printf("%x ", entry->binary);
	printf("%llx ", entry->binarySize);
	printf("%x ", entry->unknown2);
	printf("%x ", entry->kindOffset);
	printf("%x ", entry->unknown3);
	printf("%x ", entry->unknown4);
	printf("%x ", entry->name);
	printf("%x ", entry->nameSize);
	printf("%llx ", entry->flags);
	printf("%llx ", entry->unknown7);
	printf("%llx ", entry->uncompressedBinarySize);
	printf("\n");

	printf("%s\n", (char *)((char*)entry + entry->name));
	printf("%lld (C)%lld B (U)%lld B\n", entry->binary, entry->binarySize, entry->uncompressedBinarySize);

        void* libz = dlopen("libz.so", RTLD_NOW);
        assert(libz);
        int (*uncompressptr)(unsigned char*, unsigned long*, const unsigned char*, unsigned long) = (int (*)(unsigned char*, unsigned long*, const unsigned char*, unsigned long))dlsym(libz, "uncompress");
        assert(uncompressptr);
        unsigned char* output = new unsigned char[entry->binarySize * 10]();
        unsigned long outputSize=entry->binarySize * 10;
        //for(int i=0; i<8; i++){
        //   printf("%llx  ", *((unsigned long long *)((char*)entry + entry->binary) + i));
        //}
        //printf("\n");
        int result = (*uncompressptr)((unsigned char*)output, (unsigned long*)&outputSize, (const unsigned char*)((char*)entry + entry->binary), (unsigned long) (entry->binarySize));
        //for(int i=0; i<8; i++){
        //   printf("%llx  ", *((unsigned long long *)output + i));
        //}
        //printf("\n");
        printf("Decompressed size : %lu\nPTX string : \n", outputSize);
        int i=0;
        for(; (output+i) && (i<outputSize) ; i++)
           printf("%c", output[i]);
        printf("Nb of bytes => %d\n", i);
        //assert(result == 0);
        dlclose(libz);

    }
    else
    {
        printf("Unrecognized CUDA FAT MAGIC 0x%x\n", *(int*)cubin_ptr);
    }
}
