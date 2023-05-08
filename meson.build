project('imagev', 'cpp', version: '2.0', meson_version: '>= 1.1',
  default_options: [
    'b_lto=true', 'b_ndebug=if-release', 'cpp_std=c++17', 'warning_level=2',
  ])

if host_machine.system() == 'darwin'
  add_languages('objcpp', native: false)
endif

cpp = meson.get_compiler('cpp')
config = configuration_data({
  # std::filesystem::path is not available before macOS 10.15
  'HAVE_STD_FILESYSTEM':
    cpp.has_header_symbol('filesystem', 'std::filesystem::path::preferred_separator'),
})

glfw_dep       = dependency('glfw3')
mpv_dep        = dependency('mpv')
if config.get('HAVE_STD_FILESYSTEM')
  boost_fs_dep = dependency('', required: false)
else
  boost_fs_dep = dependency('boost', modules: ['filesystem'])
endif
icu_i18n_dep   = dependency('icu-i18n', required: false)
icu_uc_dep     = dependency('icu-uc', required: false)

subdir('src')
