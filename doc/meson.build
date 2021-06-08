doxygen = find_program('doxygen', required: false)

if not doxygen.found()
  warning('Doxygen not found. Documentation disabled.')
  subdir_done()
endif

a3_readme = meson.project_source_root() + '/README.md'

doxyfile_config = configuration_data(
  {
    'version': meson.project_version(),
    'output': meson.current_build_dir(),
    'input': meson.project_source_root() + '/src/include/a3 ' + a3_readme
  }
)

doxyfile = configure_file(input: 'Doxyfile.in', output: 'Doxyfile', configuration: doxyfile_config)

a3_doc = custom_target(
  'doc',
  command: [doxygen, doxyfile],
  output: ['html', 'latex'],
  depends: a3_lib,
  depend_files: a3_readme,
  build_by_default: false
)

a3_doc_target = run_target('doc', command: 'echo', depends: a3_doc)
