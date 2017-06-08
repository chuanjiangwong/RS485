
os_dir := Windows
file_ext := .exe

t_mconf = mconf_not_on_mingw
mconf_not_on_mingw:
	@echo ""
	@echo "The 'menuconfig' option is not supported in MinGW"
	@echo "Please use 'make config' instead"
	@echo ""
	@false

# Function to resolve input path
define b-abspath
$(join $(filter %:,$(subst :,: ,$(1))),$(abspath $(filter-out %:,$(subst :,: ,$(subst \,/,$(1))))))
endef
