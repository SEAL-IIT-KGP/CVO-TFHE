#ifndef CACHEUTILS_H
#define CACHEUTILS_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>

void CPU_affinity(int number)	{
	cpu_set_t set;
	int cpu_number = number;
	CPU_ZERO(&set);
	CPU_SET(cpu_number, &set);
	sched_setaffinity(getpid(), sizeof(set), &set);
}

uint64_t rdtsc_nofence() {
	uint64_t a, d;
	asm volatile ("rdtsc" : "=a" (a), "=d" (d));
	a = (d<<32) | a;
	return a;
}

uint64_t rdtsc() {
	uint64_t a, d;
	asm volatile ("mfence");
	asm volatile ("rdtsc" : "=a" (a), "=d" (d));
	a = (d<<32) | a;
	asm volatile ("mfence");
	return a;
}

static inline unsigned int timestamp(void)	{
	unsigned int bottom, top;
	asm volatile("xorl %%eax, %%eax\n cpuid \n" ::: "%eax", "%ebx", "%ecx", "%edx"); 
	asm volatile("rdtsc\n" : "=a" (bottom), "=d" (top) );	 
	asm volatile("xorl %%eax, %%eax\n cpuid \n" ::: "%eax", "%ebx", "%ecx", "%edx");
	return bottom;
}

static inline uint32_t memaccesstime(void *v) {
  uint32_t rv;
  asm volatile (
      "mfence\n"
      "lfence\n"
      "rdtscp\n"
      "mov %%eax, %%esi\n"
      "mov (%1), %%eax\n"
      "rdtscp\n"
      "sub %%esi, %%eax\n"
      : "=&a" (rv): "r" (v): "ecx", "edx", "esi");
  return rv;
}

void maccess(void* p)
{
	asm volatile ("movq (%0), %%rax\n"
		:
		: "c" (p)
		: "rax");
}

void flush(void* p) {
	asm volatile ("clflush 0(%0)\n"
		:
		: "c" (p)
		: "rax");
}

uint64_t rdtsc_begin() {
	uint64_t a, d;
	asm volatile ("mfence\n\t"
		"CPUID\n\t"
		"RDTSCP\n\t"
		"mov %%rdx, %0\n\t"
		"mov %%rax, %1\n\t"
		"mfence\n\t"
		: "=r" (d), "=r" (a)
		:
		: "%rax", "%rbx", "%rcx", "%rdx"
	);
	a = (d<<32) | a;
	return a;
}

uint64_t rdtsc_end() {
	uint64_t a, d;
	asm volatile("mfence\n\t"
		"RDTSCP\n\t"
		"mov %%rdx, %0\n\t"
		"mov %%rax, %1\n\t"
		"CPUID\n\t"
		"mfence\n\t"
		: "=r" (d), "=r" (a)
		:
		: "%rax", "%rbx", "%rcx", "%rdx"
	);
	a = (d<<32) | a;
	return a;
}

void prefetch(void* p)
{
	asm volatile ("prefetcht0 %0" : : "m" (p));
	asm volatile ("prefetcht1 %0" : : "m" (p));
	asm volatile ("prefetcht2 %0" : : "m" (p));
//	asm volatile ("prefetchtnta %0" : : "m" (p));
}

void longnop()
{
  asm volatile ("nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
                "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
                "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
                "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
                "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
                "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
                "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n"
                "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n");
}

#endif



