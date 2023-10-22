// Copyright 2023 CSCE 311
//
#include <mmap_fstream.h>
#include <mmap_fstream.h>  // DO NOT DELETE, NOLINT

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>


bool TestConstructorOpenAndSize();

bool TestOpenIsOpenAndGet();

bool TestOpenIsOpenPut_newSizeAndGet();

bool TestOpenIsOpenSizePutAteAndGet();

bool TestOpenIsOpenPut_oldSizeAndGet();

int main(int argc, char* argv[]) {
  const std::vector<bool (*)()> kFstreamTests{
    TestConstructorOpenAndSize,
    TestOpenIsOpenAndGet,
    TestOpenIsOpenPut_newSizeAndGet,
    TestOpenIsOpenSizePutAteAndGet,
    TestOpenIsOpenPut_oldSizeAndGet
  };

  if (argc == 2) {
    if (kFstreamTests.at(::atoi(argv[1]))()) {
      std::cout << "Result: " << "PASSED" << std::endl;
      return 0;
    } else {
      std::cout << "Result: " << "FAILED" << std::endl;
      return 1;
    }
  }

  int i = 0;
  for (auto test : kFstreamTests) {
    bool test_passed = test();
    std::cout << "\tResult: " << (test_passed ? "PASSED" : "FAILED")
      << std::endl;
    i += test_passed ? 0 : 1;
  }

  std::cout << '\n'
    << (kFstreamTests.size() - i) << " of " << kFstreamTests.size()
    << " tests passed" << std::endl;

  return i == 0 ? 0 : 1;
}


bool TestConstructorOpenAndSize() {
  const std::string kTestFileName = "test_open.txt";
  const std::size_t kTestSize = 10;
  bool passed = true;

  std::cout << "TestConstructorOpenAndSize" << std::endl;

  // open file with constructor
  mem_map::fstream fstream(kTestFileName, std::ios_base::in);

  // does file test open
  std::cout << "\tExpected is_open(1): true" << std::endl;
  std::cout << std::boolalpha;  // print bool as text
  std::cout << "\tActual is_open(1): " << fstream.is_open() << std::endl;
  passed = fstream.is_open();

  // is file size correct
  std::cout << "\tExpected file size(1): " << kTestSize << std::endl;
  std::cout << "\tActual file size(1): " << fstream.size() << std::endl;
  passed = fstream.size() == kTestSize && passed;

  // calling open on an open file does nothing
  fstream.open(kTestFileName);

  // does file test open
  std::cout << "\tExpected is_open(2): true" << std::endl;
  std::cout << "\tActual is_open(2): " << fstream.is_open() << std::endl;
  passed = fstream.is_open() && passed;

  // is file size correct
  std::cout << "\tExpected file size(2): " << kTestSize << std::endl;
  std::cout << "\tActual file size(2): " << fstream.size() << std::endl;
  passed = fstream.size() == kTestSize && passed;


  fstream.close();
  // does file test closed
  std::cout << "\tExpected is_open(3): false" << std::endl;
  std::cout << "\tActual is_open(3): " << fstream.is_open() << std::endl;
  passed = !fstream.is_open() && passed;

  return passed;
}


bool TestOpenIsOpenAndGet() {
  const std::string kTestFileName = "test_get.txt";
  const std::size_t kTestSize = 4;
  const char kTestValues[] = {'w', 'x', 'y', 'z'};
  bool passed = true;

  std::cout << "TestOpenIsOpenAndGet" << std::endl;

  // use default constructor and test open
  mem_map::fstream fstream;
  fstream.open(kTestFileName, std::ios_base::in);
  passed = fstream.is_open();
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << std::boolalpha;
  std::cout << "\tActual is_open: " << passed << std::endl;

  std::size_t i = 0;
  char c = fstream.get();  // call mmap::fstream::get
  while (c) {  // run while c != \0
    if (i >= kTestSize) {  // check bounds
      passed = false;
      std::cout << "\tExpected get: 0, Actual get: " << static_cast<int>(c)
        << std::endl;
      break;
    }

    passed = kTestValues[i] == c && passed;

    std::cout << "\tExpected get: " << kTestValues[i]
      << ", Actual get: " << c << std::endl;

    c = fstream.get();  // get next character
    ++i;  // update bounds count
  }

  if (i == 0) {
    std::cout << "\tExpected get: " << kTestValues[i]
      << ", Actual get: 0" << std::endl;
    passed = false;
  }

  fstream.close();

  return passed;
}


