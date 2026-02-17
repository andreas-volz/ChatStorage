
ChatStorageModuleTest_sources = files(
  'TestHelpers.cpp',
  'TestMain.cpp',
  'importer/ChatFormatAStreamParserTest.cpp'
  )

executable('ChatStorageModuleTest',
			ChatStorageModuleTest_sources,
			include_directories : [config_incdir],
			dependencies : [cppunit_dep, log4cxx_dep, libchatstorage_dep],
			install : false)
