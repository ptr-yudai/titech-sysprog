#!/bin/sh
key=$1
cd `dirname $0`

rm -f ${key}-sh-out.txt
rm -f ${key}-sh-err.txt
rm -f ${key}-mysh-out.txt
rm -f ${key}-mysh-err.txt
rm -f ${key}-output.txt
rm -f ${key}-output2.txt
/bin/echo Hello_World! > ${key}-input.txt

sh t/$key 2>>${key}-sh-err.txt >>${key}-sh-out.txt
rm -f ${key}-output.txt
rm -f ${key}-output2.txt

cat t/$key | while read line; do
    ./mysh -q "`echo $line`" 2>>${key}-mysh-err.txt >>${key}-mysh-out.txt
done

if cmp ${key}-sh-out.txt ${key}-mysh-out.txt; then
    /bin/echo "✅   OK ${key} 🎉"
else
    /bin/echo Input command:
    cat t/$key | while read line; do
    	/bin/echo $line
    done
    /bin/echo -n "mysh:"
    cat ${key}-mysh-out.txt
    /bin/echo
    /bin/echo -n "  sh:"
    cat ${key}-sh-out.txt
    /bin/echo
    /bin/echo "❌   NG ${key} 😱";
fi

rm -f ${key}-output2.txt
rm -f ${key}-output.txt
rm -f ${key}-input.txt
