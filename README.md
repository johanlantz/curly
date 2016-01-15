# curly
Curly is a drop in extension of curl simplifying the usescases of GET, PUT and POST operations from memory buffers.

##Installation
Simply add curly.h and curly.c to your existing project and use the simple api in curly.h instead of the native curl api.

###Visual Studio conversion of gtest solution
If you want to use the gtest unit-tests and Visual Studio says the gtest-md.sln file can not be converted to your version of Visual Studio you must first open the gtest-md.sln solution manually so the conversion can be performed. Then just open the CurlyTest.sln file and it should be ok.
