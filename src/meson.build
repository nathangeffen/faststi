cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
glib_dep = dependency('glib-2.0')
gsl_dep = dependency('gsl')
thread_dep = dependency('threads')

src = ['fsti-main.c', 'fsti-simset.c', 'fsti-procini.c', 'fsti-agent.c',
       'fsti-events.c', 'fsti-error.c', 'fsti-eventdefs.c', 'fsti-defaults.c',
       'fsti-config.c', 'fsti-simulation.c', 'fsti-userdefs.c', 'fsti-defs.c',
       'fsti-report.c', 'fsti-tests.c', 'fsti-dataset.c', 'fsti-generate.c',
       'utils/array.c', 'utils/algorithms.c',  'utils/array.c',
       'utils/csv.c', 'utils/test.c']


exe = executable('faststi', sources : src,
                 dependencies : [glib_dep, gsl_dep, m_dep, thread_dep],
                 install : true)
test('Unit tests', exe, args : '-t')


valgrind = find_program('valgrind', required : false)

if valgrind.found()
  test('Memory leak check', valgrind, args : ['--error-exitcode=1',
                                              '--leak-check=full',
                                              exe,
                                             '-m'])
else
  message('Valgrind not found, so not running memory leak tests')
endif
