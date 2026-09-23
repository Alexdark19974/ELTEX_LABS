#! /bin/bash

explain_build() 
{
	echo "[build]"
	echo "action: build a lab, multiple labs, or all labs"
	echo "option: variable 'LAB' can be set to specify a particular lab (defaults to LAB=all)"
	echo "example 1: make build LAB=LAB_11"
	echo "example 2: make build (all labs are to be built)"
	echo "example 3: make build_LAB_14 (same as example 1; also make build_all)"
	echo "---------------------------------"
	echo ""
}

explain_clean()
{
	echo "[clean]"
	echo "action: clean a lab, multiple labs, or all labs by removing build artifacts"
	echo "option: variable 'LAB' can be set to specify a particular lab (defaults to LAB=all)"
	echo "example 1: make clean LAB=LAB_11"
	echo "example 2: make clean (all labs are to be cleaned)"
	echo "example 3: make clean_LAB_14 (same as example 1; also make clean_all)"
	echo "---------------------------------"
	echo ""
}

explain_cppcheck()
{
	echo "[cppcheck]"
	echo "action: run cppcheck on source files in a lab, multiple labs, or all labs"
	echo "option: variable 'LAB' can be set to specify a particular lab (defaults to LAB=all)"
	echo "example 1: make cppcheck LAB=LAB_11"
	echo "example 2: make cppcheck (all labs are to be cppchecked)"
	echo "example 3: make cppcheck_LAB_14 (same as example 1; also make cppcheck_all)"
	echo "---------------------------------"
	echo ""
}

explain_variables()
{
	echo "[variables/options]"
	echo "description: useful variables passed to make to alter build behavior"
	echo ""
	echo "DEBUG=(y/n): build labs with debug information (-g3 -O0). Default is n."
	echo "example: make build LAB=LAB_11 DEBUG=y"
	echo ""
	echo "LAB=(LAB_*/all): build, clean, or cppcheck one lab or all labs. Default is all."
	echo ""
	echo "Inside a task directory, after a build:"
	echo "  make run            run the program"
	echo "  make run_vlgrnd     run it under valgrind"
	echo "  PROG=<name> make run   choose which binary, when a task builds several"
	echo ""
	echo "---------------------------------"
	echo ""

}

display_help()
{
	echo "====== COMMAND AND NAVIGATION HELPER ======"
	echo "Command list: 1) build 2) clean 3) cppcheck 4) help"
	echo ""
	explain_build
	explain_clean
	explain_cppcheck
	explain_variables
	echo "==========================================="
}

display_help

exit 0
