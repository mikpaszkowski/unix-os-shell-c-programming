#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m' # No Color

print(){
    echo -e "${RED}"$1"${NC}"
}

print_test_name() {
    echo -e "${GREEN}***** TEST "$1" - "$2" *****${NC}"
}

echo "
================================= modify.sh TESTS ======================================="


#***** test 1 *********
print_test_name 1 "should change filename to uppercase"

print "given:"

    mkdir test_dir
    touch test_dir/example.text

    ls -I modify_examples.sh -I modify.sh -R test_dir/

print "when:    bash modify.sh -u test_dir/example.text"

    bash modify.sh -u test_dir/example.text

print "then: "
    
    ls test_dir/

    # cleaning 
    rm -r test_dir/


echo "
"
#***** test 2 *********
print_test_name 2 "should change filename to lowercase"

print "given:"

    mkdir test_dir
    touch test_dir/EXAMPLE.text

    ls -I modify_examples.sh -I modify.sh -R test_dir/

print "when:    bash modify.sh -l test_dir/EXAMPLE.text"

    bash modify.sh -l test_dir/EXAMPLE.text

print "then: "
    
    ls test_dir/

    # cleaning 
    rm -r test_dir/


#***** test 3 *********
print_test_name 3 "should change filename according to sed pattern"

print "given:"

    mkdir test_dir
    touch test_dir/EXAMPLE_FILE.txt

    ls -I modify_examples.sh -I modify.sh -R test_dir/

print "when:    bash modify.sh 's/EXAMPLE/SED_PATTERN/g' test_dir/EXAMPLE_FILE.txt"

    bash modify.sh 's/EXAMPLE/SED_PATTERN/g' test_dir/EXAMPLE_FILE.txt

print "then: should change 'EXAMPLE' TO 'SED_PATTERN'"
    
    ls test_dir/

    # cleaning 
    rm -r test_dir/

#***** test 4 *********
print_test_name 4 "should change multiple filenames according to sed pattern"

print "given:"

    mkdir test_dir
    touch test_dir/EXAMPLE_FILE.txt
    touch test_dir/EXAMPLE_FILE_2.txt
    touch test_dir/EXAMPLE_FILE_3.txt

    ls -I modify_examples.sh -I modify.sh -R test_dir/

print "when:    bash modify.sh 's/EXAMPLE/SED_PATTERN/g' test_dir/EXAMPLE_FILE.txt test_dir/EXAMPLE_FILE_2.txt test_dir/EXAMPLE_FILE_2.txt"

    bash modify.sh 's/EXAMPLE/SED_PATTERN/g' test_dir/EXAMPLE_FILE.txt test_dir/EXAMPLE_FILE_2.txt test_dir/EXAMPLE_FILE_3.txt

print "then: should change 'EXAMPLE' TO 'SED_PATTERN' in all files"
    
    ls test_dir/

    # cleaning 
    rm -r test_dir/

#***** test 5 *********
print_test_name 5 "should not modify directory name"

print "given:"

    mkdir test_dir

    ls -I modify_examples.sh -I modify.sh 

print "when:    bash modify.sh -u test_dir/"

    bash modify.sh -u test_dir/

print "and:    bash modify.sh 's/test/NEW_TEST/g' test_dir/"

    bash modify.sh 's/test/NEW_TEST/g' test_dir/

print "then: appropriate error message displayed"
    
    ls -I modify_examples.sh -I modify.sh 

    #cleaning
    rm -r test_dir/

#***** test 7 *********
print_test_name 7 "should change file names recursively to uppercase"

print "given:"

    mkdir test_dir
    touch test_dir/example_1.txt
    mkdir test_dir/test_dir_2
    touch test_dir/test_dir_2/example_2.txt

    ls -I modify_examples.sh -I modify.sh -R test_dir/

print "when:    bash modify.sh -r -u test_dir/"

    bash modify.sh -r -u test_dir/

print "then: "

    ls -I modify_examples.sh -I modify.sh -R test_dir/

    # cleaning 
    rm -r test_dir/


#***** test 8 *********
print_test_name 8 "should change file names recursively to lowercase"

print "given:"

    mkdir test_dir
    touch test_dir/EXAMPLE_1.txt
    mkdir test_dir/test_dir_2
    touch test_dir/test_dir_2/EXAMPLE_2.txt

    ls -I modify_examples.sh -I modify.sh -R test_dir/

