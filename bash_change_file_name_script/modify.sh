#!/bin/sh

RED='\033[0;31m'
NC='\033[0m' # No Color

# Beware variables can be inherited from the environment. So
# it's important to start with a clean state if you're going to
# dereference variables while not being guaranteed that they'll
# be assigned to:
unset -v l_flag u_flag r_flag sed_mode SED_PATTERN FILE_NAME

#flags
l_flag=false
u_flag=false
r_flag=false
sed_mode=false
SED_PATTERN=
script_name="$(basename "$0")"

usage() {

	cat <<EOT 1>&2
Synopsis: 
	modify [OPTION]... [FILE|DIR]... 

Change the letter case of given files or direlsctories to uppercase, lowercase or according to given sed pattern.

Options:
	-l, 	- lowerize file namels
	-u, 	- uppercase file name
	-r, 	- files recursively 
	
Usage:  
	$name [ -r ] [ -u | -l] <dir/file names ...>
	$name [ -r ] [ <sed-pattern> <dir/file names ...>
   	$name [ -h ]

Important:
	The -r flag is OPTIONAL. You cannot use simultaneously -l and -u.
	
Examples:	
	$name -r -l file.c
	$name -u directory
	$name -help
	$name -r example_file.c

EOT
	exit 1
}

throw_error() {
	#expands to all of the command line arguments as separate words. It is diﬀerent from "$*" , which expands to
	#all of the arguments as a single word. It is halpful in looping through arguments
	for arg in "$@"; do
		echo 1>&2 -e "${RED}"$arg"${NC}\n"
	done
	exit
}

is_file_curr_script() {
	if test "$script_name" = "$1"; then
		throw_error "You cannot modify script name"
	fi
}

#arg $1 - filename
check_file() {

	is_file_curr_script $1

	#checking if the file/directory or sed pattern name were given
	if [ -z $1 ]; then
		throw_error "Filename was not provided" "Enter 'modify.sh -h' for more information"
	fi

	# checking if the file exists
	if ! [[ -e $1 ]]; then
		throw_error "No such file or directory" "Enter 'modify.sh -h' for more information"
	fi

	if ! [[ -d $1 || -f $1 ]]; then
		throw_error "Input file should be directory or regular file" "Enter 'modify.sh -h' for more information"
	fi
}

modify() {
	prev_filename=$(basename "$1")
	# Trim the longest match from the end
	basename=${prev_filename%%.*}
	# SHELL PARAMETER EXPANSION {parameter#word} - The word is expanded to produce a pattern and matched result of the 
	# value of parameter with the shortest matching pattern, trim shortest match from the beginning, ## - longest match from ther beggining
	extension=${prev_filename#*.}
	path=$(dirname "$1")


	if [[ $l_flag = true ]]; then
		basename_changed="$(echo "$basename" | tr [:upper:] [:lower:])"
	fi

	if [[ $u_flag = true ]]; then
		basename_changed="$(echo "$basename" | tr [:lower:] [:upper:])"
	fi

	if [[ $sed_mode = true ]]; then
		basename_changed="$(echo "$basename" | sed -s "$SED_PATTERN")"
	fi

	if [[ -d $1 ]]; then
		next_filename="${path}/${basename_changed}"
	else
		next_filename="${path}/${basename_changed}.${extension}"
	fi

	if test "${path}/$prev_filename" != "$next_filename"; then
		mv -- "$1" "$next_filename"
	fi
}

modify_with_recursion() {
	
	for file in "$1"/*; do

		if [[ -f "$file" ]]; then

			modify "$file"

		elif [[ -d "$file" ]];
		then
			modify_with_recursion "$file"
		else
			break
		fi
	done

}

# $# is the number of arguments, the number of positional parameters passed to scriptshell or shell function,
#positional parameters are temporarily replaced with the arguments to the function
#checking if there is no argument given
if test $# == 0; then
	throw_error "No arguments provided. Check usage below." $(usage)
fi

while getopts ":rluh" opt; do

	case ${opt} in
		r) r_flag=true ;;
		l) l_flag=true ;;
		u) u_flag=true ;;
		h | :) usage; exit;;
		*) throw_error "Invalid option. Please check usage below." ;;
	esac

	if test $l_flag = true && $u_flag = true; then
		throw_error "[PROHIBITED ACTION]" "You cannot provide both -u and -l flag"
	fi
done
shift $((OPTIND - 1))

if [[ "$l_flag" = false && "$u_flag" = false ]]; then
	SED_PATTERN=$1
	sed_mode=true
	shift
fi

check_file "$1"

FILE_NAME="$1"

if [[ -d $1 && $r_flag = false ]];
	then	
		throw_error "You cannot modify name of directory"
fi

while ! [ -z "$1" ]; do 

	is_file_curr_script $1
	
	if [[ $r_flag = true ]];
	then
		if [[ -d "$1" ]]; then
			modify_with_recursion "$1"
		else
			modify "$1"
		fi
	else
		if [[ -f "$1" ]]; then
			modify "$1"
		fi
	fi
	shift
done
