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

echo "cd $location;$cmoddir/cmod ./ 0 30; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 1 30; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 2 30; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 3 30; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 4 30; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 5 30; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 6 30; exit" | ssh -T luigi &
echo "cd $location;$cmoddir/cmod ./ 7 30; exit" | ssh -T luigi &

echo "cd $location;$cmoddir/cmod ./ 8 30; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 9 30; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 10 30; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 11 30; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 12 30; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 13 30; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 14 30; exit" | ssh -T ben &
echo "cd $location;$cmoddir/cmod ./ 15 30; exit" | ssh -T ben &

echo "cd $location;$cmoddir/cmod ./ 16 30; exit" | ssh -T iannis &
echo "cd $location;$cmoddir/cmod ./ 17 30; exit" | ssh -T iannis &

echo "cd $location;$cmoddir/cmod ./ 18 30; exit" | ssh -T john &
echo "cd $location;$cmoddir/cmod ./ 19 30; exit" | ssh -T john &

echo "cd $location;$cmoddir/cmod ./ 20 30; exit" | ssh -T james &
echo "cd $location;$cmoddir/cmod ./ 21 30; exit" | ssh -T james &

echo "cd $location;$cmoddir/cmod ./ 22 30; exit" | ssh -T sal &
echo "cd $location;$cmoddir/cmod ./ 23 30; exit" | ssh -T sal &

echo "cd $location;$cmoddir/cmod ./ 24 30; exit" | ssh -T pierre &
echo "cd $location;$cmoddir/cmod ./ 25 30; exit" | ssh -T pierre &

echo "cd $location;$cmoddir/cmod ./ 26 30; exit" | ssh -T edgard &
echo "cd $location;$cmoddir/cmod ./ 27 30; exit" | ssh -T edgard &

echo "cd $location;$cmoddir/cmod ./ 28 30; exit" | ssh -T herbert &
echo "cd $location;$cmoddir/cmod ./ 29 30; exit" | ssh -T herbert &

