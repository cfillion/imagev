configure_file(output: 'config.hpp', configuration: config)

src = files([
  'main.cpp',
  'options.cpp',
  'player.cpp',
  'playlist.cpp',
  'window.cpp',
])

deps = [glfw_dep, mpv_dep, boost_fs_dep, icu_i18n_dep, icu_uc_dep]

if icu_i18n_dep.found() and icu_uc_dep.found()
  src += 'comparators_icu.cpp'
elif host_machine.system() == 'darwin'
  src += 'comparators.mm'
else
  error('Missing collator implementation. Is ICU installed?')
endif

if host_machine.system() == 'darwin'
  src += files(['filesystem.mm', 'window.mm'])
  deps += dependency('appleframeworks', modules: ['AppKit'])
else
  src += files('filesystem.cpp')
endif

exe = executable('imagev', src, dependencies: deps, install: true)
test('runs', exe, args: ['--help'])
