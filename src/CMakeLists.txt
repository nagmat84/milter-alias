set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(SYSTEMD REQUIRED IMPORTED_TARGET libsystemd)

add_executable(
	milter-alias
	daemon.c
	extfile.c
	extldap.c
	extstring.c
	ini_parser.c
	log.c
	main.c
	priv_data.c
	runtime_setting.c
	service_manager.c
	smfi.c
	smfi_cb.c
	string_array.c
)

target_compile_options(milter-alias PRIVATE -Wall -Wextra -pedantic -Werror)
target_compile_features(milter-alias PRIVATE c_std_11)
target_link_libraries(milter-alias PRIVATE milter Threads::Threads PkgConfig::SYSTEMD ldap lber)

