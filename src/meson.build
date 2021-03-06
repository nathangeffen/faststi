cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)
glib_dep = dependency('glib-2.0')
gsl_dep = dependency('gsl')
thread_dep = dependency('threads')

src = [
    'fsti-agent.c', 'fsti-defs.c', 'fsti-simulation.c', 'fsti-config.c',
    'fsti-error.c', 'fsti-procini.c', 'fsti-tests.c', 'fsti-dataset.c',
    'fsti-eventdefs.c', 'fsti-report.c', 'fsti-userdefs.c', 'fsti-defaults.c',
    'fsti-events.c', 'fsti-simset.c', 'fsti-pythoncalls.c',
    'utils/algorithms.c', 'utils/array.c', 'utils/csv.c', 'utils/test.c',
]

include_subdir = join_paths(dir_name, 'utils')

install_headers('fsti-agent.h', 'fsti-error.h', 'fsti-pythoncalls.h',
                'fsti-userdefs.h', 'fsti-config.h', 'fsti-eventdefs.h',
                'fsti-report.h', 'fsti-variants.h', 'fsti-dataset.h',
                'fsti-events.h', 'fsti-simset.h', 'fsti-defaults.h',
                'fsti.h', 'fsti-simulation.h', 'fsti-defs.h',
                'fsti-procini.h', 'fsti-tests.h', subdir: dir_name)

install_headers('utils/utils.h', 'utils/array.h', 'utils/algorithms.h',
                'utils/csv.h', 'utils/test.h', 'utils/uthash.h',
                subdir: include_subdir)

lib = shared_library(meson.project_name(),
                     sources : src,
                     dependencies : [glib_dep, gsl_dep, m_dep, thread_dep],
                     version : lib_version,
                     soversion : '0',
                     install : true)

env = environment()
env.set('FSTI_DATA', join_paths(meson.source_root(), 'data'))

exe = executable('faststi', sources : 'fsti-main.c',
                 dependencies : [glib_dep, gsl_dep, m_dep, thread_dep],
                 link_with : lib,
                 install : true)

test('Unit tests', exe, args: '-t', env: env)

valgrind = find_program('valgrind', required : false)

if valgrind.found()
  test('Memory leak check',
       valgrind,
       args : ['--error-exitcode=1',
               '--leak-check=full',
               exe,
               '-m'],
       env: env
  )
else
  message('Valgrind not found, so not running memory leak tests')
endif


# Pkgconfig
pkg = import('pkgconfig')
libs = '-l' + dir_name
h = [dir_name, ]
pkg.generate(libraries : libs,
             subdirs : h,
             version : meson.project_version(),
             name : meson.project_name(),
             filebase : dir_name,
             description : project_description)
