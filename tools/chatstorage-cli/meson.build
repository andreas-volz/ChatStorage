chatstorage_inspect_sources = files(
  'chatstorage-inspect.cpp'
)

chatstorage_import_sources = files(
  'chatstorage-import.cpp'
)

chatstorage_view_sources = files(
  'chatstorage-view.cpp'
)

executable('chatstorage-inspect',
 	chatstorage_inspect_sources,
  	dependencies : [libchatstorage_dep],
  	install : true
)

executable('chatstorage-import',
 	chatstorage_import_sources,
  	dependencies : [libchatstorage_dep],
  	install : true
)

executable('chatstorage-view',
 	chatstorage_view_sources,
  	dependencies : [libchatstorage_dep],
  	install : true
)