bool TestOpenIsOpenPut_newSizeAndGet() {
  const std::string kTestFileName = "test_put.txt";
  const std::size_t kTestSize = 4;
  const char kTestValues[] = {'a', 'b', 'c', 'd'};
  bool passed = true;

  std::cout << "TestOpenIsOpenPut_newSizeAndGet" << std::endl;

  // open file
  mem_map::fstream fstream_out(kTestFileName);
  passed = fstream_out.is_open();
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << std::boolalpha;  // print bool as text
  std::cout << "\tActual is_open: " << fstream_out.is_open() << std::endl;

  // write to file
  for (std::size_t i = 0; i < kTestSize; ++i)
    fstream_out.put(kTestValues[i]);

  fstream_out.close();

  // open file to read results of write
  mem_map::fstream fstream_in(kTestFileName, std::ios_base::in);
  passed = fstream_in.size() == kTestSize && passed;
  std::cout << "\tExpected size: " << kTestSize << std::endl;
  std::cout << "\tActual size: " << fstream_in.size() << std::endl;

  std::size_t i = 0;
  char c = fstream_in.get();  // call mmap::fstream::get
  while (c) {  // run while c != \0
    if (i >= kTestSize) {  // check bounds
      passed = false;
      std::cout << "\tExpected get: 0, Actual get: " << static_cast<int>(c)
        << std::endl;
      break;
    }

    passed = kTestValues[i] == c && passed;

    std::cout << "\tExpected get: " << kTestValues[i]
      << ", Actual get: " << c << std::endl;

    c = fstream_in.get();  // get next character
    ++i;  // update bounds count
  }

  if (i == 0) {
    std::cout << "\tExpected get: " << kTestValues[i]
      << ", Actual get: 0" << std::endl;
    passed = false;
  }

  fstream_in.close();

  return passed;
}


bool TestOpenIsOpenSizePutAteAndGet() {
  const std::string kTestFileName = "test_ate.txt";
  const std::size_t kTestSize = 4;
  const char kTestValues[] = { 'e', 'f', 'g', 'h' };
  const std::size_t kResultSize = 8;
  const char kResultValues[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
  bool passed = true;

  std::cout << "TestSizeIsOpenPutAteAndGet" << std::endl;

  // open file
  mem_map::fstream fstream_out(
    kTestFileName,
    std::ios_base::in | std::ios_base::out | std::ios_base::ate);
  passed = fstream_out.is_open();
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << std::boolalpha;  // print bool as text
  std::cout << "\tActual is_open: " << fstream_out.is_open() << std::endl;

  // write to file
  for (std::size_t i = 0; i < kTestSize; ++i)
    fstream_out.put(kTestValues[i]);

  fstream_out.close();

  // open file to read results of write
  mem_map::fstream fstream_in(kTestFileName, std::ios_base::in);
  passed = fstream_in.size() == kResultSize && passed;
  std::cout << "\tExpected size: " << kResultSize << std::endl;
  std::cout << "\tActual size: " << fstream_in.size() << std::endl;

  std::size_t i = 0;
  char c = fstream_in.get();  // call mmap::fstream::get
  while (c) {  // run while c != \0
    if (i >= kResultSize) {  // check bounds
      passed = false;
      std::cout << "\tExpected get: 0, Actual get: " << static_cast<int>(c)
        << std::endl;
      break;
    }

    passed = kResultValues[i] == c && passed;

    std::cout << "\tExpected get: " << kResultValues[i]
      << ", Actual get: " << c << std::endl;

    c = fstream_in.get();  // get next character
    ++i;  // update bounds count
  }

  if (i == 0) {
    std::cout << "\tExpected get: " << kTestValues[i]
      << ", Actual get: 0" << std::endl;
    passed = false;
  }

  return passed;
}


bool TestOpenIsOpenPut_oldSizeAndGet() {
  const std::string kTestFileName = "test_put.txt";
  const std::size_t kTestSize = 4;
  const char kTestValues[] = {'b', 'b', 'c', 'd'};
  bool passed = true;

  std::cout << "TestOpenIsOpenPut_oldSizeAndGet" << std::endl;

  // open file
  mem_map::fstream fstream_out(kTestFileName);
  passed = fstream_out.is_open();
  std::cout << "\tExpected is_open: true" << std::endl;
  std::cout << std::boolalpha;  // print bool as text
  std::cout << "\tActual is_open: " << fstream_out.is_open() << std::endl;

  // write to file
  fstream_out.put(kTestValues[0]);

  fstream_out.close();

  // open file to read results of write
  mem_map::fstream fstream_in(kTestFileName, std::ios_base::in);
  passed = fstream_in.size() == kTestSize && passed;
  std::cout << "\tExpected size: " << kTestSize << std::endl;
  std::cout << "\tActual size: " << fstream_in.size() << std::endl;

  std::size_t i = 0;
  char c = fstream_in.get();  // call mmap::fstream::get
  while (c) {  // run while c != \0
    if (i >= kTestSize) {  // check bounds
      passed = false;
      std::cout << "\tExpected get: 0, Actual get: " << static_cast<int>(c)
        << std::endl;
      break;
    }

    passed = kTestValues[i] == c && passed;

    std::cout << "\tExpected get: " << kTestValues[i]
      << ", Actual get: " << c << std::endl;

    c = fstream_in.get();  // get next character
    ++i;  // update bounds count
  }

  if (i == 0) {
    std::cout << "\tExpected get: " << kTestValues[i]
      << ", Actual get: 0" << std::endl;
    passed = false;
  }

  fstream_in.close();

  return passed;
}
