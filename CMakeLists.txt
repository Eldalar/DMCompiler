cmake_minimum_required (VERSION 2.6)
project (DMCompiler CXX)
add_executable(DMCompiler input_stream.cpp
                          main.cpp
			  token.cpp
			  token_stream.cpp
			  parse_exception.cpp
			  file_reader.cpp)
target_compile_features( DMCompiler PRIVATE cxx_range_for )
