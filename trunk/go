location=`cd $1;pwd`
if [ -e "$location/T" ]
then
echo "Path $location is a DISSCO project."
else
echo ""
echo "Error: Project path is not valid."
echo "Could not find: $location/T"
echo ""
echo "Specify relative or absolute path to the DISSCO project:"
echo "./go ../myproject"
echo ""
exit
fi

cmoddir=`pwd`

rm -f $location/*multi*
rm -f $location/SoundFiles/*multi*
echo "Starting nodes..."
echo "cd $location;nohup nice $cmoddir/cmod ./ 0 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 1 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 2 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 3 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 4 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 5 18 > /dev/null 2> /dev/null < /dev/null & hostname; exit" | ssh -T luigi

echo "cd $location;nohup nice $cmoddir/cmod ./ 6 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 7 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 8 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 9 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 10 18 > /dev/null 2> /dev/null < /dev/null & nohup nice $cmoddir/cmod ./ 11 18 > /dev/null 2> /dev/null < /dev/null & hostname; exit" | ssh -T ben

echo "cd $location;nohup nice $cmoddir/cmod ./ 12 18 > /dev/null 2> /dev/null < /dev/null & hostname; exit" | ssh -T herbert

echo "cd $location;nohup nice $cmoddir/cmod ./ 13 18 > /dev/null 2> /dev/null < /dev/null & hostname; exit" | ssh -T john

echo "cd $location;nohup nice $cmoddir/cmod ./ 14 18 > /dev/null 2> /dev/null < /dev/null & hostname; exit" | ssh -T james

echo "cd $location;nohup nice $cmoddir/cmod ./ 15 18 > /dev/null 2> /dev/null < /dev/null & hostname; exit" | ssh -T sal

echo "cd $location;nohup nice $cmoddir/cmod ./ 16 18 > /dev/null 2> /dev/null < /dev/null & hostname; exit" | ssh -T pierre

echo "cd $location;nohup nice $cmoddir/cmod ./ 17 18 > /dev/null 2> /dev/null < /dev/null & hostname; exit" | ssh -T edgard

./progress $location

