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

echo "cd $location;$cmoddir/cmod ./ 0 18; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 1 18; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 2 18; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 3 18; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 4 18; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 5 18; exit" | ssh -T luigi &

echo "cd $location;$cmoddir/cmod ./ 6 18; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 7 18; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 8 18; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 9 18; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 10 18; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 11 18; exit" | ssh -T ben &

echo "cd $location;$cmoddir/cmod ./ 12 18; exit" | ssh -T herbert &

echo "cd $location;$cmoddir/cmod ./ 13 18; exit" | ssh -T john &

echo "cd $location;$cmoddir/cmod ./ 14 18; exit" | ssh -T james &

echo "cd $location;$cmoddir/cmod ./ 15 18; exit" | ssh -T sal &

echo "cd $location;$cmoddir/cmod ./ 16 18; exit" | ssh -T pierre &

echo "cd $location;$cmoddir/cmod ./ 17 18; exit" | ssh -T edgard &

