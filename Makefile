# ----- PROJECT INFO ----- 

PROJ_NAME := dl
PROJ_DESC := "directory listing"
PROJ_VER := 1.2

# ----- COMPILER CONFIGURATION -----

CC := gcc
CC_OPTS := \
	-Wall \
	-Werror \
	# -g \
	# -DDEBUG_MODE
CC_LIBS := \
	-largvp \
	-lfileio \
	-lerr \
	-lmem \
	-lprint

# ----- REAL TARGETS -----

.ver: build/bin/${PROJ_NAME}
	@ver -n ${PROJ_NAME} -d ${PROJ_DESC}
	@echo "Updated project metadata"

.__app__:
	@touch .__app__
	@echo "Updated application marker file"

build/bin/${PROJ_NAME}: main.c Makefile
	@mkdir -p build/bin
	@${CC} ${CC_OPTS} -o $@ $< ${CC_LIBS}
	@echo "Built target $@"

# ----- PHONY TARGETS -----

.PHONY: \
	all \
	clean \
	init \
	init-git \
	install \
	uninstall \
	ver

all: build/bin/${PROJ_NAME} .ver .__app__
	@echo "Built ${PROJ_NAME} version ${PROJ_VER} (build $$(ver -b))"

clean:
	@rm -rf build
	@echo "All build artifacts removed"

init:
	@touch main.c
	@echo "Created main.c"    
	@touch HELP.txt
	@echo "Created HELP.txt"
	@touch .gitignore
	@echo "Created .gitignore"
	@touch .__app__
	@echo "Created .__app__"
	@echo "Project successfully initialized"

init-git:
	@git init
	@echo "Initialized empty git repository"
	@git add --all
	@echo "Added all existing files to the repository"
	@git commit -m "v${PROJ_VER}; init commit"
	@echo "Created first commit"
	@git branch -M main
	@echo "Set the primary branch to 'main'"
	@git remote add origin git@github.com:kawright/${PROJ_NAME}.git
	@echo "Linked repository to remote GitHub repository"
	@git push -u origin main
	@echo "Pushed first commit to remote repository"
	@echo "Git repository successfully initialized"

install: build/bin/${PROJ_NAME}
	@mkdir -p /usr/local/etc/${PROJ_NAME}	
	@cp build/bin/${PROJ_NAME} /usr/local/bin/${PROJ_NAME}
	@echo "Copied ${PROJ_NAME}"
	@cp VERSION.txt /usr/local/etc/${PROJ_NAME}/VERSION.txt
	@echo "Copied VERSION.txt"
	@cp HELP.txt /usr/local/etc/${PROJ_NAME}/HELP.txt
	@echo "Copied HELP.txt"
	@cp .ver /usr/local/etc/${PROJ_NAME}/.ver
	@echo "Copied .ver"
	@cp .__app__ /usr/local/etc/${PROJ_NAME}/.__app__
	@echo "Copied .__app__"
	@echo "${PROJ_NAME} version ${PROJ_VER} successfully installed"

uninstall:
	@rm /usr/local/bin/${PROJ_NAME}
	@echo "Deleted binary '${PROJ_NAME}'"
	@rm -rf /usr/local/etc/${PROJ_NAME}
	@echo "Deleted all configuration files"
	@echo "${PROJ_NAME} successfully uninstalled"

ver:
	@ver -V ${PROJ_VER}
	@echo "Updated version number to ${PROJ_VER}"

