inc = include_directories('.')

subdir('importer')
subdir('common')
subdir('core')
subdir('database')

chatstorage_lib_sources = files()

libchatstorage = library('chatstorage',
  chatstorage_lib_sources,
  importer_sources,
  common_sources,
  core_sources,
  database_sources,
  include_directories : config_incdir,
  dependencies : [sqlite_dep, log4cxx_dep],
  install : true)

libchatstorage_dep = declare_dependency(include_directories : [config_incdir, inc],
  link_with : libchatstorage,
  dependencies : [sqlite_dep, log4cxx_dep]
)

  
