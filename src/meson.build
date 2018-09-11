cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
glib_dep = dependency('glib-2.0')
gsl_dep = dependency('gsl')
useful_dep = dependency('useful')
thread_dep = dependency('threads')
#src = ['fsti-main.c', 'fsti-error.c', 'fsti-tests.c', 'fsti-config.c',
#       'fsti-defaults.c', 'fsti-procini.c', 'fsti-simset.c']

src = ['fsti-main.c', 'fsti-simset.c', 'fsti-procini.c', 'fsti-agent.c',
       'fsti-events.c', 'fsti-error.c', 'fsti-eventdefs.c', 'fsti-defaults.c',
       'fsti-config.c', 'fsti-simulation.c', 'fsti-defs.c', 'fsti-tests.c']


exe = executable('faststi', sources : src,
                 dependencies : [glib_dep, gsl_dep, useful_dep, m_dep, thread_dep],
                 install : true)
test('Unit tests', exe, args : '-t')


valgrind = find_program('valgrind', required : false)

if valgrind.found()
  test('Memory leak check', valgrind, args : ['--error-exitcode=1',
                                              '--leak-check=full',
                                              exe,
                                             '-t'])
else
  message('Valgrind not found, so not running memory leak tests')
endif
