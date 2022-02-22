#!/bin/sh

# set project init variables
ProjectName="Kuafu"
Description="Simple and Fast Market Data Processor"
Author="Fudan CS Group"
Today=$(date '+%Y%m%d')
Version="0.1"
Machine="$(uname -s)"

ReplaceItems="\
s,Kuafu,${ProjectName},g;\
s,Simple and Fast Market Data Processor,${Description},g;\
s,Fudan CS Group,${Author},g;\
s,20220222,${Today},g;\
s,0.1,${Version},g;\
" 

echo "Rendering file templates"
case "${Machine}" in
    Linux*)  
        find . -type d -name .git -prune -o -type f -print0 | xargs -0 sed -i "${ReplaceItems}"
        ;;
    Darwin*) 
        find . -type d -name .git -prune -o -type f -print0 | LC_ALL=C xargs -0 sed -i "" "${ReplaceItems}"
        ;;
    *)
esac

# rename files
echo "Rendering file names"
mv "include/ProjectName.h" "include/${ProjectName}.h"
mv "src/ProjectName.cpp" "src/${ProjectName}.cpp"
mv "tests/ProjectNameTest.cpp" "tests/${ProjectName}Test.cpp"
mv "cmake/ProjectNameConfig.cmake" "cmake/${ProjectName}Config.cmake"

echo "All Done"

