// Copyright 2023 CSCE 311
//
#include <mmap_fstream.h>
#include <mmap_fstream.h>  // DO NOT DELETE

#include <cstddef>
#include <iostream>
#include <string>


bool TestConstructorOpenAndSize();

bool TestOpenIsOpenAndGet();

bool TestOpenIsOpenPut_newSizeAndGet();

bool TestOpenIsOpenSizePutAteAndGet();

int main(int argc, char* argv[]) {
  bool passed = true;
  if (argc == 1) {
    passed = passed && TestConstructorOpenAndSize();
    passed = passed && TestOpenIsOpenAndGet();
    passed = passed && TestOpenIsOpenPut_newSizeAndGet();
    passed = passed && TestOpenIsOpenSizePutAteAndGet();
    return passed ?  0 : 1;
  }

  switch (atoi(argv[1])) {
    case 0:
      return TestConstructorOpenAndSize() ? 0 : 1;
    case 1:
      return TestOpenIsOpenAndGet() ? 0 : 1;
    case 2:
      return TestOpenIsOpenPut_newSizeAndGet() ? 0 : 1;
    case 3:
      return TestOpenIsOpenSizePutAteAndGet() ? 0 : 1;
    default:
      std::cout << argv[1] << " is not a valid test [0, 3]" << std::endl;
  }

}


bool TestConstructorOpenAndSize() {
  const std::string kTestFileName = "test_open.txt";
  const std::size_t kTestSize = 10;

  std::cout << "TestConstructorOpenAndSize" << std::endl;

  mem_map::fstream fstream(kTestFileName, std::ios_base::in);
  std::cout << "\tExpected size: " << kTestSize << std::endl;
  std::cout << "\tActual size: " << fstream.size() << std::endl;

  if (fstream.size() == kTestSize) {
    fstream.close();
    std::cout << "\tResult: PASSED" << std::endl;
    return true;
  }

  std::cout << "\tResult: FAILED" << std::endl;
  fstream.close();
  return false;
}


bool TestOpenIsOpenAndGet() {
  const std::string kTestFileName = "test_get.txt";
  const std::size_t kTestSize = 4;
  const char kTestValues[] = {'w', 'x', 'y', 'z'};

  std::cout << "TestOpenIsOpenAndGet" << std::endl;

  mem_map::fstream fstream;
  fstream.open(kTestFileName, std::ios_base::in);
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << "\tActual is_open: " << (fstream.is_open() ? "true" : "false")
    << std::endl;
  if (!fstream.is_open()) {
    std::cout << "\tResult: FAILED" << std::endl;
    return false;
  }

  bool passed = true;
  for (std::size_t i = 0; i < kTestSize; ++i) {
    char c = fstream.get();
    std::cout << "\tExpected: " << kTestValues[i] << ", Actual: " << c
      << std::endl;
    if (kTestValues[i] != c)
      passed = false;
  }

  fstream.close();

  if (passed)
    std::cout << "\tResult: PASSED" << std::endl;
  else
    std::cout << "\tResult: FAILED" << std::endl;

  return passed;
}


bool TestOpenIsOpenPut_newSizeAndGet() {
  const std::string kTestFileName = "test_put.txt";
  const std::size_t kTestSize = 4;
  const char kTestValues[] = {'a', 'b', 'c', 'd'};

  std::cout << "TestOpenIsOpenPut_newSizeAndGet" << std::endl;

  // open file
  mem_map::fstream fstream_out;
  fstream_out.open(kTestFileName);
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << "\tActual is_open: "
    << (fstream_out.is_open() ? "true" : "false") << std::endl;
  if (!fstream_out.is_open()) {
    std::cout << "\tResult: FAILED" << std::endl;
    return false;
  }

  // write to file
  for (std::size_t i = 0; i < kTestSize; ++i)
    fstream_out.put(kTestValues[i]);

  fstream_out.close();

  // open file to read results of write
  bool passed = true;
  mem_map::fstream fstream_in(kTestFileName,
                              std::ios_base::in | std::ios_base::out);
  std::cout << "\tExpected size: " << kTestSize << std::endl;
  std::cout << "\tActual size: " << fstream_in.size() << std::endl;

  if (fstream_in.size() != kTestSize)
    passed = false;

  for (std::size_t i = 0; i < kTestSize; ++i) {
    char c = fstream_in.get();
    std::cout << "\tExpected: " << kTestValues[i] << ", Actual: " << c
      << std::endl;
    if (kTestValues[i] != c)
      passed = false;
  }

  fstream_in.close();

  if (passed)
    std::cout << "\tResult: PASSED" << std::endl;
  else
    std::cout << "\tResult: FAILED" << std::endl;

  return passed;
}


bool TestOpenIsOpenSizePutAteAndGet() {
  const std::string kTestFileName = "test_ate.txt";
  const std::size_t kTestSize = 4;
  const char kTestValues[] = { 'e', 'f', 'g', 'h' };
  const std::size_t kResultSize = 8;
  const char kResultValues[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };

  std::cout << "TestSizeIsOpenPutAteAndGet" << std::endl;

  // open file
  mem_map::fstream fstream_out;
  fstream_out.open(
    kTestFileName,
    std::ios_base::in | std::ios_base::out | std::ios_base::ate);
  // check open
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << "\tActual is_open: "
    << (fstream_out.is_open() ? "true" : "false") << std::endl;
  if (!fstream_out.is_open()) {
    std::cout << "\tResult: FAILED" << std::endl;
    return false;
  }

  // write to file
  for (std::size_t i = 0; i < kTestSize; ++i)
    fstream_out.put(kTestValues[i]);

  fstream_out.close();

  // open file to read results of write
  bool passed = true;
  mem_map::fstream fstream_in(kTestFileName, std::ios_base::in);
  std::cout << "\tExpected size: " << kResultSize << std::endl;
  std::cout << "\tActual size: " << fstream_in.size() << std::endl;

  if (fstream_in.size() != kResultSize) {
    std::cout << "\tResult: FALSE" << std::endl;
    passed = false;
  }

  for (std::size_t i = 0; i < kResultSize; ++i) {
    char c = fstream_in.get();
    std::cout << "\tExpected: " << kResultValues[i] << ", Actual: " << c
      << std::endl;
    if (kResultValues[i] != c)
      passed = false;
  }

  fstream_in.close();

  if (passed)
    std::cout << "\tResult: PASSED" << std::endl;
  else
    std::cout << "\tResult: FAILED" << std::endl;

  return passed;
}

