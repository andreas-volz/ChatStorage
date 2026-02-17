project('ChatStorage', 'cpp',
        default_options : [
        'cpp_std=c++17', # not newer as c++17 to ensure Android+iOS compatibility  
        'default_library=both'
        ]) 

# https://mesonbuild.com/Configuration.html
pkgdatadir = join_paths(get_option('datadir'), 'startsconverter')
conf = configuration_data()
conf.set_quoted('PACKAGE_DATA_DIR', join_paths(get_option('prefix'), pkgdatadir))
conf.set_quoted('PACKAGE_SOURCE_DIR', meson.current_source_dir())

configure_file(
  output: 'config.h',
  configuration: conf
)
config_incdir = include_directories('.', 'include')
add_project_arguments('-DHAVE_CONFIG_H', language: 'cpp')

sqlite_dep = dependency('sqlite3', required : true)

log4cxx_dep = dependency('liblog4cxx', required : false)
if log4cxx_dep.found()
	add_project_arguments('-DHAVE_LOG4CXX', language: 'cpp')
endif

cppunit_dep = dependency('cppunit', required : false)

subdir('src')
subdir('tools')

if cppunit_dep.found()
	subdir('test/module')
endif