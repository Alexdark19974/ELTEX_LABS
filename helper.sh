#! /bin/bash

explain_build() 
{
	echo "[build]"
	echo "action: build a lab, multiple labs, or all labs"
	echo "option: variable 'LAB' can be set to specify a particular lab (defaults to LAB=all)"
	echo "example 1: make build LAB=LAB_11"
	echo "example 2: make build (all labs are to be built)"
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
	echo "---------------------------------"
	echo ""
}

explain_variables()
{
	echo "[variables/options]"
	echo "description: useful variables passed to make to alter build behavior"
	echo ""
	echo "USE_DEBUG=(y/n): build labs with debug information to be examined;"
	echo ""
	echo "USE_GDB=(y/n): start program using gdb for debugging purposes (USE_DEBUG must be set to 'y');"
	echo ""
	echo "USE_VALGRIND=(y/n): start program using valgrind for debugging purposes (USE_DEBUG must be set to 'y');"
	echo ""
	echo "LAB=(LAB_*/all): build a lab or all labs;"
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
