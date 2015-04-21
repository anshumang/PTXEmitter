#include <cassert>
#include <iostream>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <dlfcn.h>
#include "ptx_from_fatbin.hpp"

extern "C"
void** __cudaRegisterFatBinary(void *fatCubin) 
{
  void *cudart = dlopen("/usr/local/cuda-6.0/lib64/libcudart.so", RTLD_NOW);
  assert(cudart);
  void** (*fptr)(void *);
  fptr = (void** (*)(void *))dlsym(cudart, "__cudaRegisterFatBinary");
  dlclose(cudart);
  ptx_from_fatbin(fatCubin);
  return (*fptr)(fatCubin);
}