print "when:    bash modify.sh -r -l test_dir/"

    bash modify.sh -r -l test_dir/

print "then: "

    ls -I modify_examples.sh -I modify.sh -R test_dir/

    # cleaning 
    rm -r test_dir/

#***** test 9 *********
print_test_name 9 "should change file names recursively according to sed pattern"

print "given:"

    mkdir test_dir
    touch test_dir/example_1.txt
    mkdir test_dir/test_dir_2
    touch test_dir/test_dir_2/example_2.txt

    ls -I modify_examples.sh -I modify.sh -R test_dir/

print "when:    bash modify.sh -r 's/example/SED_PATTERN/g' test_dir/"

    bash modify.sh -r 's/example/SED_PATTERN/g' test_dir/

print "then: "

    ls -I modify_examples.sh -I modify.sh -R test_dir/

    # cleaning 
    rm -r test_dir/


#***** test 10 *********
print_test_name 10 "should display error message when modify non-existing file"

print "given: no files present in directory"
    ls -I modify_examples.sh -I modify.sh

print "when:    bash modify.sh -u file.txt"

    bash modify.sh -u file.txt

print "then: throw appropriate error message"

#***** test 11 *********
print_test_name 11 "should display error message no file provided"

print "given:"
    ls -I modify_examples.sh -I modify.sh

print "when:    bash modify.sh -u"

    bash modify.sh -u

print "then: throw appropriate error message"

#***** test 12 *********
print_test_name 12 "should modify file with many '.' in name"

print "given:"

    mkdir test_dir
    touch test_dir/config.d.ts

    ls -I modify_examples.sh -I modify.sh test_dir/

print "when:    bash modify.sh -u test_dir/config.d.ts"

    bash modify.sh -u test_dir/config.d.ts

print "then: should change only name to uppercase"

    ls -I modify_examples.sh -I modify.sh test_dir/

    # cleaning 
    rm -r test_dir/

#***** test 13 *********
print_test_name 13 "should modify multiple files passed to uppercase"

print "given:"

    touch config.d.ts
    touch config_2.d.ts
    touch config_3.d.ts

    ls -I modify_examples.sh -I modify.sh

print "when:    bash modify.sh -u config.d.ts config_2.d.ts config_3.d.ts"

    bash modify.sh -u config.d.ts config_2.d.ts config_3.d.ts

print "then: should change all files to uppercase"

    ls -I modify_examples.sh -I modify.sh

    #cleaning
    rm -r *.ts

#***** test 14 *********
print_test_name 14 "should display message and usage when no arguments provided"

print "when:    bash modify.sh"

    bash modify.sh

print "then: should display appriopriate error message"

#***** test 15 *********
print_test_name 15 "should display error message when -l and -u both flags provided"

print "given:"

    touch config.d.ts

    ls -I modify_examples.sh -I modify.sh

print "when:    bash modify.sh -l -u config.d.ts"

    bash modify.sh -l -u config.d.ts

print "then: should display appriopriate error message"

    rm -r *.ts

#***** test 16 *********
print_test_name 16 "should modify files in all firectories recursively"

print "given:"

    mkdir test_1_dir_1
    touch test_1_dir_1/example_1.txt
    mkdir test_1_dir_1/test_1_dir_2
    touch test_1_dir_1/test_1_dir_2/example_2.txt

    mkdir test_2_dir_1
    touch test_2_dir_1/example_1.txt
    mkdir test_2_dir_1/test_2_dir_2
    touch test_2_dir_1/test_2_dir_2/example_2.txt

    mkdir test_3_dir_1
    touch test_3_dir_1/example_1.txt
    mkdir test_3_dir_1/test_3_dir_2
    touch test_3_dir_1/test_3_dir_2/example_2.txt

    ls -I modify_examples.sh -I modify.sh -R test_1_dir_1/ test_2_dir_1/ test_3_dir_1/

print "when:    bash modify.sh -r -u test_1_dir_1/ test_2_dir_1/ test_3_dir_1/"

    bash modify.sh -r -u test_1_dir_1/ test_2_dir_1/ test_3_dir_1/

print "then: should change all files in directories to uppercase"

    ls -I modify_examples.sh -I modify.sh -R test_1_dir_1/ test_2_dir_1/ test_3_dir_1/

    #cleaning
    rm -r test_1_dir_1/ test_2_dir_1/ test_3_dir_1